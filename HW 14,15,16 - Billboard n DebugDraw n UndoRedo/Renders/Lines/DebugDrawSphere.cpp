#include "stdafx.h"
#include "DebugDrawSphere.h"

DebugDrawSphere::DebugDrawSphere(D3DXVECTOR3& center, float& radius,
	int stackCount, int sliceCount)
{
	name = "Sphere";

	this->center = center;
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
	float cp[] = { center.x, center.y,center.z };

	D3DXCOLOR color = vertices[0].Color;
	float c[] = { color.r,color.g,color.b,color.a };
	if (ImGui::ColorEdit4("Color", c))
		SetColor(D3DXCOLOR(c));

	if (ImGui::DragFloat3("Center Pos", cp) ||
		ImGui::DragFloat("Radius", &radius) ||
		ImGui::InputInt("StackCount", &stackCount) ||
		ImGui::InputInt("SliceCount", &sliceCount))
		SetPosition(D3DXVECTOR3(cp));
}

void DebugDrawSphere::SetPosition(D3DXVECTOR3& center)
{
	this->center = center;
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
			// ���� ���� ������ ó��
			if (i == 0 || i == stackCount) {
				vertices[index] = center + 
					D3DXVECTOR3(0, i == 0 ? radius : -radius, 0);

				index++;
			}
			else {
				float phi = i * phiStep;
				for (UINT j = 0; j <= sliceCount; j++) {
					float theta = j * thetaStep;
					vertices[index] = center + D3DXVECTOR3(
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
		// ����
		for (UINT i = 1; i <= sliceCount; i++) {
			lines[index++] = vertices[0];
			lines[index++] = vertices[i];
		}

		// ����
		UINT baseIndex = 1; // �� ������ ������
		UINT ringVertexCount = sliceCount + 1;
		// stackCount - 2 �� ���� ���� 4�� �Ἥ �� ���� �Ʒ����� ���� -1 �������
		for (UINT i = 0; i < stackCount - 1; i++) {

			for (UINT j = 0; j < sliceCount; j++) {
				// ������
				if (i < stackCount - 2)
				{
					lines[index++] = vertices[baseIndex + i * ringVertexCount + j];
					lines[index++] = vertices[baseIndex + (i + 1) * ringVertexCount + j];
				}

				// ������
				lines[index++] = vertices[baseIndex + i * ringVertexCount + j];
				lines[index++] = vertices[baseIndex + i * ringVertexCount + (j + 1)];
			}
		}

		// �Ʒ���
		UINT southPoleIndex = vertexCount - 1;
		baseIndex = southPoleIndex - ringVertexCount;
		for (UINT i = 0; i < sliceCount; i++) {
			lines[index++] = vertices[southPoleIndex];
			lines[index++] = vertices[baseIndex + i];
		}
	}

	this->vertexCount = lineCount * 2;
}

void DebugDrawSphere::UpdateBuffer()
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
