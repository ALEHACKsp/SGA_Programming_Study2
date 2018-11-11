#include "stdafx.h"
#include "Monster.h"

#include "./Physics/CapsuleCollider.h"

Monster::Monster(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile)
	: GameAnimModel(matFolder, matFile, meshFolder, meshFile)
{
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

	searchDistance = 50.0f;
	attackDistance = 27.5f;

	searchDebug = new DebugDrawSphere(searchDistance);
	searchDebug->SetColor(D3DXCOLOR(1, 1, 0, 1));
	attackDebug = new DebugDrawSphere(attackDistance);
	attackDebug->SetColor(D3DXCOLOR(1, 0, 0, 1));

	selectDebug = 0;
	showDebug = true;

	moveSpeed = 5.0f;
}

Monster::~Monster()
{
	SAFE_DELETE(searchDebug);
	SAFE_DELETE(attackDebug);
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
		if (state == State::Idle || state == State::Walking) {
			if (state != State::Attack) {
				state = State::Idle;
			}

			if (AttackTarget()) {
				state = State::Attack;
			}
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
				Dying();
				break;
		}
	}

	searchDebug->Position(this->Position());
	attackDebug->Position(this->Position());
}

void Monster::Render()
{
	__super::Render();

	if (isRender == false)
		return;

	if (showDebug) {
		searchDebug->Render();
		attackDebug->Render();
	}
}

void Monster::PostRender()
{
	__super::PostRender();


	ImGui::Begin("Monster Debuger");
	ImGui::Checkbox("Show Debug", &showDebug);
	ImGui::RadioButton("search", &selectDebug, 0);
	ImGui::SameLine(120);
	ImGui::RadioButton("attack", &selectDebug, 1);

	if (showDebug) {
		if (selectDebug == 0)
			searchDebug->PostRender();
		else if (selectDebug == 1)
			attackDebug->PostRender();
	}

	ImGui::End();
}

void Monster::Idle()
{
	__super::Idle();

}

void Monster::Walking()
{
	__super::Walking();

}

void Monster::Attack()
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
		isRender = false;
	}
}

bool Monster::SearchTarget()
{
	for (int i = 0; i < otherModels.size(); i++) {
		if (otherModels[i]->GetState() == State::Dying) continue;

		D3DXVECTOR3 temp = Position() - otherModels[i]->Position();
		float dist = D3DXVec3Length(&temp);
		if (dist <= searchDistance) {
			return true;
		}
	}

	return false;
}

bool Monster::AttackTarget()
{
	for (int i = 0; i < otherModels.size(); i++) {
		if (otherModels[i]->GetState() == State::Dying) continue;

		D3DXVECTOR3 temp = Position() - otherModels[i]->Position();
		float dist = D3DXVec3Length(&temp);
		if (dist <= attackDistance) {
			return true;
		}
	}

	return false;
}
