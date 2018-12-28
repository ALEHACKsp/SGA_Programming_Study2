#pragma once
#include "GameModel.h"

class GameTree : public GameModel
{
public:
	GameTree(ExecuteValues* values);
	~GameTree();

	void Update();
	void Render();

private:
	class GameModel* cross;

	D3DXVECTOR3 rotation;

	RasterizerState* cullMode[2];

	ExecuteValues* values;
};