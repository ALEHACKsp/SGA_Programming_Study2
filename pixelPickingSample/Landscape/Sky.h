#pragma once

class Sky
{
public:
	Sky(ExecuteValues* values);
	~Sky();

	void Update();

	void PreRender();
	void Render();
	void PostRender();

	void ResizeScreen() {}

private:
	void GenerateSphere();
	void GenerateQuad();
	void GeneratePermTex();

	D3DXVECTOR3 GetDirection();
	D3DXCOLOR GetSunColor(float theta, int turbidity);

	void DrawScatter();
	void DrawGlow();
	void DrawMoon();
	void DrawCloud();

private:
	ExecuteValues * values;

	bool realTime;

	float theta, phi;
	float prevTheta, prevPhi;

	RenderTarget* mieTarget, *rayleighTarget;
	Shader* scatterShader;
	Shader* targetShader;
	Shader* moonShader;
	Shader* cloudShader;

	UINT vertexCount, indexCount;
	//UINT radius, slices, stacks;
	UINT domeCount;

	ID3D11Buffer * vertexBuffer;
	ID3D11Buffer * indexBuffer;

	VertexTexture* quadVertices;
	ID3D11Buffer* quadBuffer;

	Render2D* rayleigh2D;
	Render2D* mie2D;

	WorldBuffer* worldBuffer;

	Texture* starField;

	DepthStencilState* depthStencilState[2];
	SamplerState* skySampler;
	SamplerState* quadSampler;
	
	Texture* glowTex, *moonTex;
	SamplerState* glowSampler, *moonSampler;
	BlendState* blendState[2];
	
	Texture* permTex;
private:
	class ScatterBuffer : public ShaderBuffer
	{
	public:
		ScatterBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.StarIntensity = 0.0f;
		}

		struct Struct
		{
			float StarIntensity;

			float Padding[3];
		} Data;
	};

	ScatterBuffer* scatterBuffer;


	class TargetBuffer : public ShaderBuffer
	{
	public:
		TargetBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.WaveLength = D3DXVECTOR3(0.65f, 0.57f, 0.475f);
			Data.SampleCount = 20;

			Data.InvWaveLength.x = 1.0f / powf(Data.WaveLength.x, 4);
			Data.InvWaveLength.y = 1.0f / powf(Data.WaveLength.y, 4);
			Data.InvWaveLength.z = 1.0f / powf(Data.WaveLength.z, 4);

			Data.WaveLengthMie.x = powf(Data.WaveLength.x, -0.84f);
			Data.WaveLengthMie.y = powf(Data.WaveLength.y, -0.84f);
			Data.WaveLengthMie.z = powf(Data.WaveLength.z, -0.84f);
		}

		struct Struct
		{
			D3DXVECTOR3 WaveLength;
			int SampleCount;
			D3DXVECTOR3 InvWaveLength;
			float Padding2;
			D3DXVECTOR3 WaveLengthMie;
			float Padding3;
		} Data;
	};
	TargetBuffer* targetBuffer;

	class MoonBuffer : public ShaderBuffer
	{
	public:
		MoonBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Alpha = 0.0f;
		}

		struct Struct
		{
			float Alpha;

			float Padding[3];
		} Data;
	};
	MoonBuffer* moonBuffer;

	class CloudBuffer : public ShaderBuffer
	{
	public:
		CloudBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Time = 0.0f;
			Data.NumTiles = 16.0f;
			Data.CloudCover = -0.1f;
			Data.CloudSharpness = 0.25f;
		}

		struct Struct
		{
			float Time;
			float NumTiles;
			float CloudCover;
			float CloudSharpness;
		} Data;
	};

	CloudBuffer* cloudBuffer;
};