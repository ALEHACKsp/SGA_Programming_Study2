#pragma once
#include "Execute.h"

class TestDivision : public Execute
{
public:
	TestDivision(ExecuteValues* values);
	~TestDivision();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	Shader* shader;
	WorldBuffer* worldBuffer;

	ID3D11Buffer* vertexBuffer;
	RasterizerState* rasterizerState[2];
};