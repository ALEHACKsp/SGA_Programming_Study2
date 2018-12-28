#include "stdafx.h"
#include "Particles.h"
#include "./Viewer/Frustum.h"

Particles::Particles(ExecuteValues* values, wstring texFileName)
	: values(values), particleCount(0)
	, quadVertexBuffer(NULL), instanceVertexBuffer(NULL), indexBuffer(NULL)
{
	texture = new Texture(texFileName);
	shader = new Shader(Shaders + L"046_Particle.hlsl", "VS", "PS");

	GenerateQuad();

	particleBuffer = new ParticleBuffer();
	worldBuffer = new WorldBuffer();

	blendState[0] = new BlendState();
	blendState[1] = new BlendState();
	blendState[1]->BlendEnable(true);

	depthStencilState[0] = new DepthStencilState();
	depthStencilState[1] = new DepthStencilState();
	depthStencilState[1]->DepthEnable(false);

	rasterizerState[0] = new RasterizerState();
	rasterizerState[1] = new RasterizerState();
	rasterizerState[1]->CullMode(D3D11_CULL_NONE);
}

Particles::~Particles()
{
	SAFE_DELETE(rasterizerState[0]);
	SAFE_DELETE(rasterizerState[1]);
	SAFE_DELETE(depthStencilState[0]);
	SAFE_DELETE(depthStencilState[1]);
	SAFE_DELETE(blendState[0]);
	SAFE_DELETE(blendState[1]);

	SAFE_DELETE(texture);
	SAFE_DELETE(particleBuffer);
	SAFE_DELETE(worldBuffer);
	
	SAFE_RELEASE(quadVertexBuffer);
	SAFE_RELEASE(instanceVertexBuffer);
	SAFE_RELEASE(indexBuffer);
}

void Particles::GenerateQuad()
{
	VertexTexture vertices[6];
	vertices[0].Position = D3DXVECTOR3(0, 0, 0);
	vertices[1].Position = D3DXVECTOR3(0, 0, 0);
	vertices[2].Position = D3DXVECTOR3(0, 0, 0);
	vertices[3].Position = D3DXVECTOR3(0, 0, 0);
	vertices[4].Position = D3DXVECTOR3(0, 0, 0);
	vertices[5].Position = D3DXVECTOR3(0, 0, 0);
	vertices[0].Uv = D3DXVECTOR2(1, 1);
	vertices[1].Uv = D3DXVECTOR2(0, 1);
	vertices[2].Uv = D3DXVECTOR2(0, 0);
	vertices[3].Uv = D3DXVECTOR2(0, 0);
	vertices[4].Uv = D3DXVECTOR2(1, 0);
	vertices[5].Uv = D3DXVECTOR2(1, 1);

	vertCount = 6;

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTexture) * vertCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &quadVertexBuffer);
		assert(SUCCEEDED(hr));
	}
}

void Particles::Update()
{
}

void Particles::Render()
{
	if (Visible() == false || Enable() == false)
		return;
	
	if (particleCount == 0)
		return;

	// If we have more instances than room in our vertex buffer, grow it to the neccessary size.
	if ((instanceVertexBuffer == NULL) || (instanceTransformMatrices.size() != particleCount))
		CalcVertexBuffer();

	worldBuffer->SetMatrix(Transformed());
	worldBuffer->SetVSBuffer(1);
	particleBuffer->SetPSBuffer(10);
	texture->SetShaderResource(10);

	ID3D11Buffer* buffer[2];
	buffer[0] = quadVertexBuffer;
	buffer[1] = instanceVertexBuffer;
	UINT stride[2];
	UINT offset[2] = { 0 };
	stride[0] = sizeof(VertexTexture);
	stride[1] = sizeof(D3DXMATRIX);

	D3D::GetDC()->IASetVertexBuffers(0, 2, buffer, stride, offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	rasterizerState[1]->RSSetState();
	blendState[1]->OMSetBlendState();
	depthStencilState[1]->OMSetDepthStencilState();
	shader->Render();
	D3D::GetDC()->DrawInstanced(vertCount, particleCount, 0, 0);
	depthStencilState[0]->OMSetDepthStencilState();
	blendState[0]->OMSetBlendState();
	rasterizerState[0]->RSSetState();
}

void Particles::Add(D3DXVECTOR3 & position, D3DXVECTOR3 & scale, D3DXVECTOR3 & mods)
{
	Particle* particle = new Particle(position, scale, mods);
	D3DXMATRIX mat;
	particle->ParticleMatrix(&mat);
	instanceTransformMatrices.push_back(mat);
	particles.push_back(particle);
	particleCount++;
}

void Particles::CalcVertexBuffer()
{
	if (instanceVertexBuffer != NULL)
		SAFE_RELEASE(instanceVertexBuffer);

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		// Set up the description of the instance buffer.
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(D3DXMATRIX) * particleCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &instanceTransformMatrices[0];

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &instanceVertexBuffer);
		assert(SUCCEEDED(hr));
	}
	Sort();
}

void Particles::Sort()
{
	//Sort
	vector<SortStruct> bbDists;
	
	for (UINT p = 0; p < particleCount; p++)
	{
		SortStruct temp;
		temp.Particle = particles[p];
		temp.Distance = Math::GetDistance(particles[p]->Position(), values->MainCamera->Position());
		bbDists.push_back(temp);
	}
	sort(bbDists.begin(), bbDists.end(), SortStruct());
	
	for (UINT i = 0; i < particleCount; i++)
	{
		particles[i] = bbDists[i].Particle;
		D3DXMATRIX mat;
		particles[i]->ParticleMatrix(&mat);
		instanceTransformMatrices[i] = mat;
	}

	//update buffer
	{
		D3D11_MAPPED_SUBRESOURCE subResource;
	
		HRESULT hr = D3D::GetDC()->Map(instanceVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		assert(SUCCEEDED(hr));
		memcpy(subResource.pData, &instanceTransformMatrices[0], sizeof(D3DXMATRIX) * particleCount);
		D3D::GetDC()->Unmap(instanceVertexBuffer, 0);
	}
}

Particle::Particle(D3DXVECTOR3 & position, D3DXVECTOR3 & scale, D3DXVECTOR3 & mods)
	: pMods(mods)
{
	Position(position);
	Scale(scale);
	Transformed();
}

Particle::Particle(D3DXVECTOR3 & position, D3DXVECTOR3 & scale)
{
	Position(position);
	Scale(scale);
	Transformed();
}

Particle::~Particle()
{
}

void Particle::ParticleMatrix(D3DXMATRIX * world)
{
	*world = Transformed();
	// Set the scale
	(*world)._13 = Scale().x;
	(*world)._24 = Scale().y;
	
	// Set the image, alpha and color mod
	(*world)._12 = pMods.x;
	(*world)._23 = pMods.y;
	(*world)._34 = pMods.z;
}
