#pragma once
#include "Execute.h"

class TestScattering : public Execute
{
public:
	TestScattering();
	~TestScattering();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}
	
private:
	class ScatteringSky* sky;
};