#pragma once
#include "GameAnimModel.h"

enum class MonsterState
{
	Idle = 0,
	Move,
	Attack,
	Hit,
	Dying,
	End
};

class Vanguard : public GameAnimModel
{
public:
	Vanguard();
	~Vanguard();

	void Update() override;
	void Render() override;


	float GetAttackRange() { return attackRange; }
	float GetMoveRange() { return moveRange; }

	float GetCoolTime() { return coolTime; }

	float GetDamage() { return damage; }
	void Damaged(float val) { hp -= val; }

	MonsterState GetState() { return state; }
	void SetState(MonsterState state) { this->state = state; }

	Collider* GetHandCollider() { return handCollider; }
private:
	MonsterState preState;
	MonsterState state;

	D3DXMATRIX handBone;
	Collider* handCollider;

	class Timer* timer;

	float hp;

	float attackRange;
	float moveRange;

	float coolTime;

	float damage;

	bool bLive;
};