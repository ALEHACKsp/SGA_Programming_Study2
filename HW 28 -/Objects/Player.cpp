#include "stdafx.h"
#include "Player.h"

#include "../Physics/CapsuleCollider.h"

Player::Player(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile
	, Direction dir)
	: GameAnimModel(matFolder, matFile, meshFolder, meshFile)
{
	this->dir = dir;
	this->isRotate = false;

	moveSpeed = 10.0f;
	isAttack = false;

	// Collider
	{
		collider->Height(90);
		collider->Radius(45);
	}

	// AttackCollider
	{
		attackCollider = new CapsuleCollider(2.0f, 1.0f, CapsuleCollider::Axis::Axis_Z);

		attackBone = model->BoneByName(L"Sword_joint")->Index();
		attackCollider->Position(-12.6f, -2.75f, -48.9f);
		attackCollider->Rotation(-0.07f, 0.26f, 0);
		attackCollider->Height(60.0f);
		attackCollider->Radius(6.0f);
	}
}

Player::~Player()
{
}

void Player::Update()
{
	if (clips.size() < 1) {
		__super::Update();
		return;
	}

	__super::Update();

	// State Machine
	{
		if (isAttack == false) {
			// Walking Start
			if (Keyboard::Get()->Down('A'))
			{
				if (dir == Direction::RIGHT) {
					dir = Direction::LEFT;
					isRotate = true;
					deltaTime = 0;
				}
				else
					state = State::Walking;
			}
			if (Keyboard::Get()->Down('D'))
			{
				if (dir == Direction::LEFT) {
					dir = Direction::RIGHT;
					isRotate = true;
					deltaTime = 0;
				}
				else
					state = State::Walking;
			}
		}

		if (Mouse::Get()->Down(0)) {
			isAttack = true;
			state = State::Attack;
		}

		switch (state)
		{
			case GameAnimModel::State::Idle:
				Idle();
				break;
			case GameAnimModel::State::Walking:
				Walking();
				break;
			case GameAnimModel::State::Attack:
				Attack();
				break;
			case GameAnimModel::State::Hitted:
				Hitted();
				break;
			case GameAnimModel::State::Dying:
				break;
		}
	}
}

void Player::Idle()
{
	__super::Idle();

	if (isRotate) {
		if (dir == Direction::RIGHT) {
			//RotationDegree(0, 180, 0);

			D3DXVECTOR3 rotation = Rotation();
			if (deltaTime <= 1.0f) {
				deltaTime += Time::Delta();
				D3DXVec3Lerp(&rotation, 
					&D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, D3DX_PI, 0), deltaTime);
				Rotation(rotation);
			}
			else {
				rotation.y = D3DX_PI;
				Rotation(rotation);
				isRotate = false;

				if (Keyboard::Get()->Press('D')) {
					state = State::Walking;
					Walking();
				}
			}
		}
		else if (dir == Direction::LEFT) {
			//RotationDegree(0, 0, 0);

			D3DXVECTOR3 rotation = Rotation();
			if (deltaTime <= 1.0f) {
				deltaTime += Time::Delta();
				D3DXVec3Lerp(&rotation,
					&D3DXVECTOR3(0, D3DX_PI, 0), &D3DXVECTOR3(0, 0, 0), deltaTime);
				Rotation(rotation);
			}
			else {
				rotation.y = 0;
				Rotation(rotation);
				isRotate = false;

				if (Keyboard::Get()->Press('A')) {
					state = State::Walking;
					Walking();
				}
			}
		}
	}
}

void Player::Walking()
{
	__super::Walking();

	if (Keyboard::Get()->Press('A'))
	{
		Position(Position() + D3DXVECTOR3(0, 0, -moveSpeed * Time::Delta()));
	}
	else if (Keyboard::Get()->Press('D'))
	{
		Position(Position() + D3DXVECTOR3(0, 0, moveSpeed * Time::Delta()));
	}

	if (Keyboard::Get()->Up('A') ||
		Keyboard::Get()->Up('D'))
	{
		Idle();
		state = State::Idle;
	}
}

void Player::Attack()
{
	__super::Attack();

	for (int i = 0; i < otherModels.size(); i++) {
		if (Collision::IsOverlapCapsuleCapsule(
			&GetAttackColldierBone(), this->GetAttackCollider(),
			&otherModels[i]->GetColldierBone(), otherModels[i]->GetCollider())) {
			otherModels[i]->SetState(State::Hitted);
		}
	}

	if (this->IsPlay() == false) {
		Idle();
		state = State::Idle;
		isAttack = false;
	}
}
