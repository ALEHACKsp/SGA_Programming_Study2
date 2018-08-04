#include "stdafx.h"
#include "ExeSphere.h"

ExeSphere::ExeSphere(ExecuteValues * values)
	:Execute(values)
	, radius(5.0f), stackCount(20), sliceCount(20)
{
	shader = new Shader(Shaders + L"003_Color.hlsl");
	worldBuffer = new WorldBuffer();

	colorBuffer = new ColorBuffer;

	float phiStep = D3DX_PI / stackCount;
	float thetaStep = 2.0f * D3DX_PI / sliceCount;

	// Create VertexData
	{
		vertexCount = stackCount * sliceCount + 1;
		vertices = new VertexColor[vertexCount];

		UINT index = 0;
		vertices[index].Position = D3DXVECTOR3(0, radius, 0);
		vertices[index].Color = D3DXCOLOR(1, 1, 1, 1);
		index++;

		for (UINT i = 1; i <= stackCount - 1; i++) {
			float phi = i * phiStep;
			for (UINT j = 0; j <= sliceCount; j++) {
				float theta = j * thetaStep;
				vertices[index].Position = D3DXVECTOR3(
					(radius * sinf(phi) * cosf(theta)),
					(radius * cosf(phi)),
					(radius * sinf(phi) * sinf(theta)));

				vertices[index].Color = D3DXCOLOR(1, 1, 1, 1);
				index++;
			}
		}

		vertices[index].Position = D3DXVECTOR3(0, -radius, 0);
		vertices[index].Color = D3DXCOLOR(1, 1, 1, 1);
		index++;
	}

	// Create IndexData
	{
		indexCount = 
			sliceCount * 3 + 
			(stackCount - 1) * sliceCount * 6 + 
			sliceCount * 3;
		indices = new UINT[indexCount];

		UINT index = 0;
		for (UINT i = 1; i <= sliceCount; i++) {
			indices[index++] = 0;
			indices[index++] = i + 1;
			indices[index++] = i;
		}

		UINT baseIndex = 1;
		UINT ringVertexCount = sliceCount + 1;
		for (UINT i = 0; i < stackCount - 2; i++) {
			for (UINT j = 0; j < sliceCount; j++) {
				indices[index++] = baseIndex + i * ringVertexCount + j;
				indices[index++] = baseIndex + i * ringVertexCount + j + 1;
				indices[index++] = baseIndex + (i + 1) * ringVertexCount + j;

				indices[index++] = baseIndex + (i + 1) * ringVertexCount + j;
				indices[index++] = baseIndex + i * ringVertexCount + j + 1;
				indices[index++] = baseIndex + (i + 1) * ringVertexCount + j + 1;
			}
		}

		UINT southPoleIndex = vertexCount - 1;
		baseIndex = southPoleIndex - ringVertexCount;
		for (UINT i = 0; i < sliceCount; i++) {
			indices[index++] = southPoleIndex;
			indices[index++] = baseIndex + i;
			indices[index++] = baseIndex + i + 1;
		}
	}

	// CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT; // ��� ��������� ���� ����
		desc.ByteWidth = sizeof(VertexColor) * vertexCount; // ���� ���ۿ� �� �������� ũ��
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA  data = { 0 }; // �긦 ���ؼ� ���� �� lock ���
		data.pSysMem = vertices; // �� �������� �ּ�

		HRESULT hr = D3D::GetDevice()->CreateBuffer(
			&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr)); // �����Ǹ� hr 0���� ū �� �Ѿ��
	}

	// CreateIndexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT; // ��� ��������� ���� ����
		desc.ByteWidth = sizeof(UINT) * indexCount; // ���� ���ۿ� �� �������� ũ��
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA  data = { 0 }; // �긦 ���ؼ� ���� �� lock ���
		data.pSysMem = indices; // �� �������� �ּ�

		HRESULT hr = D3D::GetDevice()->CreateBuffer(
			&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr)); // �����Ǹ� hr 0���� ū �� �Ѿ��
	}

	{
		fillModeNumber = 0;

		D3D11_RASTERIZER_DESC desc;
		States::GetRasterizerDesc(&desc);

		States::CreateRasterizer(&desc, &fillMode[0]);

		desc.FillMode = D3D11_FILL_WIREFRAME;
		States::CreateRasterizer(&desc, &fillMode[1]);
	}
}

ExeSphere::~ExeSphere()
{
	// dx�κ��� ��������� release
	// ���� �������� ��������
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(vertices);

	SAFE_DELETE(colorBuffer);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);

	for (int i = 0; i < 2; i++)
		SAFE_RELEASE(fillMode[i]);
}

void ExeSphere::Update()
{

}

void ExeSphere::PreRender()
{
}

void ExeSphere::Render()
{
	UINT stride = sizeof(VertexColor); // �׸� ũ��
	UINT offset = 0;

	// vertex buffer ������ �� �� ����
	// IA �ٴ� ���� ���߿� �������ֽǲ�
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// �׸� ��� ����
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D::GetDC()->RSSetState(fillMode[fillModeNumber]);

	colorBuffer->SetPSBuffer(0); // �������� ��ȣ
	worldBuffer->SetVSBuffer(1);
	shader->Render();

	// ������ �׸��� ��
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
}

void ExeSphere::PostRender()
{
	// ImGui begin �ȿ��� Debugâ�� ��
	ImGui::SliderInt("Wireframe", &fillModeNumber, 0, 1);
}

void ExeSphere::ResizeScreen()
{
}
