#include "stdafx.h"
#include "Billboard.h"

#include "Utilities/BinaryFile.h"

Billboard::Billboard()
{
	shader = new Shader(Shaders + L"Homework/Billboard.fx");

	texturesPath.push_back(Textures + L"Billboard/tree_beech_a_tex.png");
	texturesPath.push_back(Textures + L"Billboard/tree_beech_b_tex.png");
	texturesPath.push_back(Textures + L"Billboard/tree_fir_a_tex.png");
	texturesPath.push_back(Textures + L"Billboard/tree_fir_b_tex.png");
	texturesPath.push_back(Textures + L"Billboard/tree_oak_a_tex.png");
	texturesPath.push_back(Textures + L"Billboard/tree_oak_b_tex.png");

	for (int i = 0; i < texturesPath.size(); i++)
		textures.push_back(new Texture(texturesPath[i]));

	textureArray = new TextureArray(texturesPath);
	shader->AsShaderResource("Maps")->SetResource(textureArray->GetSRV());

	vector<wstring> normalMaps;
	normalMaps.push_back(Textures + L"Billboard/tree_beech_a_nor.png");
	normalMaps.push_back(Textures + L"Billboard/tree_beech_b_nor.png");
	normalMaps.push_back(Textures + L"Billboard/tree_fir_a_nor.png");
	normalMaps.push_back(Textures + L"Billboard/tree_fir_b_nor.png");
	normalMaps.push_back(Textures + L"Billboard/tree_oak_a_nor.png");
	normalMaps.push_back(Textures + L"Billboard/tree_oak_b_nor.png");

	normalMapArray = new TextureArray(normalMaps);
	shader->AsShaderResource("NormalMaps")->SetResource(normalMapArray->GetSRV());

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

	maxInstanceCount = 40000;
	instanceCount = 0;

	CreateInstance();

	bRandom = true;
	scale = D3DXVECTOR2(1, 1);
	texId = 0;
	factor = 1;
}

Billboard::~Billboard()
{
	SAFE_RELEASE(vertexBuffer[0]);
	SAFE_RELEASE(vertexBuffer[1]);

	for (Texture* texture : textures)
		SAFE_DELETE(texture);

	SAFE_DELETE(textureArray);
	SAFE_DELETE(normalMapArray);
	SAFE_DELETE(shader);
}

void Billboard::ImGuiRender(bool bStart, bool bEnd)
{
	if(bStart)
		ImGui::Begin("Billboard");

	ImGui::Checkbox("Random", &bRandom);
	ImGui::SameLine(100);
	if (ImGui::SmallButton("Clear")) Clear();

	ImGui::DragFloat2("Size", (float*)&scale, 0.1f);

	if (ImGui::SliderInt("Id", &texId, 0, textures.size() - 1))
		bRandom = false;

	ImGui::Image(textures[texId]->SRV(), ImVec2(50, 50));

	if(bEnd)
		ImGui::End();
}

void Billboard::PreRender(Shader * shader)
{
	//shader->AsShaderResource("Maps")->SetResource(textureArray->GetSRV());

	UINT stride[2] = { sizeof(Vertex), sizeof(VertexWorld) };
	UINT offset[2] = { 0, 0 };

	D3D::GetDC()->IASetVertexBuffers(0, 2, vertexBuffer, stride, offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	shader->DrawInstanced(1, 0, 1, instanceCount);
}

void Billboard::Render()
{
	UINT stride[2] = { sizeof(Vertex), sizeof(VertexWorld) };
	UINT offset[2] = { 0, 0 };

	D3D::GetDC()->IASetVertexBuffers(0, 2, vertexBuffer, stride, offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	//shader->DrawInstanced(0, 0, 1, instanceCount);
	shader->DrawInstanced(0, 1, 1, instanceCount);
}

void Billboard::AddInstance(D3DXVECTOR3 & position)
{
	assert(instanceCount + 1 < maxInstanceCount);

	VertexWorld temp;
	temp.textureId = bRandom ? Math::Random(0, textures.size() - 1) : texId;

	position.y += scale.y * 0.5f;

	D3DXMATRIX S, T;
	D3DXMatrixScaling(&S, scale.x, scale.y, 1);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	temp.World = S * T;

	vertices[instanceCount++] = temp;

	D3D::GetDC()->UpdateSubresource(vertexBuffer[1], 0, NULL, &vertices[0], sizeof(VertexWorld) * instanceCount, 0);
}

void Billboard::AddInstance(vector<D3DXVECTOR3>& vec)
{
	int start = instanceCount;

	assert(instanceCount + factor < maxInstanceCount);

	for (int i = 0; i < factor; i++)
	{
		VertexWorld temp;
		temp.textureId = bRandom ? Math::Random(0, textures.size() - 1) : texId;

		D3DXVECTOR3 position = vec[i];
		position.y += scale.y * 0.5f;

		D3DXMATRIX S, T;
		D3DXMatrixScaling(&S, scale.x, scale.y, 1);
		D3DXMatrixTranslation(&T, position.x, position.y, position.z);

		temp.World = S * T;

		vertices[instanceCount++] = temp;
	}

	D3D::GetDC()->UpdateSubresource(vertexBuffer[1], 0, NULL, &vertices[0], sizeof(VertexWorld) * instanceCount, 0);
}

void Billboard::Save(wstring file)
{
	BinaryWriter* w = new BinaryWriter();

	w->Open(file);

	w->Int(instanceCount);

	w->Byte(&vertices[0], sizeof(VertexWorld) * instanceCount);

	w->Close();

	SAFE_DELETE(w);
}

void Billboard::Load(wstring file)
{
	BinaryReader* r = new BinaryReader();
	r->Open(file);

	instanceCount = r->Int();
	
	VertexWorld* temp = new VertexWorld[instanceCount];
	r->Byte((void**)&temp, sizeof(VertexWorld) * instanceCount);

	r->Close();
	SAFE_DELETE(r);

	vertices.assign(temp, temp + instanceCount);
	D3D::GetDC()->UpdateSubresource(vertexBuffer[1], 0, NULL, &vertices[0], sizeof(VertexWorld) * instanceCount, 0);

	SAFE_DELETE_ARRAY(temp);
}

void Billboard::Clear()
{
	instanceCount = 0;
	vertices.clear();
}

void Billboard::CreateInstance()
{
	vertices.resize(maxInstanceCount);

	//Create Instance Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexWorld) * maxInstanceCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &vertices[0];

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer[1]);
		assert(SUCCEEDED(hr));
	}
}
