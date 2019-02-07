#pragma once
#include "Execute.h"

class TestImGui : public Execute
{
public:
	TestImGui();
	~TestImGui();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

	void ImGuiNodeGraph(bool * opened);

private:
	//class Unit* unit;
	class TestUnit* unit;
	class BehaviorTree* behaviorTree;

private:
	class MeshCube* cube;
	Material* meshMaterial;

	class RenderTargetView* rtv;
	class DepthStencilView* dsv;

	D3DXMATRIX matCube;
};