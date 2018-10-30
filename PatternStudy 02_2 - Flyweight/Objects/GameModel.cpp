#include "stdafx.h"
#include "GameModel.h"


GameModel::GameModel(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile)
	: velocity(0,0,0), shader(NULL)
	,bAutoRotate(false), bInstance(false)
{
	model = new Model();
	model->ReadMaterial(matFolder, matFile);
	model->ReadMesh(meshFolder, meshFile);

	renderBuffer = new RenderBuffer();
}

GameModel::~GameModel()
{
	SAFE_DELETE(renderBuffer);

	SAFE_DELETE(shader);

	SAFE_DELETE(model);
}

void GameModel::Velocity(D3DXVECTOR3 & vec)
{
	velocity = vec;
}

D3DXVECTOR3 GameModel::Velocity()
{
	return velocity;
}

void GameModel::Update()
{
	CalcPosition();

	if (bAutoRotate)
		AutoRotate();

	D3DXMATRIX t = Transformed();
	// 부모부터 자식까지 쫙 해주는거
	model->CopyGlobalBoneTo(boneTransforms, t);
}

void GameModel::Render()
{
	if (bInstance == false) {
		model->Buffer()->Bones(&boneTransforms[0], boneTransforms.size());
		model->Buffer()->SetVSBuffer(2);
	}

	for (ModelMesh* mesh : model->Meshes())
	{
		// bone index
		int index = mesh->ParentBoneIndex();

		if (bInstance == false) {
			renderBuffer->Data.Index = index;
			renderBuffer->SetVSBuffer(3);
		}
		else {
			model->InstanceBuffer(index)->SetVSBuffer(4);
		}

		mesh->Render();
	}
}

void GameModel::SetShader(Shader * shader)
{
	for (Material* material : model->Materials())
		material->SetShader(shader);
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

void GameModel::SetInstance(ID3D11Buffer * instanceBuffer, UINT instanceCount)
{
	bInstance = true;

	model->SetInstance(instanceBuffer, instanceCount);
}

void GameModel::SetInstanceBuffer(vector< vector<D3DXMATRIX> >& instanceWorlds)
{
	vector<D3DXMATRIX> tempInstance;
	tempInstance.resize(instanceWorlds.size());
	for (int i = 0; i < instanceWorlds[0].size(); i++) {
		for (int j = 0; j < instanceWorlds.size(); j++)
			tempInstance[j] = instanceWorlds[j][i];

		// test
		//for (int j = 0; j < boneTransforms.size(); j++)	
		//	tempInstance[i] = boneTransforms[j];

		model->SetInstanceBuffer(tempInstance, i);
	}
}

void GameModel::CalcPosition()
{
	if (D3DXVec3Length(&velocity) <= 0.0f)
		return;

	D3DXVECTOR3 vec(0, 0, 0);
	if (velocity.z != 0.0f)
		vec += Direction() * velocity.z;

	if (velocity.y != 0.0f)
		vec += Up() * velocity.y;

	if (velocity.x != 0.0f)
		vec += Right() * velocity.x;

	D3DXVECTOR3 pos = Position() + vec * Time::Delta();
	Position(pos);
}

void GameModel::AutoRotate()
{
	if (bAutoRotate) {
		D3DXVECTOR3 rot = Rotation();
		rot.y += Time::Delta();
		Rotation(rot);
	}
}
