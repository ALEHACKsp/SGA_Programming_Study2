#include "stdafx.h"
#include "TessTriangle.h"

TessTriangle::TessTriangle()
{
	shader = new Shader(Shaders + L"059_TessTriangle.fx");

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

TessTriangle::~TessTriangle()
{
	SAFE_DELETE(shader);
	SAFE_RELEASE(vertexBuffer);
}

void TessTriangle::Update()
{
	D3DXMATRIX W;
	D3DXMatrixIdentity(&W);

	shader->AsMatrix("World")->SetMatrix(W);
}

void TessTriangle::PreRender()
{

}

void TessTriangle::Render()
{
	static int amount = 1;
	static int amountInside = 1;

	ImGui::SliderInt("Amount", &amount, 1, 10);
	ImGui::SliderInt("AmountInside", &amountInside, 1, 10);

	shader->AsScalar("TsAmount")->SetInt(amount);
	shader->AsScalar("TsAmountInside")->SetInt(amountInside);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	// HS 쓰려면 이걸 써줘야함
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	shader->Draw(0, 0, 3);
}

void TessTriangle::PostRender()
{

}