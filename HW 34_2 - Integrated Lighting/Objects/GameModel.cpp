#include "stdafx.h"
#include "GameModel.h"


GameModel::GameModel(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile)
	: velocity(0,0,0), shader(NULL)
	, isRotate(false), targetPos(0, 0, 0), targetNum(0)
	, rotateSpeed(1.0f)
	, orbitDist(10)
	, deltaTime(0)
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

	D3DXMATRIX t = Transformed();
	// 부모부터 자식까지 쫙 해주는거
	model->CopyGlobalBoneTo(boneTransforms, t);

	if (isRotate) {
		// 자전
		{
			deltaTime += rotateSpeed * Time::Delta();
			if (deltaTime > D3DX_PI * 2)
				deltaTime = 0;

			D3DXVECTOR3 R = Rotation();
			R.y = deltaTime;

			Rotation(R);

		}
		// 공전
		{
			D3DXMATRIX matT, matR, matTargetT;
			D3DXVECTOR3 rotation;
			D3DXVECTOR3 finalPos = { 0, 0, 0 };
			D3DXMATRIX matFinal;

			D3DXMatrixIdentity(&matT);
			D3DXMatrixIdentity(&matTargetT);

			D3DXMatrixTranslation(&matT, 0, 0, -orbitDist);
			//D3DXMatrixTranslation(&matT, -orbitDist.x, -orbitDist.y, -orbitDist.z);
			D3DXMatrixTranslation(&matTargetT,
				targetPos.x, targetPos.y, targetPos.z);
			rotation = Rotation();
			D3DXMatrixRotationYawPitchRoll(&matR, rotation.y, rotation.x, rotation.z);

			matFinal = matT * matR;
			D3DXVec3TransformCoord(&finalPos, &finalPos, &matFinal);
			D3DXVec3TransformCoord(&finalPos, &finalPos, &matTargetT);

			Position(finalPos.x, finalPos.y, finalPos.z);
		}
	}
}

void GameModel::Render()
{
	model->Buffer()->Bones(&boneTransforms[0], boneTransforms.size());
	model->Buffer()->SetVSBuffer(2);

	for (ModelMesh* mesh : model->Meshes())
	{
		// bone index
		int index = mesh->ParentBoneIndex();

		renderBuffer->Data.Index = index;
		renderBuffer->SetVSBuffer(3);

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
