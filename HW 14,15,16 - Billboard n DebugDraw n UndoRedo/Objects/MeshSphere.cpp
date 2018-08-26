#include "stdafx.h"
#include "MeshSphere.h"

MeshSphere::MeshSphere()
	: GameModel(Materials + L"Meshes/", L"Sphere.material", 
		Models + L"Meshes/", L"Sphere.mesh" )
{
	shader = new Shader(Shaders + L"012_Sphere.hlsl");
	for (Material* material : model->Materials()) {
		material->SetDiffuse(D3DXCOLOR(1, 0, 0, 1));
		material->SetShader(shader);
	}

	name = "Sphere";
}

MeshSphere::~MeshSphere()
{
	SAFE_DELETE(shader);
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
