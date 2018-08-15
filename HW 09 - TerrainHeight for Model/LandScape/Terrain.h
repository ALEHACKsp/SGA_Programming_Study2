#pragma once

class Terrain
{
public:
	Terrain(Material* material, wstring heightMap);
	~Terrain();

	void Update();
	void Render();

	void PostRender();

	void UpdateModel(class GameModel* model);

	// position�� y���� �Ⱦ� ã�������ϴ� ��ü�� ��ġ��
	float Y(D3DXVECTOR3& position);
	bool Y(OUT D3DXVECTOR3* out, D3DXVECTOR3& position);

	bool Y(OUT D3DXVECTOR3* outPos, OUT D3DXVECTOR3* outForward, OUT D3DXVECTOR3 * outUp, 
		D3DXVECTOR3& position, D3DXVECTOR3& right);

private:
	int fillModeNumber;
	ID3D11RasterizerState* fillMode[2];

private:
	void CreateData();
	void CreateBuffer();

private:
	Material* material;
	WorldBuffer* worldBuffer;

	Texture* heightTexture;
	
	UINT width, height;

	vector<VertexTextureNormal> vertices;
	vector<UINT> indices;

	ID3D11Buffer* vertexBuffer, *indexBuffer;
};