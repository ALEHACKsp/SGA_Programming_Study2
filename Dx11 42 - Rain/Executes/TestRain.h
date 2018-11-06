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
	class MeshPlane* plane;
	Shader* planeShader;

	class Rain* rain;
};