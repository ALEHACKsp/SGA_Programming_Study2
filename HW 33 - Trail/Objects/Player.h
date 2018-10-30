#pragma once

#include "GameAnimModel.h"

class Player : public GameAnimModel
{
public:
	Player(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile, 
		Direction dir = Direction::RIGHT);
	virtual ~Player();

	void Update() override;
	void Render() override;
	void PostRender() override;

	void Idle() override;
	void Walking() override;
	void Attack() override;
	void Hitted() override;
	void Dying() override;

private:
	int life;
	bool isAttack;

	// Sword_Center
	class Trail* trail;
};