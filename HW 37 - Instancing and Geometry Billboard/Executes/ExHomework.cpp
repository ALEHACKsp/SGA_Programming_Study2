#include "stdafx.h"
#include "ExHomework.h"

#include "../Environment/Terrain.h"

ExHomework::ExHomework()
{
	terrain = new Terrain();
	shader = new Shader(Shaders + L"Homework/InstanceBillboard.fx");

	vector<wstring> textures;
	textures.push_back(Textures + L"Grass.png");
	textures.push_back(Textures + L"BlueFlower.png");

	textureArray = new TextureArray(textures);
	shader->AsShaderResource("Maps")->SetResource(textureArray->GetSRV());

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

	instanceCountFactor = 1;
	instanceCount = 10000;
	vertexBuffer[1] = NULL;

	CreateInstance(instanceCount);

	flowerFactor = 0.95f;
	shader->AsScalar("factor")->SetFloat(flowerFactor);
}

ExHomework::~ExHomework()
{
	SAFE_DELETE(terrain);

	SAFE_RELEASE(vertexBuffer[0]);
	SAFE_RELEASE(vertexBuffer[1]);

	SAFE_DELETE(textureArray);
	SAFE_DELETE(shader);
}

void ExHomework::Update()
{
	terrain->Update();

	if (Keyboard::Get()->Press('C')) {
		instanceCount += instanceCountFactor;
		CreateInstance(instanceCount);
	}
	if (Keyboard::Get()->Press('V')) {
		if ((int)instanceCount - instanceCountFactor < 1) instanceCount = 1;
		else instanceCount -= instanceCountFactor;
		CreateInstance(instanceCount);
	}
}

void ExHomework::PreRender()
{

}

void ExHomework::Render()
{
	terrain->Render();

	ImGui::SliderInt("InstanceCountFactor", &instanceCountFactor, 1, 100);
	if (ImGui::DragInt("InstanceCount", &instanceCount))
		CreateInstance(instanceCount);
	if (ImGui::DragFloat("FlowerFactor", &flowerFactor, 0.01f, 0.0f, 1.0f, "%.3f"))
		shader->AsScalar("factor")->SetFloat(flowerFactor);

	UINT stride[2] = { sizeof(Vertex), sizeof(VertexWorld) };
	UINT offset[2] = { 0, 0 };

	D3D::GetDC()->IASetVertexBuffers(0, 2, vertexBuffer, stride, offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	shader->DrawInstanced(0, 0, 1, instanceCount);
}

void ExHomework::PostRender()
{

}

void ExHomework::CreateInstance(int count)
{
	instanceCount = count;
	shader->AsScalar("instanceCount")->SetInt(instanceCount);

	SAFE_RELEASE(vertexBuffer[1]);

	int newCount = max(instanceCount - vertices.size(), 0);
	
	for (int i = 0; i < newCount; i++)
	{
		VertexWorld vertexWorld;

		D3DXVECTOR2 scale;
		scale.x = Math::Random(1.0f, 3.0f);
		scale.y = Math::Random(1.0f, 3.0f);

		D3DXVECTOR3 position;
		position.x = Math::Random(0.0f, 255.0f);
		position.z = Math::Random(0.0f, 255.0f);
		position.y = terrain->GetHeight(position.x, position.z) + scale.y * 0.5f;

		D3DXMATRIX S, T;
		D3DXMatrixScaling(&S, scale.x, scale.y, 1);
		D3DXMatrixTranslation(&T, position.x, position.y, position.z);

		vertexWorld.World = S * T;

		float moveSpeed = Math::Random(1.0f, 3.0f);

		vertexWorld.MoveSpeed = moveSpeed;

		vertices.push_back(vertexWorld);
	}

	//Create Instance Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexWorld) * instanceCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &vertices[0];

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer[1]);
		assert(SUCCEEDED(hr));
	}
}
