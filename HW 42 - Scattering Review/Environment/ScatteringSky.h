#pragma once

class ScatteringSky
{
public:
	ScatteringSky();
	~ScatteringSky();

	void Update();
	void PreRender();
	void Render();

private:
	void GenerateSphere();
	void GenerateQuad();
	void GeneratePerlinNoiseTex();

	void UpdateStarIntensity();

	D3DXVECTOR3 GetDirection();

	D3DXVECTOR3 GetSunColor(float theta, int turbidity);

	void RenderGlow();
	void RenderMoon();
	void RenderClouds();

private:
	bool realTime;
	float timeSpeed;

	float theta, phi;
	float prevTheta, prevPhi;

	RenderTarget *mieTarget, *rayLeighTarget;
	Shader *scatterShader, *moonShader, *cloudsShader;

	UINT vertexCount, indexCount;
	UINT radius, slices, stacks;

	ID3D11Buffer *vertexBuffer, *indexBuffer;

	VertexTexture *quadVertices;
	ID3D11Buffer *quadBuffer;

	Render2D *rayleigh2D;
	Render2D *mie2D;
	Render2D *perlinNoise2D;

	Texture* starMap;
	Texture* moonMap;
	Texture* moonglowMap;

	ID3D11Texture2D* perlinNoiseTex;
	ID3D11ShaderResourceView* perlinNoiseSrv;
private:
	// Clouds
	float time;
	float numTiles;
	float cloudCover;
	float cloudSharpness;

	D3DXVECTOR3 sunColor;

	// Moon 
	float alpha;

	// Scatter
	float starIntensity;

	// Target
	D3DXVECTOR3 waveLength;
	int sampleCount;

	D3DXVECTOR3 invWaveLength;
	D3DXVECTOR3 waveLengthMie;
};
