#pragma once
#include "Execute.h"

class TestImGuizmo : public Execute
{
public:
	TestImGuizmo(ExecuteValues* values);
	~TestImGuizmo();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	D3DXMATRIX world;
};