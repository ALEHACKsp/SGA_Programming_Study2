#include "stdafx.h"
#include "DebugDrawSphere.h"

DebugDrawSphere::DebugDrawSphere(float radius, int stackCount, int sliceCount)
{
	name = "Sphere";

	this->radius = radius;

	this->stackCount = stackCount;
	this->sliceCount = sliceCount;
	lines = NULL;

	CreateVertex();

	__super::Init(lines, lineCount);
}

DebugDrawSphere::~DebugDrawSphere()
{
	SAFE_DELETE_ARRAY(lines);
}

void DebugDrawSphere::Render()
{
	__super::Render();
}

void DebugDrawSphere::PostRender()
{
	__super::PostRender();

	ImGui::ColorEdit4("Color", (float*)colorBuffer->Data.Color);

	if (ImGui::DragFloat("Radius", &radius) ||
		ImGui::InputInt("StackCount", &stackCount) ||
		ImGui::InputInt("SliceCount", &sliceCount))
		Set(radius);
}

void DebugDrawSphere::Radius(float & radius)
{
	Scale(radius, radius, radius);
}

float DebugDrawSphere::Radius()
{
	return Scale().x;
}

void DebugDrawSphere::Set(float& radius)
{
	this->radius = radius;

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

void DebugDrawSphere::Set(float & radius, D3DXCOLOR & color)
{
	this->radius = radius;

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

void DebugDrawSphere::CreateVertex()
{
	if (lines != NULL)
		SAFE_DELETE_ARRAY(lines);

	float phiStep = D3DX_PI / stackCount;
	float thetaStep = 2.0f * D3DX_PI / sliceCount;
	
	// Create Vertex
	{
		int vertexCount = 2 + (sliceCount + 1) * (stackCount - 1);
		D3DXVECTOR3* vertices = new D3DXVECTOR3[vertexCount];

		UINT index = 0;
		for (UINT i = 0; i <= stackCount; i++) {
			// 위와 밑의 꼭지점 처리
			if (i == 0 || i == stackCount) {
				vertices[index] = D3DXVECTOR3(0, i == 0 ? radius : -radius, 0);

				index++;
			}
			else {
				float phi = i * phiStep;
				for (UINT j = 0; j <= sliceCount; j++) {
					float theta = j * thetaStep;
					vertices[index] = D3DXVECTOR3(
						(radius * sinf(phi) * cosf(theta)),
						(radius * cosf(phi)),
						(radius * sinf(phi) * sinf(theta)));

					index++;
				}
			}
		}

		lineCount = (2 + (stackCount-2) + (stackCount-1)) * sliceCount;
		lines = new D3DXVECTOR3[lineCount * 2];

		index = 0;
		// 윗면
		for (UINT i = 1; i <= sliceCount; i++) {
			lines[index++] = vertices[0];
			lines[index++] = vertices[i];
		}

		// 옆면
		UINT baseIndex = 1; // 위 꼭지점 빼려고
		UINT ringVertexCount = sliceCount + 1;
		// stackCount - 2 인 이유 정점 4개 써서 위 꼭지 아래꼭지 빼고 -1 해줘야함
		for (UINT i = 0; i < stackCount - 1; i++) {

			for (UINT j = 0; j < sliceCount; j++) {
				// 세로줄
				if (i < stackCount - 2)
				{
					lines[index++] = vertices[baseIndex + i * ringVertexCount + j];
					lines[index++] = vertices[baseIndex + (i + 1) * ringVertexCount + j];
				}

				// 가로줄
				lines[index++] = vertices[baseIndex + i * ringVertexCount + j];
				lines[index++] = vertices[baseIndex + i * ringVertexCount + (j + 1)];
			}
		}

		// 아랫면
		UINT southPoleIndex = vertexCount - 1;
		baseIndex = southPoleIndex - ringVertexCount;
		for (UINT i = 0; i < sliceCount; i++) {
			lines[index++] = vertices[southPoleIndex];
			lines[index++] = vertices[baseIndex + i];
		}

		SAFE_DELETE_ARRAY(vertices);
	}

	this->vertexCount = lineCount * 2;
}

void DebugDrawSphere::UpdateBuffer()
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
