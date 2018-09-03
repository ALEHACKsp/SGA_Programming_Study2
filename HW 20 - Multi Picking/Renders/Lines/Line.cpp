#include "stdafx.h"
#include "Line.h"

Line::Line()
	: vertices(NULL)
{

}

Line::~Line()
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
}

void Line::Init(D3DXVECTOR3 * positions, UINT lineCount, D3DXCOLOR color, 
	D3D11_PRIMITIVE_TOPOLOGY topology)
{
	shader = new Shader(Shaders + L"003_Color.hlsl");
	worldBuffer = new WorldBuffer();

	colorBuffer = new ColorBuffer;

	this->topology = topology;
	
	// Create VertexData
	{
		if (topology == D3D11_PRIMITIVE_TOPOLOGY_LINELIST)
			vertexCount = lineCount * 2;
		else if (topology == D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP)
			vertexCount = lineCount + 1;

		vertices = new VertexType[vertexCount];

		for (UINT i = 0; i < vertexCount; i++) {
			vertices[i].Position = positions[i];
			vertices[i].Color = color;
		}
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
}

void Line::Render()
{
	UINT stride = sizeof(VertexType); // 그릴 크기
	UINT offset = 0;

	// vertex buffer 여러개 들어갈 수 있음
	// IA 붙는 이유 나중에 설명해주실꺼
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	//D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// 그릴 방식 설정
	D3D::GetDC()->IASetPrimitiveTopology(topology);

	colorBuffer->SetPSBuffer(0); // 레지스터 번호
	worldBuffer->SetVSBuffer(1);
	shader->Render();

	// 실제로 그리는 거
	//D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
	D3D::GetDC()->Draw(vertexCount, 0);
}

void Line::PostRender()
{
}

void Line::SetColor(D3DXCOLOR color)
{
	for (UINT i = 0; i < vertexCount; i++) {
		vertices[i].Color = color;
	}

	UpdateBuffer();
}

void Line::UpdateBuffer()
{
	D3D::GetDC()->UpdateSubresource(
		vertexBuffer, 0, NULL, &vertices[0],
		sizeof(VertexType) * vertexCount, 0
	);
}

void Line::SetLine(D3DXVECTOR3 * positions, UINT lineCount, D3D11_PRIMITIVE_TOPOLOGY topology)
{
	D3DXCOLOR color = vertices[0].Color;
	SAFE_DELETE(vertices);

	// Create VertexData
	{
		if (topology == D3D11_PRIMITIVE_TOPOLOGY_LINELIST)
			vertexCount = lineCount * 2;
		else if (topology == D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP)
			vertexCount = lineCount + 1;

		vertices = new VertexType[vertexCount];

		for (UINT i = 0; i < vertexCount; i++) {
			vertices[i].Position = positions[i];
			vertices[i].Color = color;
		}
	}

	UpdateBuffer();
}
