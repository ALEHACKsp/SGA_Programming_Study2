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
	// �̷��� �ᵵ �� �ٷ����� �θ� virtual ������ �� ����
	__super::Update(); 
}

void MeshPlane::Render()
{
	__super::Render();
}
