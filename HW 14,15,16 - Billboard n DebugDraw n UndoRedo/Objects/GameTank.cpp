#include "stdafx.h"
#include "GameTank.h"

GameTank::GameTank(wstring matFolder, wstring matFile)
	: GameModel(matFolder, matFile, 
		Models + L"Tank/", L"Tank.mesh" )
{
	shader = new Shader(Shaders + L"Homework/Model2.hlsl");
	for (Material * material : model->Materials())
		material->SetShader(shader);

	name = "Tank";
}

GameTank::~GameTank()
{
	SAFE_DELETE(shader);
}

void GameTank::Update()
{
	//GameModel::Update();
	// 이렇게 써도 됨 바로위에 부모 virtual 없으면 더 위로
	__super::Update(); 
}

void GameTank::Render()
{
	__super::Render();
}
