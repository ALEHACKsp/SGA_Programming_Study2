#include "stdafx.h"
#include "MeshSphere.h"

MeshSphere::MeshSphere()
	: GameModel(Materials + L"Meshes/", L"Sphere.material", 
		Models + L"Meshes/", L"Sphere.mesh", false )
{

}

MeshSphere::~MeshSphere()
{

}

void MeshSphere::Update()
{
	//GameModel::Update();
	// �̷��� �ᵵ �� �ٷ����� �θ� virtual ������ �� ����
	__super::Update(); 
}

void MeshSphere::Render()
{
	__super::Render();
}

void MeshSphere::PostRender()
{
	__super::PostRender();

	ImGui::Begin("Model Transform");

	D3DXVECTOR3 position = Position();
	D3DXVECTOR3 rotation = RotationDegree();
	D3DXVECTOR3 scale = Scale();
	if (ImGui::DragFloat3("Position", (float*)&position, 0.1f)) Position(position);
	if (ImGui::DragFloat3("Rotation", (float*)&rotation, 0.1f)) RotationDegree(rotation);
	if (ImGui::DragFloat3("Scale", (float*)&scale, 0.1f)) Scale(scale);

	ImGui::End();
}
