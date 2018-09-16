#include "stdafx.h"
#include "GameTank.h"

GameTank::GameTank(wstring matFolder, wstring matFile)
	: GameModel(matFolder, matFile, 
		Models + L"Tank/", L"Tank.mesh" )
{
	// 왼쪽 뒷바퀴
	leftWheelBone = model->BoneByName(L"l_back_wheel_geo");
	leftWheel = leftWheelBone->Local();
}

GameTank::~GameTank()
{
	
}

void GameTank::Update()
{
	//GameModel::Update();
	// 이렇게 써도 됨 바로위에 부모 virtual 없으면 더 위로
	__super::Update(); 

	D3DXMATRIX R;
	// sinf 안이 각도 밖이 speed값
	// 포탑용 코드 회전왓다갓다 함
	//D3DXMatrixRotationX(&R, sinf(Time::Get()->Running() * 2.0f) * 0.25f);
	D3DXMatrixRotationX(&R, Math::ToRadian(Time::Get()->Running() * -100));
	
	// 기준점에다 곱해준 거
	leftWheelBone->Local(R * leftWheel);
}

void GameTank::Render()
{
	__super::Render();
}
