#pragma once
#include "Execute.h"

class TestDebugDraw : public Execute
{
public:
	TestDebugDraw(ExecuteValues* values);
	~TestDebugDraw();
	
	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	class MeshGrid* grid;
	Material* gridMaterial;

	Ray* ray;

	class MeshCube* cube;
	Material* cubeMaterial;
	BBox* cubeBounding;
	bool cubeTest;
	DebugLine* cubeLine;

	class MeshSphere* sphere;
	Material* sphereMaterial;
	BSphere* sphereBounding;
	bool sphereTest;
	DebugLine* sphereLine;

	float cubeResult;
	float sphereResult;
};