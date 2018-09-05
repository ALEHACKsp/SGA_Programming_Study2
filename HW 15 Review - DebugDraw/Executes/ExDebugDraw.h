#pragma once
#include "Execute.h"

class ExDebugDraw : public Execute
{
public:
	ExDebugDraw(ExecuteValues* values);
	~ExDebugDraw();

	// Execute을(를) 통해 상속됨
	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();
	
	void SetTerrain(class Terrain* terrain) { this->terrain = terrain; }

private:
	vector<class Line*> lines;

	class Terrain* terrain;

	int selectLine;

	bool bPicked;
	D3DXVECTOR3 pickPos;
};