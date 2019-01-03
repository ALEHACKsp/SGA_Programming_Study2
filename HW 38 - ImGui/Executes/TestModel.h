#pragma once
#include "Execute.h"

class TestModel : public Execute
{
public:
	TestModel();
	~TestModel();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	D3DXMATRIX matCube;

	vector<GameModel*> models;
};