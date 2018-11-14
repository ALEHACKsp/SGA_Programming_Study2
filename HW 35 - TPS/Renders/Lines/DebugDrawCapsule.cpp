#include "stdafx.h"
#include "DebugDrawCapsule.h"

DebugDrawCapsule::DebugDrawCapsule(float& radius, float height, 
	Axis axis, int stackCount, int sliceCount)
{
	name = "Capsule";

	this->radius = radius;
	this->height = height;
	this->stackCount = stackCount;
	this->sliceCount = sliceCount;
	this->axis = axis;
	lines = NULL;

	CreateVertex();

	__super::Init(lines, lineCount);
}

DebugDrawCapsule::~DebugDrawCapsule()
{
	SAFE_DELETE_ARRAY(lines);
}

void DebugDrawCapsule::Render()
{
	__super::Render();
}

void DebugDrawCapsule::PostRender()
{
	__super::PostRender();

	ImGui::ColorEdit4("Color", (float*)colorBuffer->Data.Color);

	if (ImGui::DragFloat("Radius", &radius) ||
		ImGui::DragFloat("Height", &height) ||
		ImGui::InputInt("StackCount", &stackCount) ||
		ImGui::InputInt("SliceCount", &sliceCount))
		Set(radius, height);
}

void DebugDrawCapsule::Height(float & height)
{
	D3DXVECTOR3 scale = Scale();

	switch (axis)
	{
	case DebugDrawCapsule::Axis::Axis_X:
		scale.x = height;
		break;
	case DebugDrawCapsule::Axis::Axis_Y:
		scale.y = height;
		break;
	case DebugDrawCapsule::Axis::Axis_Z:
		scale.z = height;
		break;
	}

	Scale(scale);
}

float DebugDrawCapsule::Height()
{
	float height;

	switch (axis)
	{
	case DebugDrawCapsule::Axis::Axis_X:
		height = Scale().x;
		break;
	case DebugDrawCapsule::Axis::Axis_Y:
		height = Scale().y;
		break;
	case DebugDrawCapsule::Axis::Axis_Z:
		height = Scale().z;
		break;
	}

	return height;
}

void DebugDrawCapsule::Radius(float & radius)
{
	D3DXVECTOR3 scale = Scale();

	switch (axis)
	{
	case DebugDrawCapsule::Axis::Axis_X:
		scale.z = height;
		scale.y = height;
		break;
	case DebugDrawCapsule::Axis::Axis_Y:
		scale.x = height;
		scale.z = height;
		break;
	case DebugDrawCapsule::Axis::Axis_Z:
		scale.x = height;
		scale.y = height;
		break;
	}

	Scale(scale);
}

float DebugDrawCapsule::Radius()
{
	float radius;

	switch (axis)
	{
	case DebugDrawCapsule::Axis::Axis_X:
		radius = Scale().y;
		break;
	case DebugDrawCapsule::Axis::Axis_Y:
		radius = Scale().z;
		break;
	case DebugDrawCapsule::Axis::Axis_Z:
		radius = Scale().x;
		break;
	}

	return radius;
}

void DebugDrawCapsule::Set(float& radius, float& height)
{
	this->radius = radius;
	this->height = height;

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

void DebugDrawCapsule::Set(float& radius, float& height, D3DXCOLOR & color)
{
	this->radius = radius;
	this->height = height;

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

void DebugDrawCapsule::CreateVertex()
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

			switch (axis)
			{
				case DebugDrawCapsule::Axis::Axis_X:
				{
					// ���� ���� ������ ó��
					if (i == 0 || i == stackCount) {
						vertices[index] = D3DXVECTOR3(i == 0 ?
							radius + 0.5f * height : height * -0.5f
							- radius, 0, 0);

						index++;
					}
					else if (i <= stackCount / 2) {
						float phi = i * phiStep;
						for (UINT j = 0; j <= sliceCount; j++) {
							float theta = j * thetaStep;
							vertices[index] = D3DXVECTOR3(
								(radius * cosf(phi) + 0.5f * height),
								(radius * sinf(phi) * cosf(theta)),
								(radius * sinf(phi) * sinf(theta)));

							index++;
						}
					}
					else {
						float phi = i * phiStep;
						for (UINT j = 0; j <= sliceCount; j++) {
							float theta = j * thetaStep;
							vertices[index] = D3DXVECTOR3(
								(radius * cosf(phi) - 0.5f * height),
								(radius * sinf(phi) * cosf(theta)),
								(radius * sinf(phi) * sinf(theta)));

							index++;
						}
					}
				}
					break;
				case DebugDrawCapsule::Axis::Axis_Y:
				{
					// ���� ���� ������ ó��
					if (i == 0 || i == stackCount) {
						vertices[index] = D3DXVECTOR3(0, i == 0 ?
							radius + 0.5f * height : height * -0.5f
							- radius, 0);

						index++;
					}
					else if (i <= stackCount / 2) {
						float phi = i * phiStep;
						for (UINT j = 0; j <= sliceCount; j++) {
							float theta = j * thetaStep;
							vertices[index] = D3DXVECTOR3(
								(radius * sinf(phi) * cosf(theta)),
								(radius * cosf(phi) + 0.5f * height),
								(radius * sinf(phi) * sinf(theta)));

							index++;
						}
					}
					else {
						float phi = i * phiStep;
						for (UINT j = 0; j <= sliceCount; j++) {
							float theta = j * thetaStep;
							vertices[index] = D3DXVECTOR3(
								(radius * sinf(phi) * cosf(theta)),
								(radius * cosf(phi) - 0.5f * height),
								(radius * sinf(phi) * sinf(theta)));

							index++;
						}
					}
				}
					break;
				case DebugDrawCapsule::Axis::Axis_Z:
				{
					// ���� ���� ������ ó��
					if (i == 0 || i == stackCount) {
						vertices[index] = D3DXVECTOR3(0, 0, i == 0 ?
							radius + 0.5f * height : height * -0.5f
							- radius);

						index++;
					}
					else if (i <= stackCount / 2) {
						float phi = i * phiStep;
						for (UINT j = 0; j <= sliceCount; j++) {
							float theta = j * thetaStep;
							vertices[index] = D3DXVECTOR3(
								(radius * sinf(phi) * cosf(theta)),
								(radius * sinf(phi) * sinf(theta)),
								(radius * cosf(phi) + 0.5f * height));

							index++;
						}
					}
					else {
						float phi = i * phiStep;
						for (UINT j = 0; j <= sliceCount; j++) {
							float theta = j * thetaStep;
							vertices[index] = D3DXVECTOR3(
								(radius * sinf(phi) * cosf(theta)),
								(radius * sinf(phi) * sinf(theta)),
								(radius * cosf(phi) - 0.5f * height));

							index++;
						}
					}
				}
					break;
			}


		}

		lineCount = (2 + (stackCount - 2) + (stackCount - 1)) * sliceCount;
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

		SAFE_DELETE_ARRAY(vertices);
	}
}

void DebugDrawCapsule::UpdateBuffer()
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
