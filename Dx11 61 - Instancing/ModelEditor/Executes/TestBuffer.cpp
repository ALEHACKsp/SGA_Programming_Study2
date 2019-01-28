#include "stdafx.h"
#include "TestBuffer.h"

void TestBuffer::Initialize()
{

}

void TestBuffer::Ready()
{
	vertices[0].Position = D3DXVECTOR3(-0.2f, -0.2f, 0.0f);
	vertices[1].Position = D3DXVECTOR3(-0.2f,  0.2f, 0.0f);
	vertices[2].Position = D3DXVECTOR3( 0.2f, -0.2f, 0.0f);	 
	vertices[3].Position = D3DXVECTOR3( 0.2f, -0.2f, 0.0f);
	vertices[4].Position = D3DXVECTOR3(-0.2f,  0.2f, 0.0f);
	vertices[5].Position = D3DXVECTOR3( 0.2f,  0.2f, 0.0f);

	CsResource::CreateRawBuffer(sizeof(Vertex) * 6, vertices, &vertexBuffer);

	vertices2[0] = D3DXVECTOR3(0.2f, 0.0f, 0.0f);
	vertices2[1] = D3DXVECTOR3(0.2f, 0.0f, 0.0f);
	vertices2[2] = D3DXVECTOR3(0.2f, 0.0f, 0.0f);
	vertices2[3] = D3DXVECTOR3(0.2f, 0.0f, 0.0f);
	vertices2[4] = D3DXVECTOR3(0.2f, 0.0f, 0.0f);
	vertices2[5] = D3DXVECTOR3(0.2f, 0.0f, 0.0f);

	CsResource::CreateStructuredBuffer(sizeof(D3DXVECTOR3), 6, vertices2, &testBuffer);
	CsResource::CreateSRV(testBuffer, &testSrv);

	shader = new Shader(Shaders + L"066_StructuredBuffer.fx");
	shader->AsSRV("Input")->SetResource(testSrv);
}

void TestBuffer::Destroy()
{
	SAFE_DELETE(shader);
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(testBuffer);
	SAFE_RELEASE(testSrv);
}

void TestBuffer::Update()
{

	if (Keyboard::Get()->Press('A'))
	{
		for (int i = 0; i < 6; i++)
			vertices2[i].x -= Time::Delta() * 2.0f;

		D3D11_MAPPED_SUBRESOURCE subResource;
		D3D::GetDC()->Map(testBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		{
			memcpy(subResource.pData, vertices2, sizeof(D3DXVECTOR3) * 6);
		}
		D3D::GetDC()->Unmap(testBuffer, 0);
	}
	if (Keyboard::Get()->Press('D'))
	{
		for (int i = 0; i < 6; i++)
			vertices2[i].x += Time::Delta() * 2.0f;

		D3D11_MAPPED_SUBRESOURCE subResource;
		D3D::GetDC()->Map(testBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		{
			memcpy(subResource.pData, vertices2, sizeof(D3DXVECTOR3) * 6);
		}
		D3D::GetDC()->Unmap(testBuffer, 0);
	}
}

void TestBuffer::PreRender()
{
}

void TestBuffer::Render()
{
	UINT stride[] = { sizeof(Vertex) };
	UINT offset[] = { 0 }; // startVertex처럼 쓸 수 있음 어디서부터 밀어넣을지

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, stride, offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->Draw(0, 0, 6);
}

void TestBuffer::PostRender()
{
}

void TestBuffer::ResizeScreen()
{
}
