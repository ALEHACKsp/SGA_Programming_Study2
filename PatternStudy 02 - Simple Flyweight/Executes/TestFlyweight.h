#pragma once
#include "Execute.h"

class TestFlyweight : public Execute
{
public:
	TestFlyweight(ExecuteValues* values);
	~TestFlyweight();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	vector<class Line*> debugs;
	//vector<D3DXVECTOR4> positions;
	//vector<D3DXCOLOR> colors;
	vector<D3DXMATRIX> worlds;
	class Line* instance;

	bool bInstance;
};