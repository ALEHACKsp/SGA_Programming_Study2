#pragma once
#include "Execute.h"

class TestRenderTarget : public Execute
{
public:
	TestRenderTarget(ExecuteValues* values);
	~TestRenderTarget();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	Shader* shader;

	class Sky* sky;
	class MeshPlane* plane;
	class MeshSphere* sphere;
	class MeshCube* cube[2];

	RenderTarget* renderTarget[3];
	Render2D* postEffect[3];

	class PointLight* pointLight;
};