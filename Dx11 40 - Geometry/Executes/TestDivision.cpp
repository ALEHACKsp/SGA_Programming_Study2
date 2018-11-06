#include "stdafx.h"
#include "TestDivision.h"


TestDivision::TestDivision(ExecuteValues * values)
	:Execute(values)
{
	worldBuffer = new WorldBuffer();

	shader = new Shader(Shaders + L"039_Division.hlsl");
	shader->CreateGS();

	VertexTexture vertices[3];
	vertices[0].Position = D3DXVECTOR3(-0.5f, -0.5f, 0.0f);
	vertices[1].Position = D3DXVECTOR3( 0.0f,  0.5f, 0.0f);
	vertices[2].Position = D3DXVECTOR3( 0.5f, -0.5f, 0.0f);

	vertices[0].Uv = D3DXVECTOR2(0, 1);
	vertices[1].Uv = D3DXVECTOR2(0.5f, 0.5f);
	vertices[2].Uv = D3DXVECTOR2(1, 1);	

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTexture) * 3;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	rasterizerState[0] = new RasterizerState();
	rasterizerState[1] = new RasterizerState();
	rasterizerState[1]->FillMode(D3D11_FILL_WIREFRAME);
}

TestDivision::~TestDivision()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(worldBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE(rasterizerState[0]);
	SAFE_DELETE(rasterizerState[1]);
}

void TestDivision::Update()
{

}

void TestDivision::PreRender()
{

}

void TestDivision::Render()
{
	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	values->ViewProjection->SetGSBuffer(0);
	worldBuffer->SetGSBuffer(1);

	shader->Render();

	rasterizerState[1]->RSSetState();
	D3D::GetDC()->Draw(3, 0);
	rasterizerState[0]->RSSetState();

	// 이거 안해주면 문제 생김
	D3D::GetDC()->GSSetShader(NULL, NULL, 0);
}

void TestDivision::PostRender()
{

}
