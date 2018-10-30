#include "stdafx.h"
#include "DebugDrawSphere2.h"

DebugDrawSphere2::DebugDrawSphere2(float& radius, int sliceCount)
{
	name = "Sphere2";

	this->range = radius;
	this->sliceCount = sliceCount;
	lines = NULL;

	CreateVertex();

	__super::Init(lines, lineCount, D3DXCOLOR(1,1,0,1));
}

DebugDrawSphere2::~DebugDrawSphere2()
{
	SAFE_DELETE_ARRAY(lines);
}

void DebugDrawSphere2::Render()
{
	__super::Render();
}

void DebugDrawSphere2::PostRender()
{
	__super::PostRender();
}

void DebugDrawSphere2::Set(float& range)
{
	this->range = range;
	D3DXCOLOR color = vertices[0].Color;

	CreateVertex();
	if (vertices != NULL)
		SAFE_DELETE_ARRAY(vertices);
	vertices = new VertexType[vertexCount];

	for (int i = 0; i < vertexCount; i++)
	{
		vertices[i].Position = lines[i];
		vertices[i].Color = color;
	}

	UpdateBuffer();
}

void DebugDrawSphere2::CreateVertex()
{
	if (lines != NULL)
		SAFE_DELETE_ARRAY(lines);

	float phiStep = 2.0f * D3DX_PI / (float)sliceCount;
	
	// Create Vertex
	{
		int vertexCount = sliceCount * 3;
		D3DXVECTOR3* vertices = new D3DXVECTOR3[vertexCount];

		UINT index = 0;
		// x = 0
		for (UINT i = 0; i < sliceCount; i++) {
			float phi = i * phiStep;
			vertices[index] = D3DXVECTOR3(0, (range * cosf(phi)), (range * sinf(phi)));
			index++;
		}
		// y = 0
		for (UINT i = 0; i < sliceCount; i++) {
			float phi = i * phiStep;
			vertices[index] = D3DXVECTOR3((range * cosf(phi)), 0, (range * sinf(phi)));
			index++;
		}
		// z = 0
		for (UINT i = 0; i < sliceCount; i++) {
			float phi = i * phiStep;
			vertices[index] = D3DXVECTOR3((range * cosf(phi)), (range * sinf(phi)), 0);
			index++;
		}


		lineCount = sliceCount * 3;
		lines = new D3DXVECTOR3[lineCount * 2];

		index = 0;
		for (UINT i = 0; i < sliceCount; i++) {
			lines[index++] = vertices[i];
			lines[index++] = i == sliceCount - 1 ? vertices[0] : vertices[i + 1];
		}
		for (UINT i = sliceCount; i < sliceCount * 2; i++) {
			lines[index++] = vertices[i];
			lines[index++] = i == sliceCount * 2 - 1 ? vertices[sliceCount] : vertices[i + 1];
		}
		for (UINT i = sliceCount * 2; i < sliceCount * 3; i++) {
			lines[index++] = vertices[i];
			lines[index++] = i == sliceCount * 3 - 1 ? vertices[sliceCount * 2] : vertices[i + 1];
		}

		SAFE_DELETE_ARRAY(vertices);
	}

	this->vertexCount = lineCount * 2;
}

void DebugDrawSphere2::UpdateBuffer()
{
	SAFE_RELEASE(vertexBuffer);
	// CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT; // ��� ��������� ���� ����
		desc.ByteWidth = sizeof(VertexType) * vertexCount; // ���� ���ۿ� �� �������� ũ��
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA  data = { 0 }; // �긦 ���ؼ� ���� �� lock ���
		data.pSysMem = vertices; // �� �������� �ּ�

		HRESULT hr = D3D::GetDevice()->CreateBuffer(
			&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr)); // �����Ǹ� hr 0���� ū �� �Ѿ��
	}

	__super::UpdateBuffer();
}
