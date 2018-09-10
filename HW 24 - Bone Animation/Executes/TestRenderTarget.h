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

	void LoadTexture(wstring fileName);

private:
	Shader* shader;

	class Sky* sky;
	class MeshPlane* portrait;
	class MeshPlane* plane;
	class MeshSphere* sphere;
	class MeshCube* cube[2];

	RenderTarget* renderTarget;
	Render2D* render2D;
	Render2D* postEffect;

	function<void(wstring)> func;

	bool isRenderMesh;
};