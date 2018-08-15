#include "stdafx.h"
#include "GameBullet.h"

#include "../Physics/RectCollider.h"

GameBullet::GameBullet(wstring matFolder, wstring meshFolder, wstring name)
	: GameModel(matFolder, name + L".material",
		meshFolder, name + L".mesh")
	, isFire(false)
{
	this->name = L"Bullet";
	this->colliderType = COLLIDER_TYPE_RECT;

	// collider Setting
	{
		collider = new RectCollider();

		D3DXVECTOR3 min, max;
		model->MinMaxVertex(&min, &max);
		D3DXVECTOR3 center = (min + max) / 2.0f;

		D3DXVECTOR3 halfSize = (max - min) / 2.0f;

		collider->SetBound(&center, &halfSize);
	}
}

GameBullet::~GameBullet()
{
	SAFE_DELETE(collider);
}

void GameBullet::Update()
{
	__super::Update();

	Move();
}

void GameBullet::Render()
{
	__super::Render();
}

void GameBullet::PostRender()
{
	D3DXCOLOR color;
	if (isFire == true)
		color = D3DXCOLOR(1, 0, 0, 1);
	else
		color = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1);
	ImGui::TextColored(ImVec4(color.r, color.g, color.b, color.a),
		String::ToString(name).c_str());

	//__super::PostRender();	
}

bool GameBullet::IsCollision(GameModel * model)
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

void GameBullet::Fire(D3DXVECTOR3& firePos, D3DXVECTOR3& velocity)
{
	firePosition = firePos;
	Position(firePos);
	Velocity(velocity);
}

void GameBullet::Move()
{

}

///////////////////////////////////////////////////////////////////////////////
GameBullets::GameBullets(wstring matFolder, 
	wstring meshFolder, wstring name, int bulletMax, float range)
	: range(range)
{
	for(int i=0; i<bulletMax; i++)
		bullets.push_back(new GameBullet(matFolder, meshFolder, name));
}

GameBullets::~GameBullets()
{
	for (GameBullet* bullet : bullets)
		SAFE_DELETE(bullet);
}

void GameBullets::Update()
{
	for (GameBullet* bullet : bullets)
	{
		if (bullet->GetFire() == false) continue;
		
		bullet->Update();

		D3DXVECTOR3 temp = bullet->FirePosition() - bullet->Position();
		float dist = D3DXVec3Length(&temp);

		if (range < dist)
			bullet->SetFire(false);
	}
}

void GameBullets::Render()
{
	for (GameBullet* bullet : bullets)
	{
		if (bullet->GetFire() == false) continue;

		bullet->Render();
	}
}

void GameBullets::PostRender()
{
	ImGui::Begin("Bullets");

	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Bullet Info");

	for (GameBullet* bullet : bullets)
	{
		bullet->PostRender();
	}

	ImGui::End();
}

bool GameBullets::IsCollision(GameModel * model)
{
	for (GameBullet* bullet : bullets)
	{
		if (bullet->GetFire() == false) continue;

		if (bullet->IsCollision(model))
			return true;
	}

	return false;
}

void GameBullets::Fire(D3DXVECTOR3& firePos, D3DXVECTOR3& velocity)
{
	for (GameBullet* bullet : bullets)
	{
		if (bullet->GetFire() == true) continue;

		bullet->SetFire(true);
		bullet->Fire(firePos, velocity);
		break;
	}
}
