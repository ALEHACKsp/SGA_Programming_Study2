#include "stdafx.h"
#include "TessTriangleDistance.h"

TessTriangleDistance::TessTriangleDistance()
{
	shader = new Shader(Shaders + L"059_TessTriangleDistance.fx");

	Vertex vertices[3];
	vertices[0].Position = D3DXVECTOR3(-1, -1, 0);
	vertices[1].Position = D3DXVECTOR3( 0,  1, 0);
	vertices[2].Position = D3DXVECTOR3( 1, -1, 0);

	// Create VertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(Vertex) * 3;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}
}

TessTriangleDistance::~TessTriangleDistance()
{
	SAFE_DELETE(shader);
	SAFE_RELEASE(vertexBuffer);
}

void TessTriangleDistance::Update()
{
	D3DXMATRIX W;
	//D3DXMatrixIdentity(&W);
	D3DXMatrixScaling(&W, 10, 10, 1);

	shader->AsMatrix("World")->SetMatrix(W);
}

void TessTriangleDistance::PreRender()
{

}

void TessTriangleDistance::Render()
{
	static float ratio = 20;
	static int piece = 1;

	ImGui::SliderFloat("Ratio", &ratio, 1, 100);
	shader->AsScalar("Ratio")->SetFloat(ratio);

	ImGui::SliderInt("Piece", &piece, 1, 80);
	shader->AsScalar("Piece")->SetInt(piece);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	// HS 쓰려면 이걸 써줘야함
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	shader->Draw(0, 0, 3);
}

void TessTriangleDistance::PostRender()
{

}