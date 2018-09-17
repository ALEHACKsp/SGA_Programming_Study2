#pragma once
#include "Execute.h"

class TestImGuiDragnDrop : public Execute
{
public:
	TestImGuiDragnDrop(ExecuteValues* values);
	~TestImGuiDragnDrop();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:

};