#include "stdafx.h"
#include "Rain.h"

Rain::Rain(ExecuteValues * values, D3DXVECTOR3 & size, UINT count)
	:values(values), particleCount(count)
{
	rainMap = new Texture(Textures + L"Rain.png");
	shader = new Shader(Shaders + L"050_GeometryRain.hlsl");
	shader->CreateGS();

	VertexRain* vertices = new VertexRain[particleCount];

	//CreateVertex
	{
		for (UINT i = 0; i < particleCount; ++i) {
			D3DXVECTOR2 S;   //size
			S.x = Math::Random(0.1f, 0.4f);
			S.y = Math::Random(4.0f, 8.0f);

			D3DXVECTOR3 P; //position;
			P.x = Math::Random(-size.x, size.x);
			P.y = Math::Random(-size.y, size.y);
			P.z = Math::Random(-size.z, size.z);

			vertices[i] = VertexRain(P, S);
		}
	}

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexRain) * particleCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}
	SAFE_DELETE(vertices);

	buffer = new Buffer();
	buffer->Data.Size = size;
	buffer->Data.Velocity = D3DXVECTOR3(0, -200, 0);

	blendState[0] = new BlendState();
	blendState[1] = new BlendState();
	blendState[1]->BlendEnable(true);
}

Rain::~Rain()
{
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE(shader);
	SAFE_DELETE(rainMap);
}

void Rain::Update()
{
	buffer->Data.Time = Time::Get()->Running() * 1000.0f / 750.0f;
}

void Rain::Render()
{
	ImGui::SliderFloat3("Velocity", (float*)buffer->Data.Velocity, -200, 200);
	ImGui::SliderFloat3("Position", (float*)buffer->Data.Position, 0, 100);
	ImGui::ColorEdit3("Color", buffer->Data.Color);

	buffer->SetVSBuffer(10);

	UINT stride = sizeof(VertexRain);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	rainMap->SetShaderResource(10);
	rainMap->SetShaderSampler(10);

	shader->Render();
	values->ViewProjection->SetGSBuffer(0);

	blendState[1]->OMSetBlendState();
	{
		D3D::GetDC()->Draw(particleCount, 0);
	}
	blendState[0]->OMSetBlendState();

}
