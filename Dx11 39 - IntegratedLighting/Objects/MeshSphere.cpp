#include "stdafx.h"
#include "MeshSphere.h"

MeshSphere::MeshSphere()
	: GameModel(Models + L"Meshes/", L"Sphere.material",
		Models + L"Meshes/", L"Sphere.mesh" )
{

}

MeshSphere::~MeshSphere()
{

}

void MeshSphere::Update()
{
	//GameModel::Update();
	// 이렇게 써도 됨 바로위에 부모 virtual 없으면 더 위로
	__super::Update(); 
}

void MeshSphere::Render()
{
	__super::Render();
}
