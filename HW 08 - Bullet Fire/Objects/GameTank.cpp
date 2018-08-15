#include "stdafx.h"
#include "GameTank.h"

#include "../Physics/RectCollider.h"

#include "GameBullet.h"

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

		D3DXVECTOR3 halfSize = (max - min) / 2.0f;

		collider->SetBound(&center, &halfSize);
	}

	// wheel
	{
		wstring boneName[] = { L"l_front_wheel_geo", L"r_front_wheel_geo",
		L"l_back_wheel_geo", L"r_back_wheel_geo" };

		for (int i = 0; i < 4; i++) {
			wheelBones[i] = model->BoneByName(boneName[i]);
			wheels[i] = wheelBones[i]->Local();
		}
	}

	// turret 
	{
		turretBone = model->BoneByName(L"turret_geo");
		turret = turretBone->Local();

		turretAngle = 0;
	}

	// bullet
	{
		wstring matFolder = Materials + L"Meshes/";
		wstring meshFolder = Models + L"Meshes/";

		bullets = new GameBullets(matFolder, meshFolder, L"Sphere", 10, 30);

	}
}

GameTank::~GameTank()
{
	SAFE_DELETE(collider);
	SAFE_DELETE(bullets);
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
		{
			position += -forward * moveSpeed * Time::Delta();

			D3DXMATRIX R;
			for (int i = 0; i < 4; i++)
			{
				D3DXMatrixRotationX(&R, 
					Math::ToRadian(Time::Get()->Running() * -100));
				wheelBones[i]->Local(R * wheels[i]);
			}
		}
		else if (Keyboard::Get()->Press('S')) 
		{
			position += forward * moveSpeed * Time::Delta();
			D3DXMATRIX R;
			for (int i = 0; i < 4; i++)
			{
				D3DXMatrixRotationX(&R,
					Math::ToRadian(Time::Get()->Running() * 100));
				wheelBones[i]->Local(R * wheels[i]);
			}
		}

		//if (Keyboard::Get()->Press('A'))
		//	position += right * moveSpeed * Time::Delta();
		//else if (Keyboard::Get()->Press('D'))
		//	position += -right * moveSpeed * Time::Delta();

		if (Keyboard::Get()->Press('Q'))
			Rotate(D3DXVECTOR2(-rotateSpeed, 0));
		else if (Keyboard::Get()->Press('E'))
			Rotate(D3DXVECTOR2(rotateSpeed, 0));

		if (Keyboard::Get()->Press('A')) 
		{
			turretAngle -= rotateSpeed / 15 * Time::Delta();
		}
		else if (Keyboard::Get()->Press('D'))
		{
			turretAngle += rotateSpeed / 15 * Time::Delta();
		}

		if (Keyboard::Get()->Down(VK_SPACE))
		{
			D3DXVECTOR3 v = { 
				cosf(turretAngle + Math::ToRadian(90)), 
				0, 
				sinf(turretAngle - Math::ToRadian(90)) };
			D3DXVec3TransformNormal(&v, &v, &Transformed());
			bullets->Fire(Position(), v * moveSpeed);
		}

		D3DXMATRIX R;
		D3DXMatrixRotationY(&R, turretAngle);
		turretBone->Local(R * turret);

		Position(position);
	}

	__super::Update();

	bullets->Update();
}

void GameTank::Render()
{
	__super::Render();

	collider->Render(&Transformed());

	bullets->Render();
}

void GameTank::PostRender()
{
	__super::PostRender();

	string label = "turret angle";
	if (ImGui::DragFloat(label.c_str(), &turretAngle, 0.01f))
	{
		D3DXMATRIX R;
		D3DXMatrixRotationY(&R, turretAngle);
		turretBone->Local(R * turret);
	}

	bullets->PostRender();
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

	result |= IsBulletCollision(model);

	return result;
}

bool GameTank::IsBulletCollision(GameModel * model)
{
	return bullets->IsCollision(model);
}
