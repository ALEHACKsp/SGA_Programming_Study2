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
	// 이렇게 써도 됨 바로위에 부모 virtual 없으면 더 위로
	__super::Update(); 
}

void MeshPlane::Render()
{
	__super::Render();
}
