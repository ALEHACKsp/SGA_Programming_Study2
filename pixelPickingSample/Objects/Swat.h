#pragma once
#include "GameAnimModel.h"

enum class State
{
	Idle = 0,
	Move,
	FiringRifle = 9,
	Reload,
	HitReaction,
	Dying,
	End
};

enum class MoveState
{
	RunForward = 1,
	RunForwardLeft,
	RunForwardRight,
	RunLeft,
	RunRight,
	RunBackward,
	RunBackwardLeft,
	RunBackwardRight,
};

class Swat : public GameAnimModel
{
public:
	Swat();
	~Swat();

	void Update() override;
	void Render() override;

	State GetState() { return state; }
	void SetState(State state) { this->state = state; }

	void SetMoveState(MoveState state) { this->moveState = state; }

	float GetDamage() { return damage; }
	void Damaged(float val) { hp -= val; }

	Collider* GetRayCollider();

private:
	State preState;
	State state;
	MoveState preMoveState;
	MoveState moveState;

	D3DXVECTOR3 shoulderLink;
	D3DXMATRIX handBone;
	class AK* ak;

	float damage;
	float hp;
};