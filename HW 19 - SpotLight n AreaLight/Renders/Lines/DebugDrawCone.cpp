#include "stdafx.h"
#include "DebugDrawCone.h"

DebugDrawCone::DebugDrawCone(
	D3DXVECTOR3& center, float& radius, float& height, int sliceCount)
{
	name = "Cone";
	this->center = center;
	this->radius = radius;
	this->height = height;
	this->sliceCount = sliceCount;
	lines = NULL;

	CreateVertex();

	__super::Init(lines, lineCount, D3DXCOLOR(1,1,1,1));
}

DebugDrawCone::~DebugDrawCone()
{
	SAFE_DELETE_ARRAY(lines);
}

void DebugDrawCone::Render()
{
	__super::Render();
}

void DebugDrawCone::PostRender()
{

}

void DebugDrawCone::SetPosition(D3DXVECTOR3& center, float& radius, float& height)
{
	this->center = center;
	this->radius = radius;
	this->height = height;
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

void DebugDrawCone::Set(D3DXVECTOR3 & center, float & radius, float & height, D3DXCOLOR & color)
{
	this->center = center;
	this->radius = radius;
	this->height = height;
	
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

void DebugDrawCone::CreateVertex()
{
	if (lines != NULL)
		SAFE_DELETE_ARRAY(lines);

	float phiStep = 2.0f * D3DX_PI / sliceCount;
	
	// Create Vertex
	{
		int vertexCount = sliceCount + 5;
		D3DXVECTOR3* vertices = new D3DXVECTOR3[vertexCount];

		UINT index = 0;
		for (int i = 0; i < sliceCount; i++) {
			float phi = i * phiStep;
			vertices[index] = center + D3DXVECTOR3(radius * cosf(phi), 0, radius * sinf(phi));
			index++;
		}
		vertices[index++] = center + D3DXVECTOR3(0, height, 0);
		float phi = 2.0f * D3DX_PI / 4;
		vertices[index++] = center + D3DXVECTOR3(radius * cosf(phi), 0, radius * sinf(phi));
		phi += 2.0f * D3DX_PI / 4;
		vertices[index++] = center + D3DXVECTOR3(radius * cosf(phi), 0, radius * sinf(phi));
		phi += 2.0f * D3DX_PI / 4;
		vertices[index++] = center + D3DXVECTOR3(radius * cosf(phi), 0, radius * sinf(phi));
		phi += 2.0f * D3DX_PI / 4;
		vertices[index++] = center + D3DXVECTOR3(radius * cosf(phi), 0, radius * sinf(phi));


		lineCount = sliceCount + 4;
		lines = new D3DXVECTOR3[lineCount * 2];

		index = 0;
		for (UINT i = 0; i < sliceCount; i++)
		{
			lines[index++] = vertices[i];
			lines[index++] = i == sliceCount - 1 ? vertices[0] : vertices[i + 1];
		}
		lines[index++] = vertices[sliceCount];
		lines[index++] = vertices[sliceCount + 1];
		lines[index++] = vertices[sliceCount];
		lines[index++] = vertices[sliceCount + 2];
		lines[index++] = vertices[sliceCount];
		lines[index++] = vertices[sliceCount + 3];
		lines[index++] = vertices[sliceCount];
		lines[index++] = vertices[sliceCount + 4];
	}

	this->vertexCount = lineCount * 2;
}

void DebugDrawCone::UpdateBuffer()
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
