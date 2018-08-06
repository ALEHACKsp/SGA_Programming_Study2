#include "stdafx.h"
#include "ExeGrid.h"

#include "ExeLine.h"

ExeGrid::ExeGrid(ExecuteValues * values)
	:Execute(values)
	, isDrawLine(true), lineLength(5.0f)
{
	// loadInfo 이미지 불러올 때 쓰는거 그냥 읽어오면 256으로 맞춰짐(계산빠르게하기위해서)
	Texture* heightMap = new Texture(Contents + L"HeightMaps/HeightMap.png");
	vector<D3DXCOLOR> heights;
	heightMap->ReadPixels(DXGI_FORMAT_R8G8B8A8_UNORM, &heights);

	width = heightMap->GetWidth() - 1; // 점의 갯수 때문에 -1 한 거
	height = heightMap->GetHeight() - 1; // 점의 갯수 때문에 -1 한 거

	shader = new Shader(Shaders + L"008_Diffuse.hlsl", "VS", "PS_Diffuse");
	worldBuffer = new WorldBuffer();

	colorBuffer = new ColorBuffer;

	texture[0] = new Texture(Contents + L"Textures/Dirt.png");
	texture[1] = new Texture(Contents + L"Textures/Wall.png");
	texture[2] = new Texture(Contents + L"HeightMaps/AlphaMap.png");

	// Create VertexData
	{
		vertexCount = (width + 1) * (height + 1);
		vertices = new VertexType[vertexCount];

		// z축으로 하는 이유 3d라서
		for (UINT z = 0; z <= height; z++) {
			for (UINT x = 0; x <= width; x++) {
				UINT index = (width + 1) * z + x;

				vertices[index].Position.x = (float)x;
				// 선생님은 r값을 씀 그냥 쓰면 너무 높아져서 임의로 나눠준거
				vertices[index].Position.y = (float)(heights[index].r * 255.0f) / 7.5f;
				vertices[index].Position.z = (float)z;

				vertices[index].Uv.x = (float)x / width;
				vertices[index].Uv.y = (float)z / height;
			}
		} // for(z) 
	}

	// Create IndexData
	{
		indexCount = width * height * 6;
		indices = new UINT[indexCount];

		UINT index = 0;
		for (UINT z = 0; z < height; z++) {
			for (UINT x = 0; x < width; x++) {
				// 0번 index 좌측 하단
				indices[index + 0] = (width + 1) * z + x; 
				// 1번 index 좌측 상단
				indices[index + 1] = (width + 1) * (z + 1) + x; 
				// 2번 index 우측 하단
				indices[index + 2] = (width + 1) * z + x + 1; 
			
				// 2번 index 우측 하단
				indices[index + 3] = (width + 1) * z + x + 1; 
				// 1번 index 좌측 상단
				indices[index + 4] = (width + 1) * (z + 1) + x; 
				// 3번 index 좌측 상단
				indices[index + 5] = (width + 1) * (z + 1) + x + 1; 

				index += 6;
			}
		} // for(z)
	}

	CreateNormal();

	// Draw Normal Line
	{
		D3DXVECTOR3 * startPos = new D3DXVECTOR3[vertexCount];
		D3DXVECTOR3 * dir = new D3DXVECTOR3[vertexCount];

		for (UINT i = 0; i < vertexCount; i++) {
			startPos[i] = vertices[i].Position;
			dir[i] = vertices[i].Normal;
		}

		normalLine = new ExeLine(values, startPos, dir, vertexCount, 
			lineLength);

		SAFE_DELETE_ARRAY(startPos);
		SAFE_DELETE_ARRAY(dir);
	}

	// CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT; // 어떻게 저장될지에 대한 정보
		desc.ByteWidth = sizeof(VertexType) * vertexCount; // 정점 버퍼에 들어갈 데이터의 크기
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA  data = { 0 }; // 얘를 통해서 값이 들어감 lock 대신
		data.pSysMem = vertices; // 쓸 데이터의 주소

		HRESULT hr = D3D::GetDevice()->CreateBuffer(
			&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr)); // 성공되면 hr 0보다 큰 값 넘어옴
	}

	// CreateIndexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT; // 어떻게 저장될지에 대한 정보
		desc.ByteWidth = sizeof(UINT) * indexCount; // 정점 버퍼에 들어갈 데이터의 크기
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA  data = { 0 }; // 얘를 통해서 값이 들어감 lock 대신
		data.pSysMem = indices; // 쓸 데이터의 주소

		HRESULT hr = D3D::GetDevice()->CreateBuffer(
			&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr)); // 성공되면 hr 0보다 큰 값 넘어옴
	}
}

ExeGrid::~ExeGrid()
{
	// dx로부터 만들어진건 release
	// 보통 역순으로 릴리즈함
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(vertices);

	SAFE_DELETE(colorBuffer);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);

	for (int i = 0; i < 3; i++)
		SAFE_DELETE(texture[i]);

	SAFE_DELETE(normalLine);
}

void ExeGrid::Update()
{
	if (isDrawLine)
		normalLine->Update();
}

void ExeGrid::PreRender()
{
	if (isDrawLine)
		normalLine->PreRender();
}

void ExeGrid::Render()
{
	UINT stride = sizeof(VertexType); // 그릴 크기
	UINT offset = 0;

	// vertex buffer 여러개 들어갈 수 있음
	// IA 붙는 이유 나중에 설명해주실꺼
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// 그릴 방식 설정
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	texture[0]->SetShaderResource(0);
	texture[1]->SetShaderResource(1);
	texture[2]->SetShaderResource(2);

	colorBuffer->SetPSBuffer(0); // 레지스터 번호
	worldBuffer->SetVSBuffer(1);
	shader->Render();

	// 실제로 그리는 거
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);

	if(isDrawLine)
		normalLine->Render();
}

void ExeGrid::PostRender()
{
	ImGui::SliderFloat3("Direction", 
		(float *)colorBuffer->Data.Light, -1, 1);
	ImGui::SliderInt("DrawLine", &isDrawLine, 0, 1);
	if (ImGui::SliderFloat("LineLength", &lineLength, 0.01f, 10.0f))
		normalLine->ResizeLength(lineLength);
	normalLine->PostRender();
}

void ExeGrid::ResizeScreen()
{
}

// 정점이 만들어지고 난 다음에 해야함
void ExeGrid::CreateNormal()
{
	for (UINT i = 0; i < (indexCount / 3); i++) {
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		VertexTextureNormal v0 = vertices[index0];
		VertexTextureNormal v1 = vertices[index1];
		VertexTextureNormal v2 = vertices[index2];

		D3DXVECTOR3 d1 = v1.Position - v0.Position;
		D3DXVECTOR3 d2 = v2.Position - v0.Position;

		D3DXVECTOR3 normal;
		// 순서 중요 뒤집히면 정상계산하면 안됨
		D3DXVec3Cross(&normal, &d1, &d2);

		vertices[index0].Normal += normal;
		vertices[index1].Normal += normal;
		vertices[index2].Normal += normal;

		// 3점의 평균을 쓰려고 함 
		// -> 평균을 구하기위해 나눠주는게 아니라 Normalize로 쓸꺼
	}

	for (UINT i = 0; i < vertexCount; i++)
		D3DXVec3Normalize(&vertices[i].Normal, &vertices[i].Normal);
}
