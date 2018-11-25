#pragma once
#include "Execute.h"

class ExHomework : public Execute
{
public:
	ExHomework();
	~ExHomework();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

	void CreateInstance(int count);

private:
	class Terrain* terrain;

	Shader* shader;
	ID3D11Buffer* vertexBuffer[2];
	int instanceCountFactor;
	int instanceCount;
	float flowerFactor;
	TextureArray* textureArray;

private:
	struct VertexWorld
	{
		D3DXMATRIX World;
		float MoveSpeed;
	};

	vector<VertexWorld> vertices;
};