#include "stdafx.h"
#include "Line.h"

Line::Line()
	:vertexCount(0), vertices(NULL), vertexBuffer(NULL)
{
	shader = new Shader(Shaders + L"003_Color.hlsl");
	worldBuffer = new WorldBuffer();

	colorBuffer = new ColorBuffer;
}

Line::~Line()
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

void Line::Render(D3DXVECTOR3 * positions, UINT lineCount, 
	D3D11_PRIMITIVE_TOPOLOGY topology)
{
	// Create VertexData
	{
		// ���� vertex delete
		SAFE_DELETE_ARRAY(vertices);

		if (topology == D3D11_PRIMITIVE_TOPOLOGY_LINELIST)
			vertexCount = lineCount * 2;
		else if (topology == D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP)
			vertexCount = lineCount + 1;

		vertices = new VertexType[vertexCount];

		for (UINT i = 0; i < vertexCount; i++) {
			vertices[i].Position = positions[i];
			vertices[i].Color = D3DXCOLOR(1, 1, 1, 1);
		}
	}

	// CreateVertexBuffer
	{
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

	UINT stride = sizeof(VertexType); // �׸� ũ��
	UINT offset = 0;

	// vertex buffer ������ �� �� ����
	// IA �ٴ� ���� ���߿� �������ֽǲ�
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	//D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// �׸� ��� ����
	D3D::GetDC()->IASetPrimitiveTopology(topology);

	colorBuffer->SetPSBuffer(0); // �������� ��ȣ
	worldBuffer->SetVSBuffer(1);
	shader->Render();

	// ������ �׸��� ��
	//D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
	D3D::GetDC()->Draw(vertexCount, 0);
}
