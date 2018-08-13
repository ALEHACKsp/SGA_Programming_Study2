#include "stdafx.h"
#include "GameRect.h"

#include "../Physics/RectCollider.h"

GameRect::GameRect(wstring matFolder, wstring meshFolder, 
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

GameRect::~GameRect()
{
	SAFE_DELETE(collider);
}

void GameRect::Update()
{
	__super::Update();
}

void GameRect::Render()
{
	__super::Render();

	collider->Render(&Transformed());
}

bool GameRect::IsCollision(GameModel * model)
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
