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
	class BehaviorTree* behaviorTree;
};