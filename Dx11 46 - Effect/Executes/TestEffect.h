#pragma once
#include "Execute.h"

class TestEffect : public Execute
{
public:
	TestEffect();
	~TestEffect();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	Shader* shader;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	Vertex* vertices;
	UINT* indices;
};