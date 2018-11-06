#pragma once
#include "Execute.h"

class TestBillboard : public Execute
{
public:
	TestBillboard(ExecuteValues* values);
	~TestBillboard();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	class MeshPlane* plane;
	Shader* planeShader;

	Shader* shader;
	WorldBuffer* worldBuffer;

	ID3D11Buffer* vertexBuffer;
	RasterizerState* rasterizerState[2];

	Texture* texture;
};