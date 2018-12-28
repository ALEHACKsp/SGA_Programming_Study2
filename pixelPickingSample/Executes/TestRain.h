#pragma once
#include "Execute.h"

class TestRain : public Execute
{
public:
	TestRain(ExecuteValues* values);
	~TestRain();

	void Update();

	void PreRender();
	void Render();
	void PostRender();

	void ResizeScreen() {}

private:
	void WorldRender();

private:
	class Rain* rain;
	
	ID3D11Buffer* vertexBuffer[2];

	Shader* shader;
	WorldBuffer* worldBuffer;
	Texture* texture[6];
};