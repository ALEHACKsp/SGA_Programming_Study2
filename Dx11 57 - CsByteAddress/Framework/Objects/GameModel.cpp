#include "Framework.h"
#include "GameModel.h"

#include "../Model/ModelMeshPart.h"

GameModel::GameModel(wstring shaderFile, wstring matFile, wstring meshFile)
{
	model = new Model();
	model->ReadMaterial(matFile);
	model->ReadMesh(meshFile);

	model->CopyGlobalBoneTo(transforms);

	SetShader(shaderFile);
}

GameModel::~GameModel()
{
	SAFE_DELETE(model);
}

void GameModel::Update()
{
	
}

void GameModel::PreRender()
{
	for (Material* material : model->Materials())
	{
		const float* data = transforms[0];
		UINT count = transforms.size();

		material->GetShader()->AsMatrix("Bones")->SetMatrixArray(data, 0, count);
	}

	for (ModelMesh* mesh : model->Meshes())
		mesh->PreRender();
}

void GameModel::Render()
{
	for (Material* material : model->Materials())
	{
		const float* data = transforms[0];
		UINT count = transforms.size();

		material->GetShader()->AsMatrix("Bones")->SetMatrixArray(data, 0, count);
	}

	for (ModelMesh* mesh : model->Meshes())
		mesh->Render();
}

#pragma region Material
void GameModel::SetShader(wstring shaderFiler)
{
	for (Material* material : model->Materials())
		material->SetShader(shaderFiler);
}

void GameModel::SetDiffuse(float r, float g, float b, float a)
{
	for (Material* material : model->Materials())
		material->SetDiffuse(D3DXCOLOR(r, g, b, a));
}

void GameModel::SetDiffuse(D3DXCOLOR & color)
{
	for (Material* material : model->Materials())
		material->SetDiffuse(color);
}

void GameModel::SetDiffuseMap(wstring file)
{
	for (Material* material : model->Materials())
		material->SetDiffuseMap(file);
}

void GameModel::SetSpecular(float r, float g, float b, float a)
{
	for (Material* material : model->Materials())
		material->SetSpecular(D3DXCOLOR(r, g, b, a));
}

void GameModel::SetSpecular(D3DXCOLOR & color)
{
	for (Material* material : model->Materials())
		material->SetSpecular(color);
}

void GameModel::SetSpecularMap(wstring file)
{
	for (Material* material : model->Materials())
		material->SetSpecularMap(file);
}

void GameModel::SetNormalMap(wstring file)
{
	for (Material* material : model->Materials())
		material->SetNormalMap(file);
}

void GameModel::SetShininess(float val)
{
	for (Material* material : model->Materials())
		material->SetShininess(val);
}
#pragma endregion

void GameModel::UpdateWorld()
{
	__super::UpdateWorld();

	D3DXMATRIX W = World();
	model->CopyGlobalBoneTo(transforms, W);
}
