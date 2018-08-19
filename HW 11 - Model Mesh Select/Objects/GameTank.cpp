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
	// �̷��� �ᵵ �� �ٷ����� �θ� virtual ������ �� ����
	__super::Update(); 
}

void GameTank::Render()
{
	__super::Render();
}
