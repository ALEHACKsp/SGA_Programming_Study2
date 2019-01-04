#include "stdafx.h"
#include "Rain.h"

Rain::Rain(D3DXVECTOR3 & size, UINT count)
	:particleCount(count)
{
	rainMap = new Texture(Textures + L"Rain.png");

	shader = new Shader(Shaders + L"Homework/Rain.fx");
	shader->AsShaderResource("Map")->SetResource(rainMap->SRV());

	D3DXMATRIX W;
	D3DXMatrixIdentity(&W);

	shader->AsMatrix("World")->SetMatrix(W);

	// 초기값 설정
	{
		drawDistance = 1000.0f;
		velocity = D3DXVECTOR3(0, -100, 0);
		color = D3DXCOLOR(1, 1, 1, 1);
		position = D3DXVECTOR3(0, 0, 0);
		this->size = size;

		shader->AsScalar("DrawDistance")->SetFloat(drawDistance);
		shader->AsVector("Velocity")->SetFloatVector(velocity);
		shader->AsVector("Color")->SetFloatVector(color);
		shader->AsVector("Origin")->SetFloatVector(position);
		shader->AsVector("Size")->SetFloatVector(this->size);
	}

	Vertex vertex;
	vertex.Position = D3DXVECTOR3(0, 0, 0);

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(Vertex);
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &vertex;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer[0]);
		assert(SUCCEEDED(hr));
	}

	vertexBuffer[1] = NULL;
	CreateParticle(particleCount);
	countFactor = 1;
}

Rain::~Rain()
{
	SAFE_RELEASE(vertexBuffer[0]);
	SAFE_RELEASE(vertexBuffer[1]);

	SAFE_DELETE(shader);
	SAFE_DELETE(rainMap);
}

void Rain::Update()
{
	if (Keyboard::Get()->Press('Z')) {
		particleCount += countFactor;
		CreateParticle(particleCount);
	}
	if (Keyboard::Get()->Press('X')) {
		if ((int)particleCount - countFactor < 1) particleCount = 1;
		else particleCount -= countFactor;
		CreateParticle(particleCount);
	}
}

void Rain::Render()
{
	ImGui::Begin("Rain");
	if (ImGui::SliderFloat3("Velocity", (float*)&velocity, -200, 200))
		shader->AsVector("Velocity")->SetFloatVector(velocity);
	if (ImGui::ColorEdit3("Color", (float*)&color))
		shader->AsVector("Color")->SetFloatVector(color);
	if (ImGui::DragFloat3("Position", (float*)&position))
		shader->AsVector("Origin")->SetFloatVector(position);
	if (ImGui::DragFloat3("Size", (float*)&size))
		shader->AsVector("Size")->SetFloatVector(size);
	ImGui::SliderInt("CountFactor", &countFactor, 1, 100);
	if (ImGui::DragInt("ParticleCount", (int*)&particleCount))
		CreateParticle(particleCount);

	ImGui::End();

	UINT stride[2] = { sizeof(Vertex), sizeof(VertexWorld) };
	UINT offset[2] = { 0,0 };

	D3D::GetDC()->IASetVertexBuffers(0, 2, vertexBuffer, stride, offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	shader->DrawInstanced(0, 0, 1, particleCount);
}

void Rain::CreateParticle(int count)
{
	particleCount = count;

	SAFE_RELEASE(vertexBuffer[1]);

	int newCount = max(particleCount - vertices.size(), 0);

	// Create World
	{
		for (int i = 0; i < newCount; i++)
		{
			VertexWorld vertexWorld;

			D3DXVECTOR2 scale;
			scale.x = Math::Random(0.1f, 0.4f);
			scale.y = Math::Random(4.0f, 8.0f);

			D3DXVECTOR3 position;
			position.x = Math::Random(-size.x * 0.5f, size.x * 0.5f);
			position.y = Math::Random(-size.y * 0.5f, size.y * 0.5f);
			position.z = Math::Random(-size.z * 0.5f, size.z * 0.5f);

			D3DXMATRIX S, T;
			D3DXMatrixScaling(&S, scale.x, scale.y, 1);
			D3DXMatrixTranslation(&T, position.x, position.y, position.z);

			vertexWorld.World = S * T;

			vertices.push_back(vertexWorld);
		}
	}

	//CreateInstanceBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexWorld) * particleCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &vertices[0];

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer[1]);
		assert(SUCCEEDED(hr));
	}
}

void Rain::Position(D3DXVECTOR3 & val)
{
	position = val;
	shader->AsVector("Origin")->SetFloatVector(position);
}
