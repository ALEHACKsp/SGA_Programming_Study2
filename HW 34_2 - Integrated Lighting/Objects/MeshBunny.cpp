#include "stdafx.h"
#include "MeshBunny.h"

MeshBunny::MeshBunny()
	: GameModel(Models + L"Meshes/", L"StanfordBunny.material",
		Models + L"Meshes/", L"StanfordBunny.mesh" )
{

}

MeshBunny::~MeshBunny()
{

}

void MeshBunny::Update()
{
	//GameModel::Update();
	// 이렇게 써도 됨 바로위에 부모 virtual 없으면 더 위로
	__super::Update(); 
}

void MeshBunny::Render()
{
	__super::Render();
}
