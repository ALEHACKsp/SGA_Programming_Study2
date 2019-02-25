#pragma once

class Rain
{
private:
	struct VertexRain;

public:
	Rain(D3DXVECTOR3& size, UINT count); // � ������
	~Rain();

	void Initialize();
	void Ready();
	void Update();
	void Render();

	void CreateParticle(int count);

	void Position(D3DXVECTOR3& val);

private:
	Shader* shader;

	ID3D11Buffer* vertexBuffer[2];

	UINT particleCount;
	int countFactor;

	Texture* rainMap;

	float drawDistance;
	D3DXVECTOR3 velocity;
	D3DXCOLOR color;
	D3DXVECTOR3 position;
	D3DXVECTOR3 size;
private:
	struct VertexWorld
	{
		D3DXMATRIX World;
	};
	vector<VertexWorld> vertices;
};