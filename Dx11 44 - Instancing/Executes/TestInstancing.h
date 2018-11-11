#pragma once
#include "Execute.h"

class TestInstancing : public Execute
{
public:
	TestInstancing(ExecuteValues* values);
	~TestInstancing();
	
	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	void WorldRender();

private:
	ID3D11Buffer* vertexBuffer[2];

	Shader* shader;
	WorldBuffer* worldBuffer;

	Texture* texture[6];
};