#pragma once
#include "Execute.h"

class ExePicking : public Execute
{
public:
	ExePicking(ExecuteValues* values);
	~ExePicking();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
	class Terrain* terrain;
	class PickingObject* objPicking;
	class GameModel* pickedObj;
	class ModelBone* pickedPart;
	D3DXMATRIX partTransform;
	D3DXVECTOR2 partRot;

	vector<class GameModel *> objects;
};