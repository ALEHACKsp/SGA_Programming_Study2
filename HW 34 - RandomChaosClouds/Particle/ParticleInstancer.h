#pragma once

#include "ParticleInstance.h"

class ParticleInstancer
{
private:
	// VertexDeclaration
	struct VertexDeclaration
	{
		int id;
	};

public:
	ParticleInstancer();
	~ParticleInstancer();

	void Init();

	void Render();

	void CalcVertexBuffer();

	void TranslateOO(D3DXVECTOR3 distance);
	void TranslateAA(D3DXVECTOR3 distance);
	void Rotate(D3DXVECTOR3 axis, float angle);
	
private:
	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	D3DXQUATERNION rotation;
	D3DXVECTOR2 size;
	D3DXMATRIX world;

	BlendState* blendState[2];
	DepthStencilState* depthState[2];

	ID3D11Buffer* instanceBuffer;
	UINT instanceCount;

	Shader* shader;

	VertexDeclaration instanceVertexDeclaration;

// public BaseDeferredObject Object
public:
	map<ParticleInstance*, D3DXMATRIX> instanceTransformMatrices;
	ID3D11Buffer* vertexBuffer;
	UINT vertexCount;
	ID3D11Buffer* indexBuffer;

	map<int, ParticleInstance*> instances;

	D3DXCOLOR color;

private:
	class InstanceWorldBuffer : public ShaderBuffer
	{
	public:
		InstanceWorldBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			for (int i = 0; i < 128; i++)
				D3DXMatrixIdentity(&Data.Worlds[i]);
		}
	
	private:
		struct Struct
		{
			D3DXMATRIX Worlds[128];
		} Data;
	};

	InstanceWorldBuffer* instanceWorldBuffer;
};