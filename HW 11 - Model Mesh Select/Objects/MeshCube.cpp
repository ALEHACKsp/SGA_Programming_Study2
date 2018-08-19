#include "stdafx.h"
#include "MeshCube.h"

MeshCube::MeshCube()
	: GameModel(Materials + L"Meshes/", L"Cube.material", 
		Models + L"Meshes/", L"Cube.mesh" )
{
	shader = new Shader(Shaders + L"Homework/Mesh.hlsl");
	for (Material* material : model->Materials()) {
		material->SetDiffuse(D3DXCOLOR(0, 0, 0, 1));
		material->SetShader(shader);
	}

	name = "Cube";
}

MeshCube::~MeshCube()
{
	SAFE_DELETE(shader);
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
