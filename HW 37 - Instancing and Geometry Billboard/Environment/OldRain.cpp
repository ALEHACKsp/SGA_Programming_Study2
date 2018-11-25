#include "stdafx.h"
#include "OldRain.h"

OldRain::OldRain(D3DXVECTOR3 & size, UINT count)
	:particleCount(count)
{
	rainMap = new Texture(Textures + L"Rain.png");

	shader = new Shader(Shaders + L"Homework/OldRain.fx");
	shader->AsShaderResource("Map")->SetResource(rainMap->SRV());

	// 초기값 설정
	{
		drawDistance = 1000.0f;
		velocity = D3DXVECTOR3(0, -200, 0);
		color = D3DXCOLOR(1, 1, 1, 1);
		position = D3DXVECTOR3(0, 0, 0);
		this->size = size;

		//shader->AsScalar("DrawDistance")->SetFloat(drawDistance);
		//shader->AsVector("Velocity")->SetFloatVector(velocity);
		//shader->AsVector("Color")->SetFloatVector(color);
		//shader->AsVector("Origin")->SetFloatVector(position);
		//shader->AsVector("Size")->SetFloatVector(this->size);
	}

	D3DXMATRIX W;
	D3DXMatrixIdentity(&W);

	shader->AsMatrix("World")->SetMatrix(W);

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
			P.x = Math::Random(-size.x * 0.5f, size.x * 0.5f);
			P.y = Math::Random(-size.y * 0.5f, size.y * 0.5f);
			P.z = Math::Random(-size.z * 0.5f, size.z * 0.5f);

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
}

OldRain::~OldRain()
{
	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE(shader);
	SAFE_DELETE(rainMap);
}

void OldRain::Update()
{

}

void OldRain::Render()
{
	ImGui::Begin("Rain");
	if (ImGui::SliderFloat3("Velocity", (float*)&velocity, -200, 200))
		shader->AsVector("Velocity")->SetFloatVector(velocity);
	if(ImGui::ColorEdit3("Color", (float*)&color))
		shader->AsVector("Color")->SetFloatVector(color);
	if(ImGui::DragFloat3("Position", (float*)&position))
		shader->AsVector("Origin")->SetFloatVector(position);
	if (ImGui::DragFloat3("Size", (float*)&size))
		shader->AsVector("Size")->SetFloatVector(size);
	ImGui::End();

	UINT stride = sizeof(VertexRain);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->DrawIndexed(0, 0, indexCount);
}

void OldRain::Position(D3DXVECTOR3 & val)
{
	position = val;
	shader->AsVector("Origin")->SetFloatVector(position);
}
