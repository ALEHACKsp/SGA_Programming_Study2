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

bool GameModel::IsMultiPickModel(D3DXVECTOR3 start, D3DXVECTOR3 dir)
{
	// InvWorld
	{
		D3DXMATRIX invWorld;
		D3DXMatrixInverse(&invWorld, NULL, &Transformed());

		D3DXVec3TransformCoord(&start, &start, &invWorld);
		D3DXVec3TransformNormal(&dir, &dir, &invWorld);
		D3DXVec3Normalize(&dir, &dir);
	}

	//float u, v, distance;

	//return model->IsPick(start, dir, u, v, distance);

	// 용책 355p
	// 경계 구체에 대한 충돌 판정으로 변경
	// 테스트용 구
	float radius = 0.5f;
	D3DXVECTOR3 center = { 0,0,0 };

	D3DXVECTOR3 v = start - center;

	float b = 2.0f * D3DXVec3Dot(&dir, &v);
	float c = D3DXVec3Dot(&v, &v) - radius * radius;

	// 판별식 찾기
	float discriminant = (b*b) - (4.0f * c);

	// 가상의 수에 대한 테스트
	if (discriminant < 0.0f)
		return false;

	discriminant = sqrtf(discriminant);

	float s0 = (-b + discriminant) / 2.0f;
	float s1 = (-b - discriminant) / 2.0f;

	// 해가 >= 0일 경우 구체 교차
	if (s0 >= 0.0f || s1 >= 0.0f)
		return true;

	return false;
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

	//float u, v, distance;

	//return model->IsPick(start, direction, u, v, distance);

	// 용책 355p
	// 경계 구체에 대한 충돌 판정으로 변경
	// 테스트용 구
	float radius = 0.5f;
	D3DXVECTOR3 center = { 0,0,0 };

	D3DXVECTOR3 v = start - center;

	float b = 2.0f * D3DXVec3Dot(&direction, &v);
	float c = D3DXVec3Dot(&v, &v) - radius * radius;

	// 판별식 찾기
	float discriminant = (b*b) - (4.0f * c);

	// 가상의 수에 대한 테스트
	if (discriminant < 0.0f)
		return false;

	discriminant = sqrtf(discriminant);

	float s0 = (-b + discriminant) / 2.0f;
	float s1 = (-b - discriminant) / 2.0f;

	// 해가 >= 0일 경우 구체 교차
	if (s0 >= 0.0f || s1 >= 0.0f)
		return true;

	return false;
}
