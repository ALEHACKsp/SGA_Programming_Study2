#pragma once

#include "GameAnimModel.h"

class Monster : public GameAnimModel
{
public:
	Monster(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile,
		Direction dir = Direction::LEFT);
	virtual ~Monster();

	virtual void Update();

	virtual void Hitted();
	virtual void Dying();

private:
	int life;
};