#pragma once
#include "Execute.h"

class TestShadow : public Execute
{
public:
	TestShadow(ExecuteValues* values);
	~TestShadow();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	Shader* hanjoShader;
	class GameAnimModel* hanjo;

	Shader* planeShader;
	class MeshPlane* plane;

	class Shadow* shadow;
};