#include "stdafx.h"
#include "MeshObject.h"

MeshObject::MeshObject(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile, D3DXCOLOR color)
	: GameModel(matFolder, matFile, meshFolder, meshFile)
{
	shader = new Shader(Shaders + L"017_Sphere.hlsl");
	for (Material* material : model->Materials())
	{
		material->SetDiffuse(color);
		material->SetShader(shader);
	}
}

MeshObject::~MeshObject()
{
	SAFE_DELETE(shader)
}

void MeshObject::Update()
{
	__super::Update();
}

void MeshObject::Render()
{
	__super::Render();
}
