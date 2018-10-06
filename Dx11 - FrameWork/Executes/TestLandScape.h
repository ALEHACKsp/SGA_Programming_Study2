#pragma once
#include "Execute.h"

class TestLandScape : public Execute
{
public:
	TestLandScape(ExecuteValues* values);
	~TestLandScape();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	class Sky* sky;
	class TerrainRender* terrain;
	class Water* water;
};