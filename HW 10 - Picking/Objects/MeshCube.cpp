#include "stdafx.h"
#include "MeshCube.h"

#include "../Physics/RectCollider.h"

MeshCube::MeshCube()
	: GameModel(Materials + L"Meshes/", L"Cube.material", 
		Models + L"Meshes/", L"Cube.mesh" )
{
	shader = new Shader(Shaders + L"Homework/Mesh.hlsl");
	for (Material* material : model->Materials()) {
		material->SetDiffuse(D3DXCOLOR(0, 0, 0, 1));
		material->SetShader(shader);
	}

	// Collider Setting
	{
		collider = new RectCollider();
		this->colliderType = COLLIDER_TYPE_RECT;

		D3DXVECTOR3 min, max;
		model->MinMaxVertex(&min, &max);
		D3DXVECTOR3 center = (min + max) / 2.0f;

		D3DXVECTOR3 halfSize = (max - min) / 2.0f;

		collider->SetBound(&center, &halfSize);
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
	// 이렇게 써도 됨 바로위에 부모 virtual 없으면 더 위로
	__super::Update(); 
}

void MeshCube::Render()
{
	__super::Render();
}
