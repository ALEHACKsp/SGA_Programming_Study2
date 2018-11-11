#pragma once

#include "GameAnimModel.h"

class Monster : public GameAnimModel
{
public:
	Monster(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile,
		Direction dir = Direction::LEFT);
	virtual ~Monster();

	void Update();
	void Render();
	void PostRender();

	void Idle();
	void Walking();
	void Attack();
	void Hitted();
	void Dying();

	bool SearchTarget(OUT Direction& otherDir);
	bool AttackTarget(OUT Direction& otherDir);

private:
	int life;

	bool showDebug;

	float searchDistance;
	float attackDistance;

	class DebugDrawSphere* searchDebug;
	class DebugDrawSphere* attackDebug;

	int selectDebug;
};