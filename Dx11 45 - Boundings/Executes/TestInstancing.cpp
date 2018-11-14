#include "stdafx.h"
#include "TestInstancing.h"


TestInstancing::TestInstancing(ExecuteValues * values)
	:Execute(values)
{
	//shader = new Shader(Shaders + L"043_World.hlsl");
	shader = new Shader(Shaders + L"044_Instance.hlsl");

	worldBuffer = new WorldBuffer();

	VertexTexture vertex[6];
	vertex[0].Position = D3DXVECTOR3(-0.5f, -0.5f, 0);
	vertex[1].Position = D3DXVECTOR3(-0.5f, 0.5f, 0);
	vertex[2].Position = D3DXVECTOR3(0.5f, -0.5f, 0);
	vertex[3].Position = D3DXVECTOR3(0.5f, -0.5f, 0);
	vertex[4].Position = D3DXVECTOR3(-0.5f, 0.5f, 0);
	vertex[5].Position = D3DXVECTOR3(0.5f, 0.5f, 0);

	vertex[0].Uv = D3DXVECTOR2(0, 1);
	vertex[1].Uv = D3DXVECTOR2(0, 0);
	vertex[2].Uv = D3DXVECTOR2(1, 1);
	vertex[3].Uv = D3DXVECTOR2(1, 1);
	vertex[4].Uv = D3DXVECTOR2(0, 0);
	vertex[5].Uv = D3DXVECTOR2(1, 0);

	//Create Vertex Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTexture) * 6;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertex;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer[0]);
		assert(SUCCEEDED(hr));
	}

	Vertex instance[6];
	instance[0].Position = D3DXVECTOR3(-3.5f, -3.5f, 0);
	instance[1].Position = D3DXVECTOR3(-3.5f, 3.5f, 0);
	instance[2].Position = D3DXVECTOR3(3.5f, -3.5f, 0);
	instance[3].Position = D3DXVECTOR3(3.5f, 3.5f, 0);
	instance[4].Position = D3DXVECTOR3(6.5f, -6.5f, 0);
	instance[5].Position = D3DXVECTOR3(9.5f, 9.5f, 0);

	//Create Instance Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(Vertex) * 6;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = instance;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer[1]);
		assert(SUCCEEDED(hr));
	}

	texture = new Texture(Textures + L"Floor.png");

	vector<wstring> names =
	{
		Textures + L"Floor.png",
		Textures + L"Box.png",
		Textures + L"Dirt.png",
		Textures + L"Dirt2.png",
		Textures + L"Dirt3.png",
		Textures + L"Wall.png",
	};

	textures = new TextureArray(names);
}

TestInstancing::~TestInstancing()
{
	SAFE_RELEASE(vertexBuffer[0]);
	SAFE_RELEASE(vertexBuffer[1]);

	SAFE_DELETE(worldBuffer);
	
	SAFE_DELETE(shader);

	SAFE_DELETE(texture);
}

void TestInstancing::Update()
{

}

void TestInstancing::PreRender()
{

}

void TestInstancing::Render()
{
	UINT stride[2] = { sizeof(VertexTexture), sizeof(Vertex) };
	UINT offset[2] = { 0, 0 };

	D3D::GetDC()->IASetVertexBuffers(0, 2, vertexBuffer, stride, offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->Render();

	ID3D11ShaderResourceView* srv = textures->GetSRV();

	D3D::GetDC()->PSSetShaderResources(10, 1, &srv);
	//texture->SetShaderResource(10);

	worldBuffer->SetVSBuffer(1);

	D3D::GetDC()->DrawInstanced(6, 6, 0, 0);
}

void TestInstancing::PostRender()
{

}

void TestInstancing::WorldRender()
{
	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer[0], &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->Render();
	texture->SetShaderResource(10);

	// 이 방법 인스턴싱하기 전
	D3DXMATRIX W, S, T;
	D3DXMatrixScaling(&S, 3, 3, 3);
	D3DXMatrixTranslation(&T, 0, 0, 0);
	W = S * T;

	worldBuffer->SetMatrix(W);
	worldBuffer->SetVSBuffer(1);
	D3D::GetDC()->Draw(6, 0);

	D3DXMatrixScaling(&S, 5, 5, 5);
	D3DXMatrixTranslation(&T, 3, 0, 0);
	W = S * T;

	worldBuffer->SetMatrix(W);
	worldBuffer->SetVSBuffer(1);
	D3D::GetDC()->Draw(6, 0);
}
