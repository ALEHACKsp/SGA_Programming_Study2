#include "Framework.h"
#include "TerrainRender.h"

#include "Terrain.h"
#include "Patch.h"
#include "HeightMap.h"
#include "./Viewer/Frustum.h"

#include "Objects\Billboard.h"

#include "Command\BrushCommand.h"
#include "Command/BillboardCommand.h"

const float TerrainRender::MaxDistance = 500.0f;
const float TerrainRender::MinDistance = 20.0f;
const float TerrainRender::MaxTessellation = 6.0f;
const float TerrainRender::MinTessellation = 0.0f;

TerrainRender::TerrainRender(Terrain * terrain)
	: terrain(terrain)//, bUseTessellation(true)
	, quadPatchVB(NULL), quadPatchIB(NULL)
	, bvhVertexBuffer(NULL), bvhIndexBuffer(NULL)
	, bWireFrame(false)
	, layerMapArraySRV(NULL)
	, heightMapTex(NULL), heightMapSRV(NULL), heightMapUAV(NULL)
	, blendMapTex(NULL), blendMapUAV(NULL), blendMapSRV(NULL)
{

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

	SAFE_RELEASE(blendMapTex);
	SAFE_RELEASE(blendMapSRV);
	SAFE_RELEASE(blendMapUAV);

	SAFE_RELEASE(heightMapTex);
	SAFE_RELEASE(heightMapSRV);
	SAFE_RELEASE(heightMapUAV);

	SAFE_DELETE(frustum);

	SAFE_DELETE(fastPickingShader);
	SAFE_DELETE(rtv);
	SAFE_DELETE(dsv);
	SAFE_DELETE(render2D);

	SAFE_DELETE(adjustHeightMapComputeShader);
	SAFE_DELETE(blendMapComputeShader);

	SAFE_DELETE(billboard);

	while (!undoStack.empty()) {
		Command * command = undoStack.top();
		SAFE_DELETE(command);
		undoStack.pop();
	}

	while (!redoStack.empty()) {
		Command * command = redoStack.top();
		SAFE_DELETE(command);
		redoStack.pop();
	}
}

void TerrainRender::Initialize()
{
	D3DXMatrixIdentity(&world);


	patchVertexRows = ((UINT)(terrain->Desc().HeightMapHeight - 1) / Terrain::CellsPerPatch) + 1;
	patchVertexCols = ((UINT)(terrain->Desc().HeightMapWidth - 1) / Terrain::CellsPerPatch) + 1;

	patchVerticesCount = patchVertexRows * patchVertexCols;
	patchQuadFacesCount = (patchVertexRows - 1) * (patchVertexCols - 1);

	CalcAllPatchBoundsY();

	frustum = new Frustum(500);

	fastPickingShader = new Shader(Shaders + L"Homework/FastPicking.fx");
	adjustHeightMapComputeShader = new Shader(Shaders + L"Homework/AdjustHeightMap.fx", true);
	blendMapComputeShader = new Shader(Shaders + L"Homework/CreateBlendMap.fx", true);
}

void TerrainRender::Ready(Material* material)
{
	billboard = new Billboard();

	this->material = material;

	BuildQuadPatchVB();
	BuildQuadPatchIB();

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

	rtv = new RenderTargetView(
		Context::Get()->GetViewport()->GetWidth(),
		Context::Get()->GetViewport()->GetHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT);

	dsv = new DepthStencilView(Context::Get()->GetViewport()->GetWidth(),
		Context::Get()->GetViewport()->GetHeight());
	render2D = new Render2D();
	render2D->Position(0, 75);
	render2D->Scale(300, 300);

	selectSRV = 0;
	render2D->SRV(rtv->SRV());
	selectEdit = 0;

	vector<wstring> textures;
	for (int i = 0; i < 5; i++)
		textures.push_back(terrain->Desc().layerMapFile[i]);
	layerMapArray = new TextureArray(textures, 512, 512);
	layerMapArraySRV = layerMapArray->GetSRV();

	terrain->GetHeightMap()->Build(&heightMapTex, &heightMapSRV, &heightMapUAV);

	CreateBlendMap();

	Shader* shader = material->GetShader();

	heightMapVariable = shader->AsSRV("HeightMap");
	layerMapArrayVariable = shader->AsSRV("LayerMapArray");
	blendMapVariable = shader->AsSRV("BlendMap");

	heightMapVariable->SetResource(heightMapSRV);
	layerMapArrayVariable->SetResource(layerMapArraySRV);
	blendMapVariable->SetResource(blendMapSRV);

	// buffer, brush
	{
		buffer.FogStart = 300.0f;
		buffer.FogRange = 200.0f;
		buffer.FogColor = D3DXCOLOR(192 / 255.0f, 192 / 255.0f, 192 / 255.0f, 1);
		buffer.MinDistance = MinDistance;
		buffer.MaxDistance = MaxDistance;
		buffer.MinTessellation = MinTessellation;
		buffer.MaxTessellation = MaxTessellation;
		buffer.TexelCellSpaceU = 1.0f / terrain->Desc().HeightMapWidth;
		buffer.TexelCellSpaceV = 1.0f / terrain->Desc().HeightMapHeight;
		buffer.WorldCellSpace = terrain->Desc().CellSpacing;
		buffer.TexScale = D3DXVECTOR2(5.0f, 5.0f);

		brush.Type = 0;
		brush.Location = D3DXVECTOR3(0, 0, 0);
		brush.Range = 2;
		brush.Color = D3DXVECTOR3(0, 1, 0);
		brush.Capacity = 0.1f;
	}

	UpdateHeightMap();

	fastPickingShader->AsScalar("Width")->SetFloat(1024);
	fastPickingShader->AsScalar("Height")->SetFloat(1024);
	fastPickingShader->AsScalar("MaxHeight")->SetFloat(terrain->GetHeightMap()->MaxHeight());
	fastPickingShader->AsSRV("BillboardTextures")->SetResource(billboard->GetTextureArray()->GetSRV());

	billboard->GetShader()->AsScalar("MaxHeight")->SetFloat(terrain->GetHeightMap()->MaxHeight());

	constantBufferVariable = shader->AsConstantBuffer("CB_Terrain");
	worldVariable = shader->AsMatrix("World");
}

void TerrainRender::Update()
{
	// Brush 
	{
		if (Keyboard::Get()->Press(VK_LCONTROL))
		{
			//Mouse::Get()->GetMoveValue 이거의 z 값 +- 값이 mouse wheel 값임

			if (!ImGui::IsAnyWindowHovered() && !ImGui::IsAnyItemActive()) {
				if (ImGui::GetIO().MouseWheel > 0) {
					if (selectEdit == 0)
						brush.Capacity += 0.01f;
					else if (selectEdit == 1)
						billboard->UpdateFactor(1);
				}
				else if (ImGui::GetIO().MouseWheel < 0) {
					if (selectEdit == 0)
						brush.Capacity -= 0.01f;
					else if (selectEdit == 1)
						billboard->UpdateFactor(-1);
				}
			}

			if (Keyboard::Get()->Down('Z')) Undo();
			if (Keyboard::Get()->Down('Y')) Redo();

		}
		else
		{
			if (!ImGui::IsAnyWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::GetIO().MouseWheel > 0)
				brush.Range++;
			else if (!ImGui::IsAnyWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::GetIO().MouseWheel < 0)
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
	if (Keyboard::Get()->Down('Z')) selectEdit = (selectEdit++) % 2;

	// Pick
	if (Pick(&pickPos) && !ImGui::IsAnyWindowHovered() && !ImGui::IsAnyItemActive()) {
		brush.Location = pickPos;

		if (selectEdit == 0 && brush.Type != 0) {
			if (Mouse::Get()->Down(0)) {
				undoStack.push(new BrushCommand(this));
			}

			if (Mouse::Get()->Press(0))
				AdjustY();

			if (Mouse::Get()->Up(0)) {
				undoStack.top()->Execute();
			}
		}
		else if (selectEdit == 1 && Mouse::Get()->Down(0)) {
			undoStack.push(new BillboardCommand(billboard));
			AddBillboard();
			undoStack.top()->Execute();
		}
	}
}

void TerrainRender::PreRender()
{
	// FastPicking 
	{
		Context::Get()->GetViewport()->RSSetViewport();

		D3D::Get()->SetRenderTarget(rtv->RTV(), dsv->DSV());
		D3D::Get()->Clear(D3DXCOLOR(0, 0, 0, 0), rtv->RTV(), dsv->DSV());

		frustum->GetPlanes(buffer.WorldFrustumPlanes);

		D3D11_MAPPED_SUBRESOURCE subResource;
		HRESULT hr = D3D::GetDC()->Map
		(
			cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource
		);
		assert(SUCCEEDED(hr));

		memcpy(subResource.pData, &buffer, sizeof(Buffer));
		D3D::GetDC()->Unmap(cBuffer, 0);

		hr = fastPickingShader->AsConstantBuffer("CB_Terrain")->SetConstantBuffer(cBuffer);
		assert(SUCCEEDED(hr));

		UINT stride = sizeof(TerrainCP);
		UINT offset = 0;

		D3D::GetDC()->IASetVertexBuffers(0, 1, &quadPatchVB, &stride, &offset);
		D3D::GetDC()->IASetIndexBuffer(quadPatchIB, DXGI_FORMAT_R16_UINT, 0);
		D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

		fastPickingShader->AsMatrix("World")->SetMatrix(world);
		fastPickingShader->DrawIndexed(0, 0, patchQuadFacesCount * 4);

		billboard->PreRender(fastPickingShader);
	}
}

void TerrainRender::Render()
{
	ImGuiRender();

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

	billboard->Render();

	render2D->Render();
}

bool TerrainRender::Pick(OUT D3DXVECTOR3 * val)
{
	float x, y, z;
	D3DXVECTOR3 position = Mouse::Get()->GetPosition();
	D3DXCOLOR result = Textures::ReadPixel(rtv->Texture(), position.x, position.y);

	x = result.r * 1024 - 512;
	y = result.b;
	z = result.g * 1024 - 512;

	(*val) = D3DXVECTOR3(x, y, z);

	return x + z + 1024 > 0;
}

void TerrainRender::LoadHeightMap(wstring fileName)
{
	terrain->GetHeightMap()->Load(fileName);

	SAFE_RELEASE(heightMapTex);
	SAFE_RELEASE(heightMapSRV);
	SAFE_RELEASE(heightMapUAV);

	terrain->GetHeightMap()->Build(&heightMapTex, &heightMapSRV, &heightMapUAV);

	CreateBlendMap();

	material->GetShader()->AsSRV("BlendMap")->SetResource(blendMapSRV);

	UpdateHeightMap();
}

void TerrainRender::SaveHeightMap(wstring fileName)
{
	terrain->GetHeightMap()->Save(fileName, heightMapSRV);
}

void TerrainRender::LoadBillboard(wstring fileName)
{
	billboard->Load(fileName);
}

void TerrainRender::SaveBillboard(wstring fileName)
{
	billboard->Save(fileName);
}

void TerrainRender::ImGuiRender()
{
	// wheel test
	//ImGui::Text("Wheel %.1f", ImGui::GetIO().MouseWheel);

	ImGui::Text("Pos %f %f", pickPos.x, pickPos.z);
	string check = "Pick ";
	check += pickPos.y == 1.0f ? "True" : "False";
	ImGui::Text(check.c_str());

	ImGui::Checkbox("WireFrame", &bWireFrame);

	// Map
	{
		ImGui::Begin("Map");

		// Select SRV
		{
			ImGui::Columns(2);

			if (ImGui::RadioButton("FastPicking Map", &selectSRV, 0)) render2D->SRV(rtv->SRV());
			if (ImGui::RadioButton("Height Map", &selectSRV, 1)) render2D->SRV(heightMapSRV);
			if (ImGui::RadioButton("Blend Map", &selectSRV, 2)) render2D->SRV(blendMapSRV);

			ImGui::NextColumn();

			ImGui::Text("Select Edit");
			ImGui::RadioButton("Brush", &selectEdit, 0);
			ImGui::RadioButton("Billboard", &selectEdit, 1);

			ImGui::Text("Instance Count %d", billboard->GetCount());

			ImGui::Columns(1);
		}

		ImGui::End();
	}

	// Brush
	{
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
	}

	// Billboard
	{
		ImGui::Begin("Billboard");

		ImGui::Columns(2);
		//ImGui::SetColumnWidth(0, 115);

		billboard->ImGuiRender(false, false);

		ImGui::NextColumn();

		ImGui::RadioButton("One", &brush.Type, 0);

		ImGui::SameLine(70);
		if (ImGui::SmallButton("Save"))
		{
			func = bind(&TerrainRender::SaveBillboard, this, placeholders::_1);
			Path::SaveFileDialog(L"", L"Billboard Files(*.billboard)\0*.billboard\0", L"./", func);
		}

		ImGui::SameLine(115);
		if (ImGui::SmallButton("Load"))
		{
			func = bind(&TerrainRender::LoadBillboard, this, placeholders::_1);
			Path::OpenFileDialog(L"", L"Billboard Files(*.billboard)\0*.billboard\0", L"./", func);
		}

		ImGui::RadioButton("Rect", &brush.Type, 1);
		ImGui::SameLine(80);
		ImGui::RadioButton("Circle", &brush.Type, 2);

		ImGui::DragInt("Range", &brush.Range, 1, 1, 50);
		ImGui::ColorEdit3("Color", (float*)&brush.Color);
		ImGui::InputInt("Factor", billboard->Factor());

		ImGui::Columns(1);

		ImGui::End();
	}

	// Command
	{
		//string name = "Command (Stack " + to_string(undoStack.size()) + "/" + to_string(redoStack.size()) + ")";

		ImGui::Begin("Command");
		//ImGui::Begin(name.c_str());
		//ImGui::SetWindowPos(ImVec2(0, 300));
		//ImGui::SetWindowSize(ImVec2(300, 345));
		// 0, 300
		//ImGui::Text("%f %f", ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
		// 300 345
		//ImGui::Text("%f %f", ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

		ImGui::BulletText("Stack (%d / %d)", undoStack.size(), redoStack.size());

		ImGui::Columns(2);

		{
			ImGui::Text("Undo Stack");

			stack<Command*> temp = undoStack;
			stack<Command*> temp2;

			while (!temp.empty()) {
				temp2.push(temp.top());
				temp.pop();
			}

			while (!temp2.empty()) {
				temp2.top()->Render();
				temp2.pop();
			}
		}

		ImGui::NextColumn();

		{
			ImGui::Text("Redo Stack");

			stack<Command*> temp = redoStack;
			stack<Command*> temp2;

			while (!temp.empty()) {
				temp2.push(temp.top());
				temp.pop();
			}

			while (!temp2.empty()) {
				temp2.top()->Render();
				temp2.pop();
			}
		}

		ImGui::Columns(1);

		ImGui::End();
	}
}

void TerrainRender::SetHeightMap(ID3D11Texture2D * tex)
{
	SAFE_RELEASE(heightMapTex);
	SAFE_RELEASE(heightMapSRV);
	SAFE_RELEASE(heightMapUAV);

	D3D11_TEXTURE2D_DESC desc;
	tex->GetDesc(&desc);
	D3D::GetDevice()->CreateTexture2D(&desc, NULL, &heightMapTex);

	D3D::GetDC()->CopyResource(heightMapTex, tex);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc; // 안에 포인터 있으면 0 초기화 불가능, 생성자가 없어도 안됨
											 // union 그 구조체 안에 가장 큰 사이즈로 맞추고 나머지는 그 공간 공유해서 쓰는거
											 // union에서도 padding 잡아줄 수 있음
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	HRESULT hr = D3D::GetDevice()->CreateShaderResourceView(heightMapTex, &srvDesc, &heightMapSRV);
	assert(SUCCEEDED(hr));

	//SAFE_DELETE_ARRAY(temp);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	uavDesc.Format = desc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

	hr = D3D::GetDevice()->CreateUnorderedAccessView(heightMapTex, &uavDesc, &heightMapUAV);
	assert(SUCCEEDED(hr));

	UpdateHeightMap();
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
	DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	//   Create Blend Texture2D
	if (blendMapTex == NULL)
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
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;;

		HRESULT hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &blendMapTex);
		assert(SUCCEEDED(hr));
	}

	// Create BlendMap SRV
	if (blendMapSRV == NULL)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc; // 안에 포인터 있으면 0 초기화 불가능, 생성자가 없어도 안됨
												 // union 그 구조체 안에 가장 큰 사이즈로 맞추고 나머지는 그 공간 공유해서 쓰는거
												 // union에서도 padding 잡아줄 수 있음
		ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvDesc.Format = format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;

		HRESULT hr = D3D::GetDevice()->CreateShaderResourceView(blendMapTex, &srvDesc, &blendMapSRV);
		assert(SUCCEEDED(hr));
	}

	// Create BlendMap UAV
	if (blendMapUAV == NULL)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		uavDesc.Format = format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

		HRESULT hr = D3D::GetDevice()->CreateUnorderedAccessView(blendMapTex, &uavDesc, &blendMapUAV);
		assert(SUCCEEDED(hr));
	}

	blendMapComputeShader->AsScalar("MaxHeight")->SetFloat(terrain->GetHeightMap()->MaxHeight());
	blendMapComputeShader->AsSRV("Input")->SetResource(heightMapSRV);
	blendMapComputeShader->AsUAV("Output")->SetUnorderedAccessView(blendMapUAV);

	blendMapComputeShader->Dispatch(0, 0, 256, 256, 1);

	SmoothBlendMap();
	SmoothBlendMap();
}

void TerrainRender::SmoothBlendMap()
{
	blendMapComputeShader->Dispatch(0, 1, 256, 256, 1);
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



	UINT width = 256;
	UINT height = 256;

	adjustHeightMapComputeShader->Dispatch(0, 0, width, height, 1);
}

void TerrainRender::AddBillboard()
{
	// single billboard Add
	if (brush.Type == 0) {
		int exp = 1;
		pair<int, int> temp = make_pair((int)(pickPos.x * exp), (int)(pickPos.z * exp));

		if (checkSet.find(temp) == checkSet.end()) {
			checkSet.insert(temp);
			billboard->AddInstance(D3DXVECTOR3(pickPos.x, 0, pickPos.z));
		}
	}

	// multi billboard Add
	else
	{
		vector<D3DXVECTOR3> vec(*billboard->Factor());

		for (int i = 0; i < vec.size(); i++) {
			if (brush.Type == 1 || brush.Type == 4)
			{
				vec[i].x = pickPos.x + Math::Random(-(float)brush.Range, (float)brush.Range);
				vec[i].y = 0;
				vec[i].z = pickPos.z + Math::Random(-(float)brush.Range, (float)brush.Range);
			}
			else if (brush.Type == 2 || brush.Type == 3)
			{
				float angle = Math::Random(0.0f, D3DX_PI * 2.0f);

				vec[i].x = pickPos.x + Math::Random(-(float)brush.Range, (float)brush.Range) * cosf(angle);
				vec[i].y = 0;
				vec[i].z = pickPos.z + Math::Random(-(float)brush.Range, (float)brush.Range) * sinf(angle);
			}
		}

		billboard->AddInstance(vec);
	}
}

void TerrainRender::UpdateHeightMap()
{
	material->GetShader()->AsSRV("HeightMap")->SetResource(heightMapSRV);
	fastPickingShader->AsSRV("HeightMap")->SetResource(heightMapSRV);
	billboard->GetShader()->AsSRV("HeightMap")->SetResource(heightMapSRV);

	adjustHeightMapComputeShader->AsUAV("Output")->SetUnorderedAccessView(heightMapUAV);

	if (selectSRV == 1) render2D->SRV(heightMapSRV);
}

void TerrainRender::Undo()
{
	if (!undoStack.empty()) {
		Command* command = undoStack.top();
		command->Undo();
		redoStack.push(command);
		undoStack.pop();
	}
}

void TerrainRender::Redo()
{
	if (!redoStack.empty()) {
		Command* command = redoStack.top();
		command->Execute();
		undoStack.push(command);
		redoStack.pop();
	}
}
