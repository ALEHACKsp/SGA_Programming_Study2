#include "stdafx.h"
#include "Player.h"

#include "../Physics/CapsuleCollider.h"

Player::Player(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile)
	: GameAnimModel(matFolder, matFile, meshFolder, meshFile)
{
	moveSpeed = 25.0f;
	rotationSpeed = 1.5f;
	isAttack = false;

	// Collider
	{
		collider->Height(90);
		collider->Radius(45);
	}

	// AttackCollider
	{
		//attackCollider = new CapsuleCollider(2.0f, 1.0f, CapsuleCollider::Axis::Axis_Z);

		//if (name == "Paladin")
		//{
		//	attackBone = model->BoneByName(L"Sword_joint")->Index();
		//	attackCollider->Position(-12.6f, -2.75f, -48.9f);
		//	attackCollider->Rotation(-0.07f, 0.26f, 0);
		//	attackCollider->Height(60.0f);
		//	attackCollider->Radius(6.0f);
		//}

		//else if (name == "BigVegas")
		//{
		//	attackBone = model->BoneByName(L"Weapon")->Index();
		//	attackCollider->Position(-12.6f, -2.75f, -48.9f);
		//	attackCollider->Rotation(-0.07f, 0.26f, 0);
		//	attackCollider->Height(60.0f);
		//	attackCollider->Radius(6.0f);
		//}

	}

	life = 3;
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
			if (Keyboard::Get()->Down('W'))
				state = State::Walking;
		}

		if (state != State::Dying && Mouse::Get()->Down(0)) {
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

	if (!Keyboard::Get()->Press('C')) {
		D3DXVECTOR3 rotation = Rotation();
		// 마우스 이동값 가져오는거 얼마만큼 이동했는지의 차값
		D3DXVECTOR3 val = Mouse::Get()->GetMoveValue();

		// 좌우회전이 y축 회전
		rotation.y += val.x * rotationSpeed * Time::Delta();
		Rotation(rotation);
	}
}

void Player::Walking()
{
	__super::Walking();

	if (!Keyboard::Get()->Press('C')) {
		D3DXVECTOR3 rotation = Rotation();
		// 마우스 이동값 가져오는거 얼마만큼 이동했는지의 차값
		D3DXVECTOR3 val = Mouse::Get()->GetMoveValue();

		// 좌우회전이 y축 회전
		rotation.y += val.x * rotationSpeed * Time::Delta();
		Rotation(rotation);
	}

	if (Keyboard::Get()->Press('W')) {
		D3DXVECTOR3 pos = Position();
		D3DXVECTOR3 dir = Direction();
		D3DXVec3Normalize(&dir, &dir);

		pos += -dir * this->moveSpeed * Time::Delta();
		Position(pos);
	}

	if (Keyboard::Get()->Up('W'))
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

void Player::Hitted()
{
	__super::Hitted();

	if (this->IsPlay() == false) {
		life--;

		if (life > 0) {
			Idle();
			state = State::Idle;
		}
		else {
			Dying();
			state = State::Dying;
		}
	}
}

void Player::Dying()
{
	__super::Dying();

	if (this->IsPlay() == false) {
		//isRender = false;
	}
}
