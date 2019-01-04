#pragma once
#include "Execute.h"

class TestRain : public Execute
{
public:
	TestRain();
	~TestRain();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	class Rain* rain;
};