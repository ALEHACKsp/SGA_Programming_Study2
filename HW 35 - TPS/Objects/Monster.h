#pragma once

#include "GameAnimModel.h"

class Monster : public GameAnimModel
{
public:
	Monster(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile);
	virtual ~Monster();

	void Update();
	void Render();
	void PostRender();

	void Idle();
	void Walking();
	void Attack();
	void Hitted();
	void Dying();

	bool SearchTarget();
	bool AttackTarget();

private:
	int life;

	bool showDebug;

	float searchDistance;
	float attackDistance;

	class DebugDrawSphere* searchDebug;
	class DebugDrawSphere* attackDebug;

	int selectDebug;
};