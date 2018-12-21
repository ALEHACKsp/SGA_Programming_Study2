#pragma once
#include "Execute.h"

class TestLine : public Execute
{
public:
	TestLine();
	~TestLine();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	BBox* bBox;
	DebugLine* boxLine;

	MeshCube* cube;
	Material* cubeMaterial;
};