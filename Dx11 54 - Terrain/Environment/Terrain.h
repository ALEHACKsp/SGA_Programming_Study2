#pragma once

class Terrain
{
public:
	friend class QuadTree;

public:
	Terrain(Material* material, wstring heightMap, float gridX = 1.0f, float gridY = 1.0f, UINT patchSize = 32);
	~Terrain();

	void Update();
	void Render();

private:
	void CreateData();
	void CreateNormal();
	void CreateBuffer();

private:
	Material* material;
	Texture* heightMap;

	UINT width, height;
	float gridX, gridY; // 한 칸의 크기
	UINT patchSize; // 나눠쓸 때 한 칸을 의미 (최하 분할 갯수) 16주면 16x16으로 자르겟다는거

	VertexTextureNormal* vertices;
	UINT vertexCount;

	ID3D11Buffer* vertexBuffer;

	class QuadTree* root;
	class Frustum* frustum;
};
