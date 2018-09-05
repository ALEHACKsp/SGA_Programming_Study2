#pragma once

class Render2D
{
public:
	Render2D(struct ExecuteValues* values);
	~Render2D();

	void Position(float x, float y);
	void Position(D3DXVECTOR2& vec);

	void Scale(float x, float y);
	void Scale(D3DXVECTOR2& vec);

	void Update();
	void Render();

	void SRV(ID3D11ShaderResourceView* srv) { this->srv = srv; }

private:
	struct ExecuteValues* values;

	Shader* shader;
	ID3D11ShaderResourceView* srv;

	WorldBuffer* worldBuffer;

	ID3D11Buffer* vertexBuffer;
	DepthStencilState* depthState[2];

	D3DXMATRIX orthoGraphic; // 직교 투영할꺼

	D3DXVECTOR2 position;
	D3DXVECTOR2 size;
};