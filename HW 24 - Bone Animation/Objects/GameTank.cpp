#include "stdafx.h"
#include "GameTank.h"

GameTank::GameTank(wstring matFolder, wstring matFile)
	: GameModel(matFolder, matFile, 
		Models + L"Tank/", L"Tank.mesh" )
{
	name = "Tank";
}

GameTank::~GameTank()
{
	
}

void GameTank::Update()
{
	//GameModel::Update();
	// �̷��� �ᵵ �� �ٷ����� �θ� virtual ������ �� ����
	__super::Update(); 
}

void GameTank::Render()
{
	__super::Render();
}
