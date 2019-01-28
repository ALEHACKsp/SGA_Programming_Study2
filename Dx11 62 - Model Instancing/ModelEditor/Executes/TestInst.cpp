#include "stdafx.h"
#include "TestInst.h"

void TestInst::Initialize()
{

}

void TestInst::Ready()
{
	vertices[0].Position = D3DXVECTOR3(-0.2f, -0.2f, 0.0f);
	vertices[1].Position = D3DXVECTOR3(-0.2f,  0.2f, 0.0f);
	vertices[2].Position = D3DXVECTOR3( 0.2f, -0.2f, 0.0f);	 
	vertices[3].Position = D3DXVECTOR3( 0.2f, -0.2f, 0.0f);
	vertices[4].Position = D3DXVECTOR3(-0.2f,  0.2f, 0.0f);
	vertices[5].Position = D3DXVECTOR3( 0.2f,  0.2f, 0.0f);

	CsResource::CreateRawBuffer(sizeof(Vertex) * 6, vertices, &vertexBuffer[0]);

	D3D11_TEXTURE2D_DESC destDesc;
	ZeroMemory(&destDesc, sizeof(D3D11_TEXTURE2D_DESC));
	destDesc.Width = 128 * 4;
	destDesc.Height = 128;
	destDesc.MipLevels = 1; // 꼭 1로 하자 데이터 섞일 수 있음
	destDesc.ArraySize = 1; // 꼭 1로 해야함 데이터 섞일 수 있음
	destDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	destDesc.SampleDesc.Count = 1;
	destDesc.SampleDesc.Quality = 0;
	destDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	destDesc.Usage = D3D11_USAGE_DYNAMIC;
	destDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	//destDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

	HRESULT hr;
	hr = D3D::GetDevice()->CreateTexture2D(&destDesc, NULL, &worldData);
	assert(SUCCEEDED(hr));

	CsResource::CreateSRV(worldData, &worldDataSrv);

	//D3DXMatrixTranslation(&world[0], 0.5f, 0.5f, 0);
	//D3DXMatrixTranslation(&world[1], -0.5f, -0.5f, 0);
	//D3DXMatrixTranslation(&world[2], 0.5f, -0.5f, 0);
	//D3DXMatrixTranslation(&world[3], -0.5f, 0.5f, 0);

	D3DXMatrixTranslation(&world[0][0], 0.5f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&world[0][1], 0.0f, 0.5f, 0.0f);
	D3DXMatrixTranslation(&world[0][2], 0.0f, 0.0f, 0.1f);

	D3DXMatrixTranslation(&world[1][0], -0.4f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&world[1][1], 0.0f, -0.4f, 0.0f);
	D3DXMatrixTranslation(&world[1][2], 0.0f, 0.0f, 0.2f);
	
	// 2차원 배열 테스트
	//D3DXMATRIX mat = *(*(world + 1) + 1);

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(worldData, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		for (int i = 0; i < 2; i++) {
			void * p = ((D3DXMATRIX*)subResource.pData) + 128 * i;
			memcpy(p, world[i], sizeof(D3DXMATRIX) * 3);
		}
		
		//memcpy(subResource.pData, world, sizeof(D3DXMATRIX) * 4);
	}
	D3D::GetDC()->Unmap(worldData, 0);

	colors[0] = D3DXCOLOR(1, 0, 0, 1);
	colors[1] = D3DXCOLOR(0, 1, 0, 1);
	//colors[2] = D3DXCOLOR(0, 0, 1, 1);
	//colors[3] = D3DXCOLOR(1, 0, 1, 1);

	//Create Instance Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		//desc.ByteWidth = sizeof(D3DXCOLOR) * 4;
		desc.ByteWidth = sizeof(D3DXCOLOR) * 2;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = colors;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer[1]);
		assert(SUCCEEDED(hr));
	}

	shader = new Shader(Shaders + L"067_Instancing.fx");
	shader->AsSRV("WorldData")->SetResource(worldDataSrv);
}

void TestInst::Destroy()
{
	SAFE_DELETE(shader);
	SAFE_RELEASE(vertexBuffer[0]);
	SAFE_RELEASE(vertexBuffer[1]);

	SAFE_RELEASE(worldData);
	SAFE_RELEASE(worldDataSrv);
}

void TestInst::Update()
{


}

void TestInst::PreRender()
{
}

void TestInst::Render()
{
	UINT stride[] = { sizeof(Vertex), sizeof(D3DXCOLOR) };
	UINT offset[] = { 0, 0 }; // startVertex처럼 쓸 수 있음 어디서부터 밀어넣을지

	D3D::GetDC()->IASetVertexBuffers(0, 2, vertexBuffer, stride, offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->DrawInstanced(0, 0, 6, 2);
	//shader->DrawInstanced(0, 1, 6, 4);
}

void TestInst::PostRender()
{
}

void TestInst::ResizeScreen()
{
}
