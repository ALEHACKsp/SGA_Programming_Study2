#pragma once

class OldRain
{
private:
	struct VertexRain;

public:
	OldRain(D3DXVECTOR3& size, UINT count); // 몇개 만들wl
	~OldRain();

	void Update();
	void Render();

	void Position(D3DXVECTOR3& val);

private:
	Shader* shader;

	ID3D11Buffer* vertexBuffer;
	VertexRain* vertices;
	
	ID3D11Buffer* indexBuffer;
	UINT* indices;

	UINT vertexCount;
	UINT indexCount;
	UINT particleCount;

	Texture* rainMap;

	float drawDistance;
	D3DXVECTOR3 velocity;
	D3DXCOLOR color;
	D3DXVECTOR3 position;
	D3DXVECTOR3 size;
private:
	struct VertexRain
	{
		VertexRain()
		{
			Position = D3DXVECTOR3(0, 0, 0);
			Uv = D3DXVECTOR2(0, 0);
			Scale = D3DXVECTOR2(0, 0);
		}

		VertexRain(D3DXVECTOR3& position, D3DXVECTOR2& uv, D3DXVECTOR2 scale)
		{
			Position = position;
			Uv = uv;
			Scale = scale;
		}

		D3DXVECTOR3 Position;
		D3DXVECTOR2 Uv;
		D3DXVECTOR2 Scale;
	};
};