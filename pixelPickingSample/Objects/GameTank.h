#pragma once
#include "GameModel.h"

enum TANK_STATE
{
	TANK_STATE_NONE,
	TANK_STATE_FRONT,
	TANK_STATE_BACK,
	TANK_STATE_END
};

class GameTank : public GameModel
{
public:
	GameTank();
	~GameTank();

	void Update() override;
	void Render() override;

	bool IsMove(D3DXVECTOR3 pos);

	void TankState(TANK_STATE state);
private:
	ModelBone* leftWheelBone;
	D3DXMATRIX leftWheel;
	
	ModelBone* rightWheelBone;
	D3DXMATRIX rightWheel;

	ModelBone* leftFrontWheelBone;
	D3DXMATRIX leftFrontWheel;

	ModelBone* rightFrontWheelBone;
	D3DXMATRIX rightFrontWheel;


	D3DXVECTOR3 prePos;
	
	TANK_STATE state;
};