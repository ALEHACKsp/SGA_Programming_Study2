#pragma once
#include "Execute.h"

class TestDetailMap : public Execute
{
public:
	TestDetailMap(ExecuteValues* values);
	~TestDetailMap();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	Shader* shader;
	Shader* detailShader;

	class Sky* sky;
	class MeshPlane* plane;
	//class MeshSphere* sphere;
	//class MeshCube* cube[2];

	RenderTarget* renderTarget[2];
	Render2D* postEffect;
};