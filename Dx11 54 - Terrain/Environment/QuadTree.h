#pragma once

class QuadTree
{
public:
	friend class Terrain;

public:
	QuadTree(class Terrain* terrain, class Frustum* frustum, Shader* shader);
	~QuadTree();

	void Render();

private:
	void Build(D3DXVECTOR2& center, float width);

private:
	Shader* shader;
	class Terrain* terrain;
	QuadTree* nodes[4];

	D3DXVECTOR3 centerPosition;
	float width;
	float bottom;
	float top;

	UINT* indices;
	UINT indexCount;

	ID3D11Buffer* indexBuffer;

	class Frustum* frustum;
};