#pragma once

#include "ParticleInstance.h"

class ParticleInstancer
{
private:
	// VertexDeclaration
	struct VertexDeclaration
	{
		D3DXMATRIX Element;
	};

public:
	ParticleInstancer();
	~ParticleInstancer();

	void Init();

	void Update();
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

	//BlendState* blendState[2]; // 이거 안씀
	DepthStencilState* depthState[2];

	ID3D11Buffer* instanceBuffer;
	UINT instanceCount;

	Shader* shader;
	Texture* texture;

	VertexDeclaration instanceVertexDeclaration;

// public BaseDeferredObject Object
public:
	map<ParticleInstance*, D3DXMATRIX> instanceTransformMatrices;
	ID3D11Buffer* vertexBuffer;
	UINT vertexCount;
	ID3D11Buffer* indexBuffer;

	map<int, ParticleInstance*> instances;

	WorldBuffer* worldBuffer;

private:

	// shader buffer 만들어야함 인스턴스 용 말고
	class InstancerBuffer : public ShaderBuffer
	{
	public:
		InstancerBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Color = D3DXCOLOR(1, 1, 1, 1);
			Data.Day = 12.0f;
		}
	
		struct Struct
		{
			D3DXCOLOR Color;

			float Day;
			float Padding[3];
		} Data;
	};

	InstancerBuffer* instancerBuffer;
};