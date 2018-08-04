#include "stdafx.h"
#include "ExeGrid.h"

ExeGrid::ExeGrid(ExecuteValues * values)
	:Execute(values)
{
	// loadInfo 이미지 불러올 때 쓰는거 그냥 읽어오면 256으로 맞춰짐(계산빠르게하기위해서)
	Texture* heightMap = new Texture(Contents + 
		//L"HeightMaps/Homework/SGA Logo.png");
		//L"HeightMaps/Homework/SGA Logo2.png");
		//L"HeightMaps/Homework/SGA Logo3.png");
		L"HeightMaps/Homework/SGA Logo4.png");
		//L"HeightMaps/Homework/test.png");
		//L"HeightMaps/Homework/test2.png");
	vector<D3DXCOLOR> heights;
	heightMap->ReadPixels(DXGI_FORMAT_R8G8B8A8_UNORM, &heights);

	width = heightMap->GetWidth() - 1; // 점의 갯수 때문에 -1 한 거
	height = heightMap->GetHeight() - 1; // 점의 갯수 때문에 -1 한 거

	shader = new Shader(Shaders + L"/Homework/HeightMap.hlsl");
	worldBuffer = new WorldBuffer();

	colorBuffer = new ColorBuffer;

	// Create VertexData
	{
		vertexCount = (width + 1) * (height + 1);
		vertices = new VertexColor[vertexCount];

		// z축으로 하는 이유 3d라서
		for (UINT z = 0; z <= height; z++) {
			for (UINT x = 0; x <= width; x++) {
				UINT index = (width + 1) * z + x;

				vertices[index].Position.x = (float)x;
				// 선생님은 r값을 씀 그냥 쓰면 너무 높아져서 임의로 나눠준거
				vertices[index].Position.y = (float)(heights[index].a * 255.0f) / 7.5f;
				vertices[index].Position.z = (float)(height - z);
				//vertices[index].Position.z = (float)(height - z);

				if (heights[index].a == 0) {
					vertices[index].Color = D3DXCOLOR(1, 1, 1, 1);
				}
				else {
					vertices[index].Color = D3DXCOLOR(
						heights[index].r,
						heights[index].g,
						heights[index].b, 1);
				}
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
				indices[index + 1] = (width + 1) * (z - 1) + x; 
				// 2번 index 우측 하단
				indices[index + 2] = (width + 1) * z + x + 1; 
			
				// 2번 index 우측 하단
				indices[index + 3] = (width + 1) * z + x + 1; 
				// 1번 index 좌측 상단
				indices[index + 4] = (width + 1) * (z - 1) + x; 
				// 3번 index 좌측 상단
				indices[index + 5] = (width + 1) * (z - 1) + x + 1; 

				index += 6;
			}
		} // for(z)
	}

	// CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT; // 어떻게 저장될지에 대한 정보
		desc.ByteWidth = sizeof(VertexColor) * vertexCount; // 정점 버퍼에 들어갈 데이터의 크기
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

	{
		fillModeNumber = 0;

		D3D11_RASTERIZER_DESC desc;
		States::GetRasterizerDesc(&desc);

		States::CreateRasterizer(&desc, &fillMode[0]);

		desc.FillMode = D3D11_FILL_WIREFRAME;
		States::CreateRasterizer(&desc, &fillMode[1]);
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

	for (int i = 0; i < 2; i++)
		SAFE_RELEASE(fillMode[i]);
}

void ExeGrid::Update()
{

}

void ExeGrid::PreRender()
{
}

void ExeGrid::Render()
{
	UINT stride = sizeof(VertexColor); // 그릴 크기
	UINT offset = 0;

	// vertex buffer 여러개 들어갈 수 있음
	// IA 붙는 이유 나중에 설명해주실꺼
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// 그릴 방식 설정
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D::GetDC()->RSSetState(fillMode[fillModeNumber]);

	colorBuffer->SetPSBuffer(0); // 레지스터 번호
	worldBuffer->SetVSBuffer(1);
	shader->Render();

	// 실제로 그리는 거
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
}

void ExeGrid::PostRender()
{
	// ImGui begin 안열면 Debug창에 들어감
	ImGui::SliderInt("Wireframe", &fillModeNumber, 0, 1);
}

void ExeGrid::ResizeScreen()
{
}
