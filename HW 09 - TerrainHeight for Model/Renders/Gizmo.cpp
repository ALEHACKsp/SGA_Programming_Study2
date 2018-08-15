#include "stdafx.h"
#include "Gizmo.h"

Gizmo::Gizmo(float length)
	:length(length)
{
	lineCount = 3;
}

Gizmo::~Gizmo()
{
}

void Gizmo::Render(D3DXVECTOR3& startPos,
	D3DXVECTOR3& forward, D3DXVECTOR3& up, D3DXVECTOR3& right)
{
	// Create VertexData
	{
		// ���� vertex delete
		SAFE_DELETE_ARRAY(vertices);

		vertexCount = lineCount * 2;

		vertices = new VertexType[vertexCount];

		//for (UINT i = 0; i < vertexCount; i++) {
		//	vertices[i].Position = positions[i];
		//	vertices[i].Color = D3DXCOLOR(1, 1, 1, 1);
		//}
		
		int index = 0;
		// x�� red
		vertices[index].Position = startPos;
		vertices[index++].Color = D3DXCOLOR(1, 0, 0, 1);
		vertices[index].Position = startPos + right * length;
		vertices[index++].Color = D3DXCOLOR(1, 0, 0, 1);

		// y�� green
		vertices[index].Position = startPos;
		vertices[index++].Color = D3DXCOLOR(0, 1, 0, 1);
		vertices[index].Position = startPos + up * length;
		vertices[index++].Color = D3DXCOLOR(0, 1, 0, 1);

		// z�� blue
		vertices[index].Position = startPos;
		vertices[index++].Color = D3DXCOLOR(0, 0, 1, 1);
		vertices[index].Position = startPos + forward * length;
		vertices[index++].Color = D3DXCOLOR(0, 0, 1, 1);
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
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	colorBuffer->SetPSBuffer(0); // �������� ��ȣ
	worldBuffer->SetVSBuffer(1);
	shader->Render();

	// ������ �׸��� ��
	//D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
	D3D::GetDC()->Draw(vertexCount, 0);
}
