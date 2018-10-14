#include "stdafx.h"
#include "MeshPlane.h"

MeshPlane::MeshPlane()
	: GameModel(Models + L"Meshes/", L"Plane.material", 
		Models + L"Meshes/", L"Plane.mesh" )
{

}

MeshPlane::~MeshPlane()
{

}

void MeshPlane::Update()
{
	//GameModel::Update();
	// 이렇게 써도 됨 바로위에 부모 virtual 없으면 더 위로
	__super::Update(); 
}

void MeshPlane::Render()
{
	__super::Render();
}
