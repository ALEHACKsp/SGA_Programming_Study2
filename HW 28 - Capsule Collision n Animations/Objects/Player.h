#pragma once

#include "GameAnimModel.h"

class Player : public GameAnimModel
{
public:
	Player(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile, 
		Direction dir = Direction::RIGHT);
	virtual ~Player();

	void Update();

	void Idle();
	void Walking();
	void Attack();
	void Hitted();
	void Dying();

private:
	int life;
	bool isAttack;
};