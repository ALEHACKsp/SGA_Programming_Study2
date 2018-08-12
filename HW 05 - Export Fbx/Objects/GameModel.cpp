#include "stdafx.h"
#include "GameModel.h"

GameModel::GameModel(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile)
	: velocity(0,0,0)
{
	model = new Model();
	model->ReadMaterial(matFolder, matFile);
	model->ReadMesh(meshFolder, meshFile);

	shader = new Shader(Shaders + L"009_Model.hlsl");
	for (Material* material : model->Materials())
		material->SetShader(shader);

	D3DXMatrixIdentity(&matRotation);

	renderBuffer = new RenderBuffer();

	//turretBone = model->BoneByName(L"turret_geo");
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

	//D3DXMATRIX R;
	//D3DXMatrixRotationY(&R, sinf(Time::Get()->Running() * 2.0f) *0.25f);
	//
	//D3DXMATRIX L = turretBone->Local();
	//turretBone->Local(L * R);

	D3DXMATRIX t = Transformed();
	model->CopyGlobalBoneTo(boneTransforms, t);
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

void GameModel::Rotate(D3DXVECTOR2 amount)
{
	amount *= Time::Delta();

	// x, y 뒤집어둠 카메라 움직임때랑 같은 이유
	D3DXMATRIX axis;
	D3DXMatrixRotationAxis(&axis, &Right(), amount.y);

	D3DXMATRIX Y;
	D3DXMatrixRotationY(&Y, Math::ToRadian(amount.x));

	matRotation = axis * Y;

	D3DXMATRIX R = World();
	World(matRotation * R);
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
