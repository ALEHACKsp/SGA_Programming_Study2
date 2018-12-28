#pragma once

class Particles : public GameRender
{
public:
	friend class Particle;
	Particles(ExecuteValues* values, wstring texFileName);
	~Particles();

	void Update();
	void Render();
	void Add(D3DXVECTOR3 & position, D3DXVECTOR3 & scale, D3DXVECTOR3 & mods);

	UINT Count() { return particleCount; }

private:
	void GenerateQuad();
	void CalcVertexBuffer();
	
	void Sort();
	void SortFrustum();

protected:
	ExecuteValues* values;
	D3DXVECTOR2 size = D3DXVECTOR2(1.5f, 1.5f);
	UINT particleCount;
	vector<Particle*> particles;
	vector<D3DXMATRIX> instanceTransformMatrices;

	WorldBuffer* worldBuffer;

	ID3D11Buffer* instanceVertexBuffer, *quadVertexBuffer, *indexBuffer;
	UINT vertCount = 0;

	Texture* texture;
	Shader* shader;

	BlendState* blendState[2];
	DepthStencilState* depthStencilState[2];
	RasterizerState* rasterizerState[2];

	class ParticleBuffer : public ShaderBuffer
	{
	public:
		ParticleBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Color = D3DXCOLOR(1, 1, 1, 1);
		}

		struct Struct
		{
			D3DXCOLOR Color;
		} Data;
	};
	ParticleBuffer* particleBuffer;

	struct SortStruct
	{
		float Distance;
		Particle* Particle;

		bool operator()(const SortStruct &s1, const SortStruct &s2)
		{
			return s1.Distance < s2.Distance;
		}
	};
};

class Particle : public GameRender
{
public:
	friend class Particles;
	Particle(D3DXVECTOR3& position, D3DXVECTOR3& scale, D3DXVECTOR3& mods);
	Particle(D3DXVECTOR3& position, D3DXVECTOR3& scale);
	~Particle();

	void ParticleMatrix(D3DXMATRIX* world);
	
private:
	D3DXMATRIX aaWorld;

	float rnd;

	D3DXVECTOR3 pMods;
};