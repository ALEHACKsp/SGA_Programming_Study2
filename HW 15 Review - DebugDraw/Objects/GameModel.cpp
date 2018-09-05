#include "stdafx.h"
#include "GameModel.h"

GameModel::GameModel(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile)
	: velocity(0,0,0), shader(NULL)
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

bool GameModel::IsPickModel(Camera * camera, Viewport * viewport, Perspective * perspective)
{

	D3DXVECTOR3 start;
	camera->Position(&start);
	D3DXVECTOR3 direction = camera->Direction(viewport, perspective);

	// InvWorld
	{
		D3DXMATRIX invWorld;
		D3DXMatrixInverse(&invWorld, NULL, &Transformed());

		D3DXVec3TransformCoord(&start, &start, &invWorld);
		D3DXVec3TransformNormal(&direction, &direction, &invWorld);
		D3DXVec3Normalize(&direction, &direction);
	}

	float u, v, distance;

	return model->IsPick(start, direction, u, v, distance);
}

bool GameModel::IsPickModelMesh(Camera * camera, Viewport * viewport,
	Perspective * perspective, OUT wstring& meshName)
{
	D3DXVECTOR3 start;
	camera->Position(&start);
	D3DXVECTOR3 direction = camera->Direction(viewport, perspective);

	//// InvWorld
	//{
	//	D3DXMATRIX invWorld;
	//	D3DXMatrixInverse(&invWorld, NULL, &Transformed());

	//	D3DXVec3TransformCoord(&start, &start, &invWorld);
	//	D3DXVec3TransformNormal(&direction, &direction, &invWorld);
	//	D3DXVec3Normalize(&direction, &direction);
	//}

	float u, v, distance;
	distance = FLT_MAX;

	bool result = model->IsPickMesh(start, direction,
		u, v, distance, meshName, boneTransforms);

	return result;
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
