#pragma once

class Render2D
{
public:
	Render2D(wstring shaderFile = L"");
	virtual ~Render2D(); // 이렇게해야 상속시 소멸됨

	void Position(float x, float y);
	void Position(D3DXVECTOR2& vec);

	void Scale(float x, float y);
	void Scale(D3DXVECTOR2& vec);

	D3DXVECTOR2 Scale() { return scale; }

	void Update();
	virtual void Render();

	void SRV(ID3D11ShaderResourceView* srv);

private:
	void UpdateWorld();

private:
	Shader* shader;

	ID3D11Buffer* vertexBuffer;

	D3DXMATRIX view;
	D3DXMATRIX orthoGraphic; // 직교 투영할꺼

	D3DXVECTOR2 position;
	D3DXVECTOR2 scale;
};