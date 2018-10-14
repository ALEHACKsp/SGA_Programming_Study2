#pragma once
#include "Execute.h"

class TestProjection : public Execute
{
public:
	TestProjection(ExecuteValues* values);
	~TestProjection();

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

	class Projection* projection;
};