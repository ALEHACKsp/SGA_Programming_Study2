#pragma once
#include "Execute.h"

class TestImGui : public Execute
{
public:
	TestImGui(ExecuteValues* values);
	~TestImGui();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	struct Item
	{
		int type;
		string name;
		ImVec2 a, b, c, d;
		ImVec2 center;
		float radius;
		ImVec4 color;
	};
};