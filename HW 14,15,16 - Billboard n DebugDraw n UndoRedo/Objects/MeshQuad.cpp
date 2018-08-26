#include "stdafx.h"
#include "MeshQuad.h"

MeshQuad::MeshQuad()
	: GameModel(Materials + L"Meshes/", L"Quad.material", 
		Models + L"Meshes/", L"Quad.mesh" )
{
	shader = new Shader(Shaders + L"017_Billboard.hlsl");
	for (Material* material : model->Materials()) {
		material->SetShader(shader);
	}

	name = "Quad";
}

MeshQuad::~MeshQuad()
{
	SAFE_DELETE(shader);
}

void MeshQuad::Update()
{
	//GameModel::Update();
	// �̷��� �ᵵ �� �ٷ����� �θ� virtual ������ �� ����
	__super::Update(); 
}

void MeshQuad::Render()
{
	__super::Render();
}
