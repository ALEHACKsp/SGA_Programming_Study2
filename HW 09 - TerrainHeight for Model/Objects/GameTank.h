#pragma once

#include "GameModel.h"

class GameTank : public GameModel
{
public:
	GameTank(wstring matFolder, wstring matFile);
	~GameTank();

	void Update() override;
	void Render() override;

private:
	ModelBone * wheelBones[4];
	D3DXMATRIX wheels[4];

	class Gizmo * gizmo;
};