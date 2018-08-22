#include "stdafx.h"
#include "Terrain.h"

Terrain::Terrain(ExecuteValues* values, Material * material)
	: material(material)
	, values(values)
	, selectBrush(0), capacity(10.0f)
	, fillMode(0), colorMode(0)
{
	heightTexture = new Texture(Contents + L"HeightMaps/Homework/HeightMap256.png");
	colorTexture = new Texture(Textures + L"Dirt.png");
	colorTexture2 = new Texture(Textures + L"Wall.png");

	alphaTexture = new Texture(Contents + L"HeightMaps/Homework/ColorMap256.png");

	worldBuffer = new WorldBuffer();

	brushBuffer = new BrushBuffer();

	CreateData();
	CreateNormalData();
	CreateBuffer();

	// Create Rasterizer
	{
		D3D11_RASTERIZER_DESC desc;
		States::GetRasterizerDesc(&desc);

		States::CreateRasterizer(&desc, &rasterizer[0]);

		desc.FillMode = D3D11_FILL_WIREFRAME;
		States::CreateRasterizer(&desc, &rasterizer[1]);
	}

	// Create Sampler
	{
		D3D11_SAMPLER_DESC desc;
		States::GetSamplerDesc(&desc);
		States::CreateSampler(&desc, &sampler);
	}
}

Terrain::~Terrain()
{
	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	SAFE_DELETE(material);

	SAFE_DELETE(heightTexture);
	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(brushBuffer);

	SAFE_DELETE(colorTexture);
	SAFE_DELETE(colorTexture2);
	SAFE_DELETE(alphaTexture)

	SAFE_RELEASE(rasterizer[0]);
	SAFE_RELEASE(rasterizer[1]);
}

void Terrain::Update()
{
	D3DXVECTOR3 position;
	bool bPicked = Y(&position);
	
	if (bPicked == true)
	{
		brushBuffer->Data.Location = position;

		UINT type = brushBuffer->Data.Type;
		// 마우스 클릭 시 높이 조정
		if (Mouse::Get()->Press(0)) 
		{
			if (type != 0) {
				if(selectBrush == 0)
					AdjustY(position);

				if (type != 3 && selectBrush == 1) {
					AdjustColorMap(position, colorMode);
				}
			}
		}
	}
}

void Terrain::Render()
{
	brushBuffer->SetVSBuffer(10);

	colorTexture->SetShaderResource(10);
	colorTexture->SetShaderSampler(10);

	colorTexture2->SetShaderResource(11);
	colorTexture2->SetShaderSampler(11);

	alphaTexture->SetShaderResource(12);
	alphaTexture->SetShaderSampler(12);

	UINT stride = sizeof(VertexColorTextureNormal);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	worldBuffer->SetVSBuffer(1);
	material->PSSetBuffer();

	D3D::GetDC()->RSSetState(rasterizer[fillMode]);
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
	D3D::GetDC()->RSSetState(rasterizer[0]);
}

void Terrain::PostRender()
{
	ImGui::Begin("Brush Info");
	ImGui::SliderInt("FillMode", &fillMode, 0, 1);
	
	ImGui::SliderInt("SelectBrush", &selectBrush, 0, 1);
	ImGui::SliderInt("ColorMode", &colorMode, 0, 1);

	ImGui::Separator();
	ImGui::Text("Brush");
	ImGui::SliderInt("Type", &brushBuffer->Data.Type, 0, 4);
	ImGui::SliderInt("Range", &brushBuffer->Data.Range, 1, 10);
	ImGui::DragFloat("Capacity", &capacity, 0.5f);
	ImGui::SliderFloat3("Color", (float*)&brushBuffer->Data.Color, 0, 1);
	
	ImGui::Separator();

	if (ImGui::Button("Save ColorMap")) {
		func = bind(&Terrain::SaveColorMap, this, placeholders::_1);
		Path::SaveFileDialog(L"", L"*.png", 
			Contents + L"HeightMaps/Homework", func);
	}
	// 다음꺼 같은 줄에 그리는거 인자는 시작 위치
	ImGui::SameLine(120);
	if (ImGui::Button("Load ColorMap")) {
		func = bind(&Terrain::LoadColorMap, this, placeholders::_1);
		Path::OpenFileDialog(L"", L"PNG Files(*.png)\0*.png\0",
			Contents + L"HeightMaps/Homework", func);
	}

	ImGui::End();
}

float Terrain::Y(D3DXVECTOR3& position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x < 0 || x >= width) return 0.0f;
	if (z < 0 || z >= height) return 0.0f;

	UINT index[4];

	index[0] = (width + 1) * z + x;
	index[1] = (width + 1) * (z + 1) + x;
	index[2] = (width + 1) * z + (x + 1);
	index[3] = (width + 1) * (z + 1) + (x + 1);

	D3DXVECTOR3 v[4];
	for (int i = 0; i < 4; i++)
		v[i] = vertices[index[i]].Position;

	// 길이에 대한 비율
	float ddx = (position.x - v[0].x) / 1.0f;
	float ddz = (position.z - v[0].z) / 1.0f;

	D3DXVECTOR3 temp;

	// 밑면
	if (ddx + ddz <= 1)
	{
		temp = v[0] + (v[2] - v[0]) * ddx + (v[1] - v[0]) * ddz;
	}
	// 윗면
	{
		ddx = 1 - ddx;
		ddz = 1 - ddz;

		temp = v[3] + (v[1] - v[3]) * ddx + (v[2] - v[3]) * ddz;
	}

	return temp.y;
}

bool Terrain::Y(OUT D3DXVECTOR3 * out, D3DXVECTOR3 & position)
{
	D3DXVECTOR3 start = D3DXVECTOR3(position.x, 1000, position.z);
	D3DXVECTOR3 direction = D3DXVECTOR3(0, -1, 0);

	// 삼각형 전체 충돌하는 방법 하나씩 충돌하는 방법 두가지가있음
	// 정밀하게 하려면 전체 삼각형에 대해 돌아야함

	// 원래 for문 전체 돌아야함 = 셀 전부다 계산해야함
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x < 0 || x >= width) return false;
	if (z < 0 || z >= height) return false;

	UINT index[4];
	index[0] = (width + 1) * z + x;
	index[1] = (width + 1) * (z + 1) + x;
	index[2] = (width + 1) * z + (x + 1);
	index[3] = (width + 1) * (z + 1) + (x + 1);

	D3DXVECTOR3 p[4];
	for (int i = 0; i < 4; i++)
		p[i] = vertices[index[i]].Position;

	float u, v, distance;
	if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start,
		&direction, &u, &v, &distance))
	{
		*out = p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;

		return true;
	}

	if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start,
		&direction, &u, &v, &distance))
	{
		*out = p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;

		return true;
	}

	return false;
}

bool Terrain::Y(OUT D3DXVECTOR3 * out)
{
	// start 카메라 위치 direction 마우스 위치로부터 계산된 방향
	//D3DXVECTOR3 start = D3DXVECTOR3(position.x, 1000, position.z);
	//D3DXVECTOR3 direction = D3DXVECTOR3(0, -1, 0);

	D3DXVECTOR3 start;
	values->MainCamera->Position(&start);

	D3DXVECTOR3 direction = values->MainCamera->Direction(
		values->Viewport, values->Perspective
	);

	// 삼각형 전체 충돌하는 방법 하나씩 충돌하는 방법 두가지가있음
	// 정밀하게 하려면 전체 삼각형에 대해 돌아야함

	// 원래 for문 전체 돌아야함 = 셀 전부다 계산해야함

	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			UINT index[4];
			index[0] = (width + 1) * z + x;
			index[1] = (width + 1) * (z + 1) + x;
			index[2] = (width + 1) * z + (x + 1);
			index[3] = (width + 1) * (z + 1) + (x + 1);

			D3DXVECTOR3 p[4];
			for (int i = 0; i < 4; i++)
				p[i] = vertices[index[i]].Position;

			float u, v, distance;
			if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start,
				&direction, &u, &v, &distance))
			{
				*out = p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;

				return true;
			}

			if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start,
				&direction, &u, &v, &distance))
			{
				*out = p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;

				return true;
			}
		} // for(x)
	} // for(z)

	return false;
}

void Terrain::AdjustY(D3DXVECTOR3 & location)
{
	// 범위
	float size = (float)brushBuffer->Data.Range;

	// directx 12에 있는 구조체
	D3D11_BOX box;
	box.left = (UINT)location.x - size;
	box.bottom = (UINT)location.z - size;
	box.top = (UINT)location.z + size;
	box.right = (UINT)location.x + size;

	// 범위 체크
	if (box.left < 0) box.left = 0;
	if (box.top >= height) box.top = height;
	if (box.right >= width) box.right = width;
	if (box.bottom < 0)box.bottom = 0;

	for (UINT z = box.bottom; z <= box.top; z++) {
		for (UINT x = box.left; x <= box.right; x++) {
			UINT index = (width + 1) * z + x;
			
			// circle 모양
			if (brushBuffer->Data.Type == 2 ||
				brushBuffer->Data.Type == 3) {
				float dx = vertices[index].Position.x - location.x;
				float dz = vertices[index].Position.z - location.z;

				float dist = sqrt(dx * dx + dz * dz);
				if (dist > brushBuffer->Data.Range) continue;
			}
			
			float temp = capacity * Time::Delta();

			// sinf
			if (brushBuffer->Data.Type == 3)
			{
				float dx = vertices[index].Position.x - location.x;
				float dz = vertices[index].Position.z - location.z;

				float dist = sqrt(dx * dx + dz * dz);
				dist = (size - dist) / (float)size * (D3DX_PI / 2.0f);
				temp *= sinf(dist);
			}

			// flat
			if (brushBuffer->Data.Type == 4)
			{
				UINT cnt = 0;
				float avg = 0;

				// 좌우로 한칸씩 빼서 평균 구하기
				// uint -> int -까지 가려고
				for (int zp = ((int)z - 1); zp <= ((int)z + 1); zp++) {
					for (int xp = ((int)x - 1); xp <= ((int)x + 1); xp++) {
						if (xp < 0 || xp > width || zp < 0 || zp > height) 
							continue;

						cnt++;
						UINT indexp = (width + 1) * zp + xp;
						avg += vertices[indexp].Position.y;
					}
				}

				temp = (cnt == 0 ? 
					vertices[index].Position.y : avg / (float)cnt);
				vertices[index].Position.y = 0;
			}

			vertices[index].Position.y += temp;

		}
	}

	// 이전 코드
	//if (brushBuffer->Data.Type == 1) 
	//{
	//	for (UINT z = box.bottom; z <= box.top; z++) {
	//		for (UINT x = box.left; x <= box.right; x++) {
	//			UINT index = (width + 1) * z + x;
	//
	//			vertices[index].Position.y += capacity * Time::Delta();
	//		}
	//	}
	//}
	//
	//if (brushBuffer->Data.Type == 2)
	//{
	//	float centerX = (UINT)location.x;
	//	float centerZ = (UINT)location.z;
	//
	//	for (UINT z = box.bottom; z <= box.top; z++) {
	//		for (UINT x = box.left; x <= box.right; x++) {
	//			float dx = x - centerX;
	//			float dz = z - centerZ;
	//		
	//			float dist = sqrt(dx * dx + dz * dz);
	//			if (dist <= brushBuffer->Data.Range) 
	//			{
	//				UINT index = (width + 1) * z + x;
	//
	//				vertices[index].Position.y += capacity * Time::Delta();
	//			}
	//		}
	//	}
	//}
	//
	//if (brushBuffer->Data.Type == 3)
	//{
	//	float centerX = (UINT)location.x;
	//	float centerZ = (UINT)location.z;
	//
	//
	//	for (UINT z = box.bottom; z <= box.top; z++) {
	//		for (UINT x = box.left; x <= box.right; x++) {
	//			float dx = x - centerX;
	//			float dz = z - centerZ;
	//
	//			float dist = sqrt(dx * dx + dz * dz);
	//			if (dist <= brushBuffer->Data.Range)
	//			{
	//				UINT index = (width + 1) * z + x;
	//
	//				vertices[index].Position.y += capacity * Time::Delta() / 2
	//					* (sinf((z - box.bottom) / (2 * size) * D3DX_PI) + 
	//						sinf((x - box.left) / (2 * size) * D3DX_PI));
	//			}
	//		}
	//	}
	//}

	// normal도 재계산해줘야함
	CreateNormalData();

	// NULL 이부분 계산해서 넣으면 수정된 구간만 변경할 수 있음
	D3D::GetDC()->UpdateSubresource(
		vertexBuffer, 0, NULL, &vertices[0], 
		sizeof(VertexColorTextureNormal) * vertexCount, 0
	);
}

void Terrain::AdjustColorMap(D3DXVECTOR3 & location, bool isColorDown)
{
	// 범위
	float size = (float)brushBuffer->Data.Range;

	// directx 12에 있는 구조체
	D3D11_BOX box;
	box.left = (UINT)location.x - size;
	box.bottom = (UINT)location.z - size;
	box.top = (UINT)location.z + size;
	box.right = (UINT)location.x + size;

	// 범위 체크
	if (box.left < 0) box.left = 0;
	if (box.top >= height) box.top = height;
	if (box.right >= width) box.right = width;
	if (box.bottom < 0) box.bottom = 0;

	bool isColorMapChange = false;

	for (UINT z = box.bottom; z <= box.top; z++) {
		for (UINT x = box.left; x <= box.right; x++) {
			UINT index = (width + 1) * z + x;

			if (brushBuffer->Data.Type == 2) {
				float dx = vertices[index].Position.x - location.x;
				float dz = vertices[index].Position.z - location.z;

				float dist = sqrt(dx * dx + dz * dz);
				if (dist < brushBuffer->Data.Range) continue;
			}

			vertices[index].Color.r += capacity * Time::Delta();
			if (vertices[index].Color.r < 0)
				vertices[index].Color.r = 0;
			if (vertices[index].Color.r > 1.0f)
				vertices[index].Color.r = 1.0f;
		}
	}

	// NULL 이부분 계산해서 넣으면 수정된 구간만 변경할 수 있음
	D3D::GetDC()->UpdateSubresource(
		vertexBuffer, 0, NULL, &vertices[0],
		sizeof(VertexColorTextureNormal) * vertexCount, 0
	);

	//if (brushBuffer->Data.Type == 1)
	//{
	//	for (UINT z = box.bottom; z <= box.top; z++) {
	//		for (UINT x = box.left; x <= box.right; x++) {
	//			UINT index = (width + 1) * z + x;
	//
	//			if (isColorDown && vertices[index].Color.r > 0.0f)
	//			{
	//				vertices[index].Color.r -= capacity * Time::Delta();
	//				if (vertices[index].Color.r < 0)
	//					vertices[index].Color.r = 0;
	//				isColorMapChange = true;
	//			}
	//			else if (!isColorDown && vertices[index].Color.r < 1.0f)
	//			{
	//				vertices[index].Color.r += capacity * Time::Delta();
	//				if (vertices[index].Color.r > 1.0f)
	//					vertices[index].Color.r = 1.0f;
	//				isColorMapChange = true;
	//			}
	//		}
	//	}
	//}
	//
	//if (brushBuffer->Data.Type == 2)
	//{
	//	float centerX = (UINT)location.x;
	//	float centerZ = (UINT)location.z;
	//
	//	for (UINT z = box.bottom; z <= box.top; z++) {
	//		for (UINT x = box.left; x <= box.right; x++) {
	//			float dx = x - centerX;
	//			float dz = z - centerZ;
	//
	//			float dist = sqrt(dx * dx + dz * dz);
	//			if (dist <= brushBuffer->Data.Range)
	//			{
	//				UINT index = (width + 1) * z + x;
	//
	//				if (isColorDown && vertices[index].Color.r > 0.0f)
	//				{
	//					vertices[index].Color.r -= capacity * Time::Delta();
	//					if (vertices[index].Color.r < 0)
	//						vertices[index].Color.r = 0;
	//					isColorMapChange = true;
	//				}
	//				else if (!isColorDown && vertices[index].Color.r < 1.0f)
	//				{
	//					vertices[index].Color.r += capacity * Time::Delta();
	//					if (vertices[index].Color.r > 1.0f)
	//						vertices[index].Color.r = 1.0f;
	//					isColorMapChange = true;
	//				}
	//			}
	//		}
	//	}
	//}
	//
	//if (isColorMapChange)
	//{
	//	// NULL 이부분 계산해서 넣으면 수정된 구간만 변경할 수 있음
	//	D3D::GetDC()->UpdateSubresource(
	//		vertexBuffer, 0, NULL, &vertices[0],
	//		sizeof(VertexColorTextureNormal) * vertexCount, 0
	//	);
	//}
}

void Terrain::SaveColorMap(wstring fileName)
{
	vector<D3DXCOLOR> colors;
	for (int i = 0; i < vertexCount; i++)
		colors.push_back(vertices[i].Color);

	alphaTexture->WritePixels(DXGI_FORMAT_R8G8B8A8_UNORM, colors);

	alphaTexture->SaveFile(fileName);
}

void Terrain::LoadColorMap(wstring fileName)
{
	if (alphaTexture != NULL)
		SAFE_DELETE(alphaTexture);

	alphaTexture = new Texture(fileName);
	vector<D3DXCOLOR> color;
	alphaTexture->ReadPixels(DXGI_FORMAT_R8G8B8A8_UNORM, &color);

	for (UINT z = 0; z <= height; z++)
	{
		for (UINT x = 0; x <= width; x++)
		{
			UINT count = (width + 1) * z + x;
			vertices[count].Color = color[count];
		}
	}

	D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, vertices, 
		sizeof(VertexColorTextureNormal) * vertexCount, 0);
}

void Terrain::CreateData()
{
	vector<D3DXCOLOR> heights;
	heightTexture->ReadPixels(DXGI_FORMAT_R8G8B8A8_UNORM, &heights);

	vector<D3DXCOLOR> colors;
	alphaTexture->ReadPixels(DXGI_FORMAT_R8G8B8A8_UNORM, &colors);

	width = heightTexture->GetWidth() - 1;
	height = heightTexture->GetHeight() - 1;

	//Create VertexData
	{
		vertexCount = (width + 1) * (height + 1);
		vertices = new VertexColorTextureNormal[vertexCount];

		for (UINT z = 0; z <= height; z++)
		{
			for (UINT x = 0; x <= width; x++)
			{
				UINT index = (width + 1) * z + x;

				vertices[index].Position.x = (float)x;
				vertices[index].Position.y = (float)(heights[index].r * 255.0f) / 10.0f;
				//vertices[index].Position.y = 0;
				vertices[index].Position.z = (float)z;

				vertices[index].Color = colors[index];

				vertices[index].Uv.x = (float)x / width;
				vertices[index].Uv.y = (float)z / height;
			}
		}//for(z)
	}


	//CreateIndexData
	{
		// 기본 자료형 usinged int 도 생성자로 써도 됨 UINT() 이걸로
		//indices.assign(width * height * 6, UINT());

		indexCount = width * height * 6;
		indices = new UINT[indexCount];

		UINT index = 0;
		for (UINT z = 0; z < height; z++)
		{
			for (UINT x = 0; x < width; x++)
			{
				indices[index + 0] = (width + 1) * z + x; //0
				indices[index + 1] = (width + 1) * (z + 1) + x; //1
				indices[index + 2] = (width + 1) * z + x + 1; //2

				indices[index + 3] = (width + 1) * z + x + 1; //2
				indices[index + 4] = (width + 1) * (z + 1) + x; //1
				indices[index + 5] = (width + 1) * (z + 1) + x + 1; //1

				index += 6;
			}
		}//for(z)
	}
}

void Terrain::CreateNormalData()
{
	// CreateNormal
	{
		for (UINT i = 0; i < (indexCount / 3); i++)
		{
			UINT index0 = indices[i * 3 + 0];
			UINT index1 = indices[i * 3 + 1];
			UINT index2 = indices[i * 3 + 2];

			VertexColorTextureNormal v1 = vertices[index1];
			VertexColorTextureNormal v2 = vertices[index2];
			VertexColorTextureNormal v0 = vertices[index0];

			D3DXVECTOR3 d1 = v1.Position - v0.Position;
			D3DXVECTOR3 d2 = v2.Position - v0.Position;

			D3DXVECTOR3 normal;
			D3DXVec3Cross(&normal, &d1, &d2);

			vertices[index0].Normal += normal;
			vertices[index1].Normal += normal;
			vertices[index2].Normal += normal;
		}

		for (UINT i = 0; i < vertexCount; i++)
			D3DXVec3Normalize(&vertices[i].Normal, &vertices[i].Normal);
	}
}

void Terrain::CreateBuffer()
{
	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexColorTextureNormal) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		// &vertices 이건 객체의 주소 0번째거를 넣어야함
		data.pSysMem = &vertices[0];

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &indices[0];

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}


