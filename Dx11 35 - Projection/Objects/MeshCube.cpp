#include "stdafx.h"
#include "MeshCube.h"

MeshCube::MeshCube()
	: GameModel(Models + L"Meshes/", L"Cube.material", 
		Models + L"Meshes/", L"Cube.mesh" )
{

}

MeshCube::~MeshCube()
{

}

void MeshCube::Update()
{
	//GameModel::Update();
	// �̷��� �ᵵ �� �ٷ����� �θ� virtual ������ �� ����
	__super::Update(); 
}

void MeshCube::Render()
{
	__super::Render();
}
