#include "stdafx.h"
#include "GameTank.h"

GameTank::GameTank()
	: GameModel(Models + L"Materials/", L"Tank.material", Models + L"Meshes/", L"Tank.mesh"), state(TANK_STATE_NONE)
{
	shader = new Shader(Shaders + L"014_Model.hlsl");
	for (Material* material : model->Materials())
		material->SetShader(shader);

	leftWheelBone = model->BoneByName(L"l_back_wheel_geo");
	leftWheel = leftWheelBone->Local();

	rightWheelBone = model->BoneByName(L"r_back_wheel_geo");
	rightWheel = rightWheelBone->Local();

	leftFrontWheelBone = model->BoneByName(L"l_front_wheel_geo");
	leftFrontWheel = leftFrontWheelBone->Local();

	rightFrontWheelBone = model->BoneByName(L"r_front_wheel_geo");
	rightFrontWheel = rightFrontWheelBone->Local();
}

GameTank::~GameTank()
{
	SAFE_DELETE(shader);
}

void GameTank::Update()
{
	__super::Update();

	switch (state)
	{
	case TANK_STATE_FRONT:
		{
			D3DXVECTOR3 position = Position();
			D3DXVECTOR3 dir = Direction();
			D3DXVec3Normalize(&dir, &dir);
			position -= dir * Time::Delta() * 5.0f;
			Position(position);

			D3DXMATRIX R;
			D3DXMatrixRotationX(&R, sinf(Time::Get()->Running() * 2.0f) * 0.25f);
			D3DXMatrixRotationX(&R, Math::ToRadian(Time::Get()->Running() * -100));
			leftWheelBone->Local(R * leftWheel);
			rightWheelBone->Local(R * rightWheel);
			leftFrontWheelBone->Local(R * leftFrontWheel);
			rightFrontWheelBone->Local(R * rightFrontWheel);
			break;
		}
	case TANK_STATE_BACK:
		{
			D3DXVECTOR3 position = Position();
			D3DXVECTOR3 dir = Direction();
			D3DXVec3Normalize(&dir, &dir);
			position += dir * Time::Delta() * 5.0f;
			Position(position);

			D3DXMATRIX R;
			D3DXMatrixRotationX(&R, sinf(Time::Get()->Running() * 2.0f) * 0.25f);
			D3DXMatrixRotationX(&R, Math::ToRadian(Time::Get()->Running() * 100));
			leftWheelBone->Local(R * leftWheel);
			rightWheelBone->Local(R * rightWheel);
			leftFrontWheelBone->Local(R * leftFrontWheel);
			rightFrontWheelBone->Local(R * rightFrontWheel);
			break;
		}
	}
	
}

void GameTank::Render()
{
	__super::Render();
}

bool GameTank::IsMove(D3DXVECTOR3 pos)
{
	if (prePos != pos)
	{
		prePos = pos;
		return true;
	}
	return false;
}

void GameTank::TankState(TANK_STATE state)
{
	this->state = state;
}
