#pragma once
#include "Execute.h"

class TestGizmo : public Execute
{
public:
	TestGizmo(ExecuteValues* values);
	~TestGizmo();
	
	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	class MeshGrid* grid;
	Material* gridMaterial;

	Ray* ray;

	bool bPicking;

	class MeshCube* cube;
	struct BBox* cubeBounding;
	Material* meshMaterial;
	DebugLine* cubeLine;

	//class MeshSphere* sphere;
	//struct BSphere* sphereBounding;

	//class GizmoModel* translate;
	//class GizmoModel* rotate;
	//class GizmoModel* scale;
	
	class Gizmo* gizmo;

	//class MeshQuad* quad;
};