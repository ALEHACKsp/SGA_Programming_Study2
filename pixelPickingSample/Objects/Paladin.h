#pragma once
#include "GameAnimModel.h"

enum class State
{
	Idle = 0,
	Move,
	Attack,
	Hit,
	Dying,
	End
};

class Paladin : public GameAnimModel
{
public:
	Paladin();
	~Paladin();

	void Update() override;
	void Render() override;

	State GetState() { return state; }
	void SetState(State state) { this->state = state; }

	float GetDamage() { return damage; }
	void Damaged(float val) { hp -= val; }

	Collider* GetSwordCollider() { return swordCollider; }
private:
	State preState;
	State state;

	D3DXMATRIX swordBone;
	Collider* swordCollider;
	class Trail* trail;

	float damage;
	float hp;
};