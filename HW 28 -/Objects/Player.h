#pragma once

#include "GameAnimModel.h"

class Player : public GameAnimModel
{
public:
	Player(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile, 
		Direction dir = Direction::RIGHT);
	virtual ~Player();

	virtual void Update();

	virtual void Idle();
	virtual void Walking();
	virtual void Attack();

private:
	float deltaTime;
	bool isAttack;
};