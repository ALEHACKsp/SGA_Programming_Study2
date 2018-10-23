#pragma once
#include "Execute.h"

class TestScattering : public Execute
{
public:
	TestScattering(ExecuteValues* values);
	~TestScattering();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	class Sky* sky;
	class TerrainRender* terrain;
};