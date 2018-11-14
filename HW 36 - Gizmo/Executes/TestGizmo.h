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
	Material* cubeMaterial;
	BBox* cubeBounding;
	DebugLine* cubeLine;

	//class GizmoModel* translate;
	//class GizmoModel* rotate;
	//class GizmoModel* scale;
	
	class Gizmo* gizmo;
};