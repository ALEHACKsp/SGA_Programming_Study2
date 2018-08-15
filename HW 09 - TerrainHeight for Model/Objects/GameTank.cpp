#include "stdafx.h"
#include "GameTank.h"

#include "../Renders/Gizmo.h"

GameTank::GameTank(wstring matFolder, wstring matFile)
	: GameModel(matFolder, matFile, 
		Models + L"Tank/", L"Tank.mesh" )
{
	shader = new Shader(Shaders + L"/Homework/Model2.hlsl");
	for (Material* material : model->Materials())
		material->SetShader(shader);

	wstring boneName[] = { L"l_front_wheel_geo", L"r_front_wheel_geo",
		L"l_back_wheel_geo", L"r_back_wheel_geo" };

	for (int i = 0; i < 4; i++) {
		wheelBones[i] = model->BoneByName(boneName[i]);
		wheels[i] = wheelBones[i]->Local();
	}

	gizmo = new Gizmo(2.5f);
}

GameTank::~GameTank()
{
	SAFE_DELETE(shader);

	SAFE_DELETE(gizmo);
}

void GameTank::Update()
{
	//GameModel::Update();
	// 이렇게 써도 됨 바로위에 부모 virtual 없으면 더 위로
	__super::Update(); 

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

		if (Keyboard::Get()->Press('A'))
			position += -right * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('D'))
			position += right * moveSpeed * Time::Delta();

		if (Keyboard::Get()->Press('Q')) {
			//Rotate(D3DXVECTOR2(-rotateSpeed, 0));
			D3DXMATRIX R;
			D3DXMatrixRotationY(&R, -Time::Delta());
			World(R * World());
		}
		else if (Keyboard::Get()->Press('E')) {
			//Rotate(D3DXVECTOR2(rotateSpeed, 0));
			D3DXMATRIX R;
			D3DXMatrixRotationY(&R, Time::Delta());
			World(R * World());
		}

		Position(position);
	}
}

void GameTank::Render()
{
	__super::Render();

	gizmo->Render(Position() + D3DXVECTOR3(0,1,0), 
		-Direction(), Up(), Right());
}
