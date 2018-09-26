#include "stdafx.h"
#include "Monster.h"

#include "./Physics/CapsuleCollider.h"

Monster::Monster(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile
	, Direction dir)
	: GameAnimModel(matFolder, matFile, meshFolder, meshFile)
{
	this->dir = dir;
	this->isRotate = false;

	// Collider
	{
		collider->Position(0, -5.0f, -13.0f);

		collider->Height(100);
		collider->Radius(40);
	}

	// AttackCollider
	{
		attackCollider = new CapsuleCollider(2.0f, 1.0f, CapsuleCollider::Axis::Axis_X);

		attackBone = model->BoneByName(L"RightHand")->Index();

		attackCollider->Position(-6.15f, -2.05f, -2.25f);
		attackCollider->Rotation(-0.0f, -0.25f, 0.42f);
		attackCollider->Height(7.0f);
		attackCollider->Radius(9.0f);
	}

	life = 3;
}

Monster::~Monster()
{
}

void Monster::Update()
{
	if (clips.size() < 1) {
		__super::Update();
		return;
	}

	__super::Update();

	// State Machine
	{
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
				Dying();
				break;
		}
	}
}

void Monster::Hitted()
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

void Monster::Dying()
{
	__super::Dying();

	if (this->IsPlay() == false) {
		//isRender = false;
	}
}
