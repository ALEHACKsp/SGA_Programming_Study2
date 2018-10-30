#include "stdafx.h"
#include "DebugDrawCone.h"

DebugDrawCone::DebugDrawCone(
	D3DXVECTOR3& center, float& height, float angle, int sliceCount)
{
	name = "Cone";
	this->center = center;
	this->angle = angle;
	this->height = height;
	this->sliceCount = sliceCount;
	lines = NULL;

	radius = height * tanf(angle);

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
	__super::PostRender();

	float degree = Math::ToDegree(angle);

	ImGui::ColorEdit4("Color", (float*)colorBuffer->Data.Color);

	if (ImGui::DragFloat3("Center Pos", (float*)&center)
		|| ImGui::DragFloat("Height", &height)
		|| ImGui::DragFloat("Degree", &degree)) {
		SetPosition(center, height, Math::ToRadian(degree));
	}
}

void DebugDrawCone::SetPosition(D3DXVECTOR3& center, float& height, float angle)
{
	this->center = center;
	this->angle = angle;
	this->height = height;

	radius = height * tanf(angle);

	CreateVertex();
	if (vertices != NULL)
		SAFE_DELETE_ARRAY(vertices);
	vertices = new VertexType[vertexCount];

	for (int i = 0; i < vertexCount; i++)
	{
		vertices[i].Position = lines[i];
	}

	UpdateBuffer();
}

void DebugDrawCone::Set(D3DXVECTOR3 & center, float & height, float & angle, D3DXCOLOR & color)
{
	this->center = center;
	this->angle = angle;
	this->height = height;
	
	radius = height * tanf(angle);

	colorBuffer->Data.Color = color;

	CreateVertex();
	if (vertices != NULL)
		SAFE_DELETE_ARRAY(vertices);
	vertices = new VertexType[vertexCount];

	for (int i = 0; i < vertexCount; i++)
	{
		vertices[i].Position = lines[i];
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
		int vertexCount = sliceCount * 2 + 1;
		D3DXVECTOR3* vertices = new D3DXVECTOR3[vertexCount];

		vertices[0] = center;

		UINT index = 1;
		for (int i = 0; i < sliceCount; i++) {
			float phi = i * phiStep;
			vertices[index++] = D3DXVECTOR3(
				center.x + radius * cosf(phi),
				center.y - height,
				center.z + radius * sinf(phi)
			);
		}

		lineCount = sliceCount * 2;
		lines = new D3DXVECTOR3[lineCount * 2];

		index = 0;
		for (int i = 1; i <= sliceCount; i++) {
			lines[index++] = vertices[0];
			lines[index++] = vertices[i];
		}
		for (int i = 1; i <= sliceCount; i++) {
			lines[index++] = vertices[i];
			lines[index++] = i == sliceCount ? vertices[1] : vertices[i + 1];
		}

	}

	this->vertexCount = lineCount * 2;
}

void DebugDrawCone::UpdateBuffer()
{
	SAFE_RELEASE(vertexBuffer);
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

	__super::UpdateBuffer();
}
