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
	class GizmoComponent* gizmoComponent;
};