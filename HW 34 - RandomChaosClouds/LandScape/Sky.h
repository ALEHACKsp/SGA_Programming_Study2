#pragma once
//   MultiTexture : 두장의 이미지가 동시에 들어갈 때

class Sky
{
public:
	Sky(ExecuteValues* values);
	~Sky();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
	void GenerateSphere();
	void GenerateQuad();
	void GeneratePerlinNoiseTex();

	float GetStarIntensity();

	D3DXVECTOR3 GetDirection();

	D3DXVECTOR3 GetSunColor(float theta, int turbidity);

	void RenderGlow();
	void RenderMoon();
	void RenderClouds();

private:
	ExecuteValues *values;
	bool realTime;
	float timeSpeed;

	float theta, phi;
	float prevTheta, prevPhi;

	RenderTarget *mieTarget, *rayLeighTarget;
	Shader *scatterShader, *targetShader, *moonShader, *cloudsShader;

	UINT vertexCount, indexCount;
	UINT radius, slices, stacks;

	ID3D11Buffer *vertexBuffer, *indexBuffer;

	VertexTexture *quadVertices;
	ID3D11Buffer *quadBuffer;

	Render2D *rayleigh2D;
	Render2D *mie2D;
	Render2D *perlinNoise2D;

	WorldBuffer * worldBuffer;

	BlendState* blendState[3];
	DepthStencilState * depthState[2];

	Texture* starMap;
	Texture* moonMap;
	Texture* moonglowMap;

	SamplerState* cloudsSampler;
	SamplerState* starSampler;
	SamplerState* moonSampler;
	SamplerState* moonglowSampler;
	SamplerState* rayleighSampler;
	SamplerState* mieSampler;

	ID3D11Texture2D* perlinNoiseTex;
	ID3D11ShaderResourceView* perlinNoiseSrv;
private:
	class CloudsBuffer : public ShaderBuffer
	{
	public:
		CloudsBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.CloudCover = -0.1f;
			Data.CloudSharpness = 0.5f;
			Data.NumTiles = 16.0f;
		}

		struct Struct
		{
			float Time;
			float NumTiles;
			float CloudCover;
			float CloudSharpness;

			D3DXVECTOR3 SunColor;
			float Padding;

		} Data;
	};

	CloudsBuffer * cloudsBuffer;

	class MoonBuffer : public ShaderBuffer
	{
	public:
		MoonBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Alpha = 1.0f;
		}

		struct Struct
		{
			float Alpha;
			float Padding[3];

		} Data;
	};

	MoonBuffer * moonBuffer;

	class ScatterBuffer : public ShaderBuffer
	{
	public:
		ScatterBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			//Data.Select = 0;
			Data.StarIntensity = 0.0f;
		}

		struct Struct
		{
			//int Select;
			float StarIntensity;
			float Padding[3];

		} Data;
	};

	ScatterBuffer * scatterBuffer;

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

	TargetBuffer * targetBuffer;
};
