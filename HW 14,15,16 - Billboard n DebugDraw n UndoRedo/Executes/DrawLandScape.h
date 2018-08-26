#pragma once
#include "Execute.h"

class DrawLandScape : public Execute
{
private:
	enum MODEL_KIND { MODEL_KIND_TREE, MODEL_KIND_TANK, MODEL_KIND_SPHERE };
public:
	DrawLandScape(ExecuteValues* values);
	~DrawLandScape();

	// Execute을(를) 통해 상속됨
	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

	class Terrain* GetTerrain() { return terrain; }

private:
	class Sky* sky;
	class Terrain* terrain;
	vector<class GameModel*> models;

	MODEL_KIND modelKind;
	int selectModel;
	int treeCnt;
};