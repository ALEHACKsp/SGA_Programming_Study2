#include "stdafx.h"
#include "GameSphere.h"

#include "../Physics/CircleCollider.h"
#include "../Physics/RectCollider.h"

GameSphere::GameSphere(wstring matFolder, wstring meshFolder, 
	wstring name)
	: GameModel(matFolder, name + L".material",
		meshFolder, name + L".mesh")
{
	this->name = name;
	this->colliderType = COLLIDER_TYPE_CIRCLE;

	// collider Setting
	{
		collider = new CircleCollider();

		D3DXVECTOR3 min, max;
		model->MinMaxVertex(&min, &max);
		D3DXVECTOR3 center = (min + max) / 2.0f;
		D3DXVECTOR3 temp = max - center;

		min.y = 0;
		max.y = 0;

		// 임시적으로 크기 min max 를 통해 circle collider의 radius을 구해봣음
		// 현재 문제점은 크기 변환 시 크기에 대응해서 circle collider가 늘어나지 않음
		temp.y = 0;
		float maxTemp = max(temp.x, max(temp.y, temp.z));

		D3DXVECTOR3 halfSize = {
			cosf((float)D3DX_PI / 4.0f) * maxTemp, 0,
			sinf((float)D3DX_PI / 4.0f) * maxTemp };

		collider->SetBound(&center, &halfSize);
	}
}

GameSphere::~GameSphere()
{
	SAFE_DELETE(collider);
}

void GameSphere::Update()
{
	__super::Update();
}

void GameSphere::Render()
{
	__super::Render();

	collider->Render(&Transformed());
}

bool GameSphere::IsCollision(GameModel * model)
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
