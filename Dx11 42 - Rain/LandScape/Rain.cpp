#include "stdafx.h"
#include "Rain.h"

Rain::Rain(ExecuteValues * values, D3DXVECTOR3 & size, UINT count)
	:values(values), particleCount(count)
{
	rainMap = new Texture(Textures + L"Rain.png");

	shader = new Shader(Shaders + L"048_Rain.hlsl");
	worldBuffer = new WorldBuffer();

	vertexCount = particleCount * 4;
	vertices = new VertexRain[vertexCount];

	indexCount = particleCount * 6;
	indices = new UINT[indexCount];

	// Create Data
	{
		for (int i = 0; i < particleCount * 4; i += 4)
		{
			D3DXVECTOR2 S;
			S.x = Math::Random(0.1f, 0.4f);
			S.y = Math::Random(4.0f, 8.0f);

			D3DXVECTOR3 P;
			P.x = Math::Random(-size.x, size.x);
			P.y = Math::Random(-size.y, size.y);
			P.z = Math::Random(-size.z, size.z);

			vertices[i + 0] = VertexRain(P, D3DXVECTOR2(0, 1), S);
			vertices[i + 1] = VertexRain(P, D3DXVECTOR2(0, 0), S);
			vertices[i + 2] = VertexRain(P, D3DXVECTOR2(1, 1), S);
			vertices[i + 3] = VertexRain(P, D3DXVECTOR2(1, 0), S);
		}

		for (int i = 0; i < particleCount; i++)
		{
			indices[i * 6 + 0] = i * 4 + 0;
			indices[i * 6 + 1] = i * 4 + 1;
			indices[i * 6 + 2] = i * 4 + 2;
			indices[i * 6 + 3] = i * 4 + 2;
			indices[i * 6 + 4] = i * 4 + 1;
			indices[i * 6 + 5] = i * 4 + 3;
		}
	}

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexRain) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//CreateIndexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = indices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}

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

	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE(worldBuffer);
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
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	rainMap->SetShaderResource(10);
	rainMap->SetShaderSampler(10);

	shader->Render();
	worldBuffer->SetVSBuffer(1);

	blendState[1]->OMSetBlendState();
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
	blendState[0]->OMSetBlendState();
}
