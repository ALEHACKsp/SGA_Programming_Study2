#include "stdafx.h"
#include "ExeLine.h"

ExeLine::ExeLine(ExecuteValues* values,
	D3DXVECTOR3 * startPos, D3DXVECTOR3 * direction,
	UINT lineCount, float length)
	:Execute(values),
	vertexCount(lineCount * 2), length(length)
{
	shader = new Shader(Shaders + L"003_Color.hlsl");
	worldBuffer = new WorldBuffer();

	colorBuffer = new ColorBuffer;

	// Create VertexData
	{
		vertices = new VertexType[vertexCount];
		dir = new D3DXVECTOR3[vertexCount / 2];

		for (UINT i = 0; i < vertexCount; i++) {
			if (i % 2 == 0) {
				vertices[i].Position = startPos[i / 2];
				vertices[i].Color = D3DXCOLOR(1,1,1,1);
			}
			else {
				dir[i/2] = direction[i / 2];
				vertices[i].Color = vertices[i - 1].Color;
				vertices[i].Position =
					startPos[i/2] + dir[i/2] * length;
			}
		}
	}

	// Create IndexData
	{

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
		//D3D11_BUFFER_DESC desc = { 0 };
		//desc.Usage = D3D11_USAGE_DEFAULT; // 어떻게 저장될지에 대한 정보
		//desc.ByteWidth = sizeof(UINT) * indexCount; // 정점 버퍼에 들어갈 데이터의 크기
		//desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		//D3D11_SUBRESOURCE_DATA  data = { 0 }; // 얘를 통해서 값이 들어감 lock 대신
		//data.pSysMem = indices; // 쓸 데이터의 주소

		//HRESULT hr = D3D::GetDevice()->CreateBuffer(
		//	&desc, &data, &indexBuffer);
		//assert(SUCCEEDED(hr)); // 성공되면 hr 0보다 큰 값 넘어옴
	}
}

ExeLine::~ExeLine()
{
	// dx로부터 만들어진건 release
	// 보통 역순으로 릴리즈함
	//SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	//SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(vertices);

	SAFE_DELETE(colorBuffer);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);

	SAFE_DELETE_ARRAY(dir);
}

void ExeLine::Update()
{

}

void ExeLine::PreRender()
{
}

void ExeLine::Render()
{
	UINT stride = sizeof(VertexType); // 그릴 크기
	UINT offset = 0;

	// vertex buffer 여러개 들어갈 수 있음
	// IA 붙는 이유 나중에 설명해주실꺼
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	//D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// 그릴 방식 설정
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	colorBuffer->SetPSBuffer(0); // 레지스터 번호
	worldBuffer->SetVSBuffer(1);
	shader->Render();

	// 실제로 그리는 거
	//D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
	D3D::GetDC()->Draw(vertexCount, 0);
}

void ExeLine::PostRender()
{

}

void ExeLine::ResizeScreen()
{
}

void ExeLine::ResizeLength(float length)
{
	this->length = length;
	for (UINT i = 0; i < vertexCount; i++) {
		if (i % 2 == 0) continue;
		vertices[i].Position =
			vertices[i - 1].Position + dir[i / 2] * length;	
	}

	// 기존 vertexBuffer release
	SAFE_RELEASE(vertexBuffer);

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
