#pragma once

class Terrain
{
public:
	Terrain(Material* material, wstring heightMap);
	~Terrain();

	void Update();
	void Render();

	// position의 y값은 안씀 찾을려고하는 물체의 위치값
	float Y(D3DXVECTOR3& position);
	bool Y(OUT D3DXVECTOR3* out, D3DXVECTOR3& position);

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