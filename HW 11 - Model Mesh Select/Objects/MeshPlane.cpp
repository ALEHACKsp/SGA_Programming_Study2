#include "stdafx.h"
#include "MeshPlane.h"

MeshPlane::MeshPlane()
	: GameModel(Materials + L"Meshes/", L"Plane.material", 
		Models + L"Meshes/", L"Plane.mesh" )
{
	shader = new Shader(Shaders + L"Homework/Mesh.hlsl");
	for (Material* material : model->Materials()) {
		material->SetDiffuse(D3DXCOLOR(1, 1, 1, 1));
		material->SetShader(shader);
	}
}

MeshPlane::~MeshPlane()
{
	SAFE_DELETE(shader);
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
