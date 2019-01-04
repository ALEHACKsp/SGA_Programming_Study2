#include "stdafx.h"
#include "TerrainRender.h"

#include "Terrain.h"
#include "Patch.h"
#include "HeightMap.h"
#include "./Viewer/Frustum.h"

const float TerrainRender::MaxDistance = 500.0f;
const float TerrainRender::MinDistance = 20.0f;
const float TerrainRender::MaxTessellation = 6.0f;
const float TerrainRender::MinTessellation = 0.0f;

TerrainRender::TerrainRender(Material * material, Terrain * terrain)
	: material(material), terrain(terrain)//, bUseTessellation(true)
	, quadPatchVB(NULL), quadPatchIB(NULL)
	, layerMapArraySRV(NULL), blendMapSRV(NULL), heightMapSRV(NULL)
	, bvhVertexBuffer(NULL), bvhIndexBuffer(NULL)
	, bWireFrame(false)
	, heightMapTex(NULL), heightMapUAV(NULL)
{
	D3DXMatrixIdentity(&world);

	// Create cBuffer
	{
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(Buffer);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, &cBuffer);
		assert(SUCCEEDED(hr));
	}

	// Create brushBuffer
	{
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(BrushBuffer);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, &brushBuffer);
		assert(SUCCEEDED(hr));
	}

	frustum = new Frustum(500);

	pixelPickingShader = new Shader(Shaders + L"Homework/FastPicking.fx");

	rtv = new RenderTargetView(
		Context::Get()->GetViewport()->GetWidth(),
		Context::Get()->GetViewport()->GetHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT);

	dsv = new DepthStencilView(Context::Get()->GetViewport()->GetWidth(),
		Context::Get()->GetViewport()->GetHeight());
	render2D = new Render2D();
	render2D->Position(0, 100);
	render2D->Scale(300, 300);

	adjustHeightMapComputeShader = new Shader(Shaders + L"Homework/AdjustHeightMap.fx", true);

}

TerrainRender::~TerrainRender()
{
	SAFE_RELEASE(cBuffer);
	SAFE_RELEASE(brushBuffer);

	for (Patch* patch : patches)
		SAFE_DELETE(patch);

	SAFE_RELEASE(quadPatchVB);
	SAFE_RELEASE(quadPatchIB);

	SAFE_RELEASE(bvhVertexBuffer);
	SAFE_RELEASE(bvhIndexBuffer);

	SAFE_DELETE(layerMapArray);

	SAFE_RELEASE(blendMapSRV);

	SAFE_RELEASE(heightMapTex);
	SAFE_RELEASE(heightMapSRV);
	SAFE_RELEASE(heightMapUAV);

	SAFE_DELETE(frustum);

	SAFE_DELETE(pixelPickingShader);
	SAFE_DELETE(rtv);
	SAFE_DELETE(dsv);
	SAFE_DELETE(render2D);

	SAFE_DELETE(adjustHeightMapComputeShader);
}

void TerrainRender::Initialize()
{
	patchVertexRows = ((UINT)(terrain->Desc().HeightMapHeight - 1) / Terrain::CellsPerPatch) + 1;
	patchVertexCols = ((UINT)(terrain->Desc().HeightMapWidth - 1) / Terrain::CellsPerPatch) + 1;

	patchVerticesCount = patchVertexRows * patchVertexCols;
	patchQuadFacesCount = (patchVertexRows - 1) * (patchVertexCols - 1);

	//if (bUseTessellation == true)
	//{
	CalcAllPatchBoundsY();

	BuildQuadPatchVB();
	BuildQuadPatchIB();
	//}
	//else
	//{
	//	BuildPatches();
	//}

	CreateBlendMap();

	vector<wstring> textures;
	for (int i = 0; i < 5; i++)
		textures.push_back(terrain->Desc().layerMapFile[i]);
	layerMapArray = new TextureArray(textures, 512, 512);
	layerMapArraySRV = layerMapArray->GetSRV();

	terrain->GetHeightMap()->Build(&heightMapTex, &heightMapSRV, &heightMapUAV);

	Shader* shader = material->GetShader();
	shader->AsShaderResource("HeightMap")->SetResource(heightMapSRV);
	shader->AsShaderResource("LayerMapArray")->SetResource(layerMapArraySRV);
	shader->AsShaderResource("BlendMap")->SetResource(blendMapSRV);

	buffer.FogStart = 300.0f;
	buffer.FogRange = 200.0f;
	buffer.FogColor = D3DXCOLOR(1, 1, 1, 1);
	buffer.MinDistance = MinDistance;
	buffer.MaxDistance = MaxDistance;
	buffer.MinTessellation = MinTessellation;
	buffer.MaxTessellation = MaxTessellation;
	buffer.TexelCellSpaceU = 1.0f / terrain->Desc().HeightMapWidth;
	buffer.TexelCellSpaceV = 1.0f / terrain->Desc().HeightMapHeight;
	buffer.WorldCellSpace = terrain->Desc().CellSpacing;
	buffer.TexScale = D3DXVECTOR2(5.0f, 5.0f);

	brush.Type = 1;
	brush.Location = D3DXVECTOR3(0, 0, 0);
	brush.Range = 2;
	brush.Color = D3DXVECTOR3(0, 1, 0);
	brush.Capacity = 0.1f;

	pixelPickingShader->AsShaderResource("HeightMap")->SetResource(heightMapSRV);

	pixelPickingShader->AsScalar("Width")->SetFloat(1024);
	pixelPickingShader->AsScalar("Height")->SetFloat(1024);
}

void TerrainRender::Update()
{
	// Brush 
	{
		if (Keyboard::Get()->Press(VK_LCONTROL))
		{
			//Mouse::Get()->GetMoveValue 이거의 z 값 +- 값이 mouse wheel 값임

			if (ImGui::GetIO().MouseWheel > 0)
				brush.Capacity += 0.01f;
			else if (ImGui::GetIO().MouseWheel < 0)
				brush.Capacity -= 0.01f;
		}
		else
		{
			if (ImGui::GetIO().MouseWheel > 0)
				brush.Range++;
			else if (ImGui::GetIO().MouseWheel < 0)
				brush.Range--;
			if (brush.Range <= 0) brush.Range = 1;
		}

		

		if (Keyboard::Get()->Down('1')) brush.Type = 0;
		if (Keyboard::Get()->Down('2')) brush.Type = 1;
		if (Keyboard::Get()->Down('3')) brush.Type = 2;
		if (Keyboard::Get()->Down('4')) brush.Type = 3;
		if (Keyboard::Get()->Down('5')) brush.Type = 4;
	}

	if (Keyboard::Get()->Down('C')) bWireFrame = !bWireFrame;

	if (Pick(&pickPos) && !ImGui::IsAnyWindowHovered() && !ImGui::IsAnyItemActive()) {
		if (Mouse::Get()->Press(0)) {
			AdjustY();
		}
	}
}

void TerrainRender::PreRender()
{
	Context::Get()->GetViewport()->RSSetViewport();
	
	D3D::Get()->SetRenderTarget(rtv->RTV(), dsv->DSV());
	D3D::Get()->Clear(D3DXCOLOR(0,0,0,0), rtv->RTV(), dsv->DSV());

	frustum->GetPlanes(buffer.WorldFrustumPlanes);

	D3D11_MAPPED_SUBRESOURCE subResource;
	HRESULT hr = D3D::GetDC()->Map
	(
		cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource
	);
	assert(SUCCEEDED(hr));

	memcpy(subResource.pData, &buffer, sizeof(Buffer));
	D3D::GetDC()->Unmap(cBuffer, 0);

	hr = pixelPickingShader->AsConstantBuffer("CB_Terrain")->SetConstantBuffer(cBuffer);
	assert(SUCCEEDED(hr));

	UINT stride = sizeof(TerrainCP);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &quadPatchVB, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(quadPatchIB, DXGI_FORMAT_R16_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	pixelPickingShader->AsMatrix("World")->SetMatrix(world);
	pixelPickingShader->DrawIndexed(0, 0, patchQuadFacesCount * 4);
}

void TerrainRender::Render()
{
	// wheel test
	//ImGui::Text("Wheel %.1f", ImGui::GetIO().MouseWheel);

	ImGui::Text("%f %f %f", pickPos.x, pickPos.y, pickPos.z);

	brush.Location = pickPos;

	ImGui::Checkbox("WireFrame", &bWireFrame);

	frustum->GetPlanes(buffer.WorldFrustumPlanes);
	
	// CB_Terrain
	{
		D3D11_MAPPED_SUBRESOURCE subResource;
		HRESULT hr = D3D::GetDC()->Map
		(
			cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource
		);
		assert(SUCCEEDED(hr));

		memcpy(subResource.pData, &buffer, sizeof(Buffer));
		D3D::GetDC()->Unmap(cBuffer, 0);

		hr = material->GetShader()->AsConstantBuffer("CB_Terrain")->SetConstantBuffer(cBuffer);
		assert(SUCCEEDED(hr));
	}
	
	ImGui::Begin("Brush");

	if (ImGui::Button("Save HeightMap"))
	{
		func = bind(&TerrainRender::SaveHeightMap, this, placeholders::_1);
		Path::SaveFileDialog(L"", L"Raw Files(*.raw)\0*.raw\0", Textures + L"Terrain", func);
	}

	ImGui::SameLine(120);
	if (ImGui::Button("Load HeightMap"))
	{
		func = bind(&TerrainRender::LoadHeightMap, this, placeholders::_1);
		Path::OpenFileDialog(L"", L"Raw Files(*.raw)\0*.raw\0", Textures + L"Terrain", func);
	}

	ImGui::RadioButton("None", &brush.Type, 0);
	ImGui::SameLine(80);
	ImGui::RadioButton("Rect", &brush.Type, 1);
	ImGui::SameLine(160);
	ImGui::RadioButton("Circle", &brush.Type, 2);
	ImGui::RadioButton("Cone", &brush.Type, 3);
	ImGui::SameLine(80);
	ImGui::RadioButton("Flat", &brush.Type, 4);

	ImGui::DragInt("Range", &brush.Range, 1, 1, 50);
	ImGui::ColorEdit3("Color", (float*)&brush.Color);
	ImGui::DragFloat("Capacity", &brush.Capacity, 0.01f);

	ImGui::End();

	// CB_Brush
	{
		D3D11_MAPPED_SUBRESOURCE subResource;
		HRESULT hr = D3D::GetDC()->Map
		(
			brushBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource
		);
		assert(SUCCEEDED(hr));

		memcpy(subResource.pData, &brush, sizeof(BrushBuffer));
		D3D::GetDC()->Unmap(brushBuffer, 0);

		hr = material->GetShader()->AsConstantBuffer("CB_Brush")->SetConstantBuffer(brushBuffer);
		assert(SUCCEEDED(hr));
	}

	UINT stride = sizeof(TerrainCP);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &quadPatchVB, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(quadPatchIB, DXGI_FORMAT_R16_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	int pass = bWireFrame == true ? 1 : 0;

	material->GetShader()->AsMatrix("World")->SetMatrix(world);
	material->GetShader()->DrawIndexed(0, pass, patchQuadFacesCount * 4);

	//render2D->SRV(rtv->SRV());
	render2D->SRV(heightMapSRV);
	render2D->Render();
}

bool TerrainRender::Pick(OUT D3DXVECTOR3 * val)
{
	float x, y;
	D3DXVECTOR3 position = Mouse::Get()->GetPosition();
	D3DXCOLOR result = Textures::ReadPixel(rtv->Texture(), position.x, position.y);
	//x = (0x000000ff & result) >> 0;
	//y = (0x0000ff00 & result) >> 8;

	x = result.r * 1024;
	y = result.g * 1024;

	(*val) = D3DXVECTOR3(x, 0, y);

	return x + y > 0;
}

void TerrainRender::LoadHeightMap(wstring fileName)
{
	terrain->GetHeightMap()->Load(fileName);

	CreateBlendMap();

	SAFE_RELEASE(heightMapTex);
	SAFE_RELEASE(heightMapSRV);
	SAFE_RELEASE(heightMapUAV);

	terrain->GetHeightMap()->Build(&heightMapTex, &heightMapSRV, &heightMapUAV);

	material->GetShader()->AsShaderResource("HeightMap")->SetResource(heightMapSRV);
	pixelPickingShader->AsShaderResource("HeightMap")->SetResource(heightMapSRV);
}

void TerrainRender::SaveHeightMap(wstring fileName)
{
	//terrain->GetHeightMap()->Save(fileName, heightMapTex);
	terrain->GetHeightMap()->Save(fileName, heightMapSRV);

	//CreateBlendMap();

	//SAFE_RELEASE(heightMapTex);
	//SAFE_RELEASE(heightMapSRV);
	//SAFE_RELEASE(heightMapUAV);

	//terrain->GetHeightMap()->Build(&heightMapTex, &heightMapSRV, &heightMapUAV);

	//material->GetShader()->AsShaderResource("HeightMap")->SetResource(heightMapSRV);
	//pixelPickingShader->AsShaderResource("HeightMap")->SetResource(heightMapSRV);
}

void TerrainRender::CalcAllPatchBoundsY()
{
	patchBoundsY.assign(patchQuadFacesCount, D3DXVECTOR2());

	for (UINT row = 0; row < patchVertexRows - 1; row++)
		for (UINT col = 0; col < patchVertexCols - 1; col++)
			CalcPatchBoundsY(row, col);
}

void TerrainRender::CalcPatchBoundsY(UINT row, UINT col)
{
	UINT x0 = col * Terrain::CellsPerPatch;
	UINT x1 = (col + 1) * Terrain::CellsPerPatch;

	UINT y0 = row * Terrain::CellsPerPatch;
	UINT y1 = (row + 1) * Terrain::CellsPerPatch;

	float minY = FLT_MAX;
	float maxY = FLT_MIN;

	for (UINT y = y0; y <= y1; y++)
	{
		for (UINT x = x0; x <= x1; x++)
		{
			minY = min(minY, terrain->Data(y, x));
			maxY = max(maxY, terrain->Data(y, x));
		}
	}

	UINT patchID = row * (patchVertexCols - 1) + col;
	patchBoundsY[patchID] = D3DXVECTOR2(minY, maxY);
}

void TerrainRender::BuildQuadPatchVB()
{
	TerrainCP* patchVertices = new TerrainCP[patchVerticesCount];
	float halfWidth = 0.5f * terrain->Width();
	float halfDepth = 0.5f * terrain->Depth();

	float patchWidth = terrain->Width() / (float)(patchVertexCols - 1);
	float patchDepth = terrain->Depth() / (float)(patchVertexRows - 1);

	float du = 1.0f / (float)(patchVertexCols - 1);
	float dv = 1.0f / (float)(patchVertexRows - 1);

	for (UINT row = 0; row < patchVertexRows; row++)
	{
		float z = halfDepth - (float)row * patchDepth;
		for (UINT col = 0; col < patchVertexCols; col++)
		{
			float x = -halfWidth + (float)col * patchWidth;
			UINT vertId = row * patchVertexCols + col;

			patchVertices[vertId].Position = D3DXVECTOR3(x, 0, z);
			patchVertices[vertId].Uv = D3DXVECTOR2(col * du, row * dv);
		}
	}

	for (UINT row = 0; row < patchVertexRows - 1; row++)
	{
		for (UINT col = 0; col < patchVertexCols - 1; col++)
		{
			UINT patchID = row * (patchVertexCols - 1) + col;
			UINT vertID = row * patchVertexCols + col;
			patchVertices[vertID].BoundsY = patchBoundsY[patchID];
		}
	}

	D3D11_BUFFER_DESC desc = { 0 };
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = sizeof(TerrainCP) * patchVerticesCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = &patchVertices[0];

	HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &quadPatchVB);
	assert(SUCCEEDED(hr));

	SAFE_DELETE_ARRAY(patchVertices);
}

void TerrainRender::BuildQuadPatchIB()
{
	vector<WORD> indices;
	for (WORD row = 0; row < (WORD)patchVertexRows - 1; row++)
	{
		for (WORD col = 0; col < (WORD)patchVertexCols - 1; col++)
		{
			indices.push_back(row * (WORD)patchVertexCols + col);
			indices.push_back(row * (WORD)patchVertexCols + col + 1);
			indices.push_back((row + 1) * (WORD)patchVertexCols + col);
			indices.push_back((row + 1) * (WORD)patchVertexCols + col + 1);
		}
	}

	D3D11_BUFFER_DESC desc = { 0 };
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = sizeof(WORD) * indices.size();
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = &indices[0];

	HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &quadPatchIB);
	assert(SUCCEEDED(hr));
}

void TerrainRender::CreateBlendMap()
{
	HeightMap* heightMap = terrain->GetHeightMap();

	vector<D3DXCOLOR> colors;

	UINT height = heightMap->Height();
	UINT width = heightMap->Width();
	UINT maxHeight = (UINT)heightMap->MaxHeight();

	for (UINT y = 0; y < height; y++)
	{
		for (UINT x = 0; x < width; x++)
		{
			float elevation = heightMap->Data(y, x);

			D3DXCOLOR color = D3DXCOLOR(0, 0, 0, 0);

			if (elevation > maxHeight * (0.05f + Math::Random(-0.05f, 0.05f)))
			{
				// dark green grass texture
				color.r = elevation / maxHeight + Math::Random(-0.05f, 0.05f);
			}
			if (elevation > maxHeight * (0.4f + Math::Random(-0.15f, 0.15f)))
			{
				// stone texture
				color.g = elevation / maxHeight + Math::Random(-0.05f, 0.05f);
			}
			if (elevation > maxHeight * (0.75f + Math::Random(-0.1f, 0.1f)))
			{
				// snow texture
				color.a = elevation / maxHeight + Math::Random(-0.05f, 0.05f);
			}

			colors.push_back(color);
		}
	}

	//SmoothBlendMap(colors);
	//SmoothBlendMap(colors);

	//vector<UINT> colors8b(colors.size());
	//const float f = 255.0f;

	//vector<D3DXCOLOR>::iterator it = colors.begin();
	//for (UINT i = 0; it != colors.end(); it++, i++)
	//	colors8b[i] = ((UINT)((f * it->a)) << 24)
	//	+ ((UINT)((f * it->b)) << 16)
	//	+ ((UINT)((f * it->g)) << 8)
	//	+ ((UINT)((f * it->r)) << 0);

	DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	ID3D11Texture2D* texture = 0;
	//   Create Blend Texture2D
	{
		UINT _width = terrain->GetHeightMap()->Width();
		UINT _height = terrain->GetHeightMap()->Height();

		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = _width;
		desc.Height = _height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &colors[0];
		data.SysMemPitch = _width * sizeof(D3DXCOLOR);
		//data.SysMemSlicePitch = _width * _height * sizeof(D3DXCOLOR); // 3d할때만 쓴다고함

		HRESULT hr = D3D::GetDevice()->CreateTexture2D(&desc, &data, &texture);
		assert(SUCCEEDED(hr));
	}

	//   Create Shader Resource View (To . blendMapSRV)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		desc.Format = format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MostDetailedMip = 0;
		desc.Texture2D.MipLevels = 1;

		HRESULT hr = D3D::GetDevice()->CreateShaderResourceView(texture, &desc, &blendMapSRV);
		assert(SUCCEEDED(hr));
	}

	SAFE_RELEASE(texture);
	colors.clear();
	//colors8b.clear();
}

void TerrainRender::SmoothBlendMap(vector<D3DXCOLOR>& colors)
{
	HeightMap* heightMap = terrain->GetHeightMap();

	int height = (int)heightMap->Height();
	int width = (int)heightMap->Width();
	float maxHeight = heightMap->MaxHeight();

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			D3DXCOLOR sum = colors[x + y * height];
			UINT num = 1;
			for (int y1 = y - 1; y1 < y + 2; y1++)
			{
				for (int x1 = x - 1; x1 < x + 1; x1++)
				{
					if (heightMap->InBounds(y1, x1) == false)
						continue;
					sum += colors[x1 + y1 * height];
					num++;
				} // for(x1)
			} // for(y1)

			colors[x + y * height] = sum / (float)num;
		} // for(x)
	} // for(y)

}

void TerrainRender::AdjustY()
{
	// CB_Brush
	{
		D3D11_MAPPED_SUBRESOURCE subResource;
		HRESULT hr = D3D::GetDC()->Map
		(
			brushBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource
		);
		assert(SUCCEEDED(hr));

		memcpy(subResource.pData, &brush, sizeof(BrushBuffer));
		D3D::GetDC()->Unmap(brushBuffer, 0);

		hr = adjustHeightMapComputeShader->AsConstantBuffer("CB_Brush")->SetConstantBuffer(brushBuffer);
		assert(SUCCEEDED(hr));
	}


	adjustHeightMapComputeShader->AsUAV("Output")->SetUnorderedAccessView(heightMapUAV);

	UINT width = 256;
	UINT height = 256;

	adjustHeightMapComputeShader->Dispatch(0, 0, width, height, 1);
}

