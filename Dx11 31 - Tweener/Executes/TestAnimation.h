#pragma once
#include "Execute.h"

class TestAnimation : public Execute
{
public:
	TestAnimation(ExecuteValues* values);
	~TestAnimation();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	class GameAnimModel* kachujin;
	Shader* shader;
};