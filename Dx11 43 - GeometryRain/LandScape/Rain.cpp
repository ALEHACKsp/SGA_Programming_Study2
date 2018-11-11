#include "stdafx.h"
#include "Rain.h"

Rain::Rain(ExecuteValues * values, D3DXVECTOR3 & size, UINT count)
	:values(values), particleCount(count)
{
	rainMap = new Texture(Textures + L"Rain.png");

	shader = new Shader(Shaders + L"042_GeometryRain.hlsl");
	shader->CreateGS();

	VertexRain* vertices = new VertexRain[particleCount];

	// Create Data
	{
		for (int i = 0; i < particleCount; i ++)
		{
			D3DXVECTOR2 S;
			S.x = Math::Random(0.1f, 0.4f);
			S.y = Math::Random(4.0f, 8.0f);

			D3DXVECTOR3 P;
			P.x = Math::Random(-size.x * 0.5f, size.x * 0.5f);
			P.y = Math::Random(-size.y * 0.5f, size.y * 0.5f);
			P.z = Math::Random(-size.z * 0.5f, size.z * 0.5f);

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
	SAFE_DELETE_ARRAY(vertices);

	buffer = new Buffer();
	buffer->Data.Size = size;
	buffer->Data.Velocity = D3DXVECTOR3(0, -200, 0);

	blendState[0] = new BlendState();
	blendState[1] = new BlendState();
	blendState[1]->BlendEnable(true);
}

Rain::~Rain()
{
	SAFE_DELETE(blendState[0]);
	SAFE_DELETE(blendState[1]);

	SAFE_DELETE(buffer);

	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE(shader);
	SAFE_DELETE(rainMap);
}

void Rain::Update()
{
	// 1000 °öÇÑ°Ç ÃÊ´ÜÀ§·Î ¹Ù²Û°Å
	buffer->Data.Time = Time::Get()->Running() * 1000.0f / 750.0f;
}

void Rain::Render()
{
	ImGui::SliderFloat3("Velocity", (float*)&buffer->Data.Velocity, -200, 200);
	ImGui::ColorEdit3("Color", (float*)&buffer->Data.Color);
	ImGui::DragFloat3("Position", (float*)&buffer->Data.Position);

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
	D3D::GetDC()->Draw(particleCount, 0);
	blendState[0]->OMSetBlendState();

	D3D::GetDC()->GSSetShader(NULL, NULL, 0);
}
