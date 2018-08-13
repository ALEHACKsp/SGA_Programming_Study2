#include "stdafx.h"
#include "GameTank.h"

#include "../Physics/RectCollider.h"

GameTank::GameTank(wstring matFolder, wstring meshFolder, 
	wstring name)
	: GameModel(matFolder, name + L".material",
		meshFolder, name + L".mesh")
{
	this->name = name;
	this->colliderType = COLLIDER_TYPE_RECT;

	// collider Setting
	{
		collider = new RectCollider();

		D3DXVECTOR3 min, max;
		model->MinMaxVertex(&min, &max);
		D3DXVECTOR3 center = (min + max) / 2.0f;
		D3DXVECTOR3 temp = max - center;

		min.y = 0;
		max.y = 0;
		D3DXVECTOR3 halfSize = halfSize = (max - min) / 2.0f;

		collider->SetBound(&center, &halfSize);
	}
}

GameTank::~GameTank()
{
	SAFE_DELETE(collider);
}

void GameTank::Update()
{
	D3DXVECTOR3 position = Position();
	float moveSpeed = 10.0f;
	float rotateSpeed = 60.0f;

	D3DXVECTOR3 forward, right;
	forward = Direction();
	right = Right();

	if (!Mouse::Get()->Press(1)) {
		if (Keyboard::Get()->Press('W'))
			position += -forward * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('S'))
			position += forward * moveSpeed * Time::Delta();

		if (Keyboard::Get()->Press('A'))
			position += right * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('D'))
			position += -right * moveSpeed * Time::Delta();

		if (Keyboard::Get()->Press('E'))
			Rotate(D3DXVECTOR2(rotateSpeed, 0));
		else if (Keyboard::Get()->Press('Q'))
			Rotate(D3DXVECTOR2(-rotateSpeed, 0));

		Position(position);
	}

	__super::Update();
}

void GameTank::Render()
{
	__super::Render();

	collider->Render(&Transformed());
}

bool GameTank::IsCollision(GameModel * model)
{
	bool result = false;

	if (model->GetColliderType() == COLLIDER_TYPE_RECT)
	{
		RectCollider * targetCollider = (RectCollider*)model->GetCollider();

		result = Collision::IsOverlap(&Transformed(), collider,
			&model->Transformed(), targetCollider);
	}
	else if (model->GetColliderType() == COLLIDER_TYPE_CIRCLE)
	{
		CircleCollider * targetCollider = (RectCollider*)model->GetCollider();

		result = Collision::IsOverlap(&Transformed(), collider,
			&model->Transformed(), targetCollider);
	}

	return result;
}
