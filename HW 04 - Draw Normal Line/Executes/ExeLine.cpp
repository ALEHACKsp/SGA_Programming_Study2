#include "stdafx.h"
#include "ExeLine.h"

ExeLine::ExeLine(ExecuteValues* values,
	D3DXVECTOR3 * startPos, D3DXVECTOR3 * endPos,
	UINT lineCount)
	:Execute(values),
	vertexCount(lineCount * 2)
{
	shader = new Shader(Shaders + L"003_Color.hlsl");
	worldBuffer = new WorldBuffer();

	colorBuffer = new ColorBuffer;

	// Create VertexData
	{
		vertices = new VertexType[vertexCount];

		for (UINT i = 0; i < vertexCount; i++) {
			if (i % 2 == 0) {
				vertices[i].Position = startPos[i / 2];
			}
			else {
				vertices[i].Position = endPos[i / 2];
			}
			vertices[i].Color = D3DXCOLOR(1, 1, 1, 1);
		}
	}

	// Create IndexData
	{

	}

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

	// CreateIndexBuffer
	{
		//D3D11_BUFFER_DESC desc = { 0 };
		//desc.Usage = D3D11_USAGE_DEFAULT; // ��� ��������� ���� ����
		//desc.ByteWidth = sizeof(UINT) * indexCount; // ���� ���ۿ� �� �������� ũ��
		//desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		//D3D11_SUBRESOURCE_DATA  data = { 0 }; // �긦 ���ؼ� ���� �� lock ���
		//data.pSysMem = indices; // �� �������� �ּ�

		//HRESULT hr = D3D::GetDevice()->CreateBuffer(
		//	&desc, &data, &indexBuffer);
		//assert(SUCCEEDED(hr)); // �����Ǹ� hr 0���� ū �� �Ѿ��
	}
}

ExeLine::~ExeLine()
{
	// dx�κ��� ��������� release
	// ���� �������� ��������
	//SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	//SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(vertices);

	SAFE_DELETE(colorBuffer);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);
}

void ExeLine::Update()
{

}

void ExeLine::PreRender()
{
}

void ExeLine::Render()
{
	UINT stride = sizeof(VertexType); // �׸� ũ��
	UINT offset = 0;

	// vertex buffer ������ �� �� ����
	// IA �ٴ� ���� ���߿� �������ֽǲ�
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	//D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// �׸� ��� ����
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	colorBuffer->SetPSBuffer(0); // �������� ��ȣ
	worldBuffer->SetVSBuffer(1);
	shader->Render();

	// ������ �׸��� ��
	//D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
	D3D::GetDC()->Draw(vertexCount, 0);
}

void ExeLine::PostRender()
{

}

void ExeLine::ResizeScreen()
{
}

void ExeLine::ResizeLength(D3DXVECTOR3 * endPos)
{
	for (UINT i = 0; i < vertexCount; i++) {
		// ������ �н�
		if (i % 2 == 0) continue;
		// ���� ����
		vertices[i].Position = endPos[i / 2];
	}

	// ���� vertexBuffer release
	SAFE_RELEASE(vertexBuffer);

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
