#include "stdafx.h"
#include "GameModel.h"

GameModel::GameModel(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile)
	: velocity(0, 0, 0), shader(NULL)
	, isOrbit(false), targetPos(0,0,0), targetNum(0)
	, rotateSpeed(1.0f)
	, orbitDist(10)
	//, rotateSpeed(0,1,0)
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

void GameModel::PostRender()
{
	ImGui::Text(name.c_str());
	ImGui::Separator();

	D3DXVECTOR3 S, R, T;
	S = Scale();
	R = Rotation();
	T = Position();

	ImGui::Text("Transform");

	ImGui::Text("Position");
	ImGui::SameLine(80);
	ImGui::DragFloat3("T", (float*)&T, 0.1f);

	ImGui::Text("Rotation");
	ImGui::SameLine(80);
	ImGui::DragFloat3("R", (float*)&R, 0.1f);

	ImGui::Text("Scale");
	ImGui::SameLine(80);
	ImGui::DragFloat3("S", (float*)&S, 0.1f);

	Scale(S);
	Rotation(R);
	Position(T);

	float scale = Scale().x;
	ImGui::Text("Scale2");
	ImGui::SameLine(80);
	if (ImGui::InputFloat("S", &scale))
	{
		S = Scale();
		S.x = S.y = S.z = scale;
		Scale(S);
	}

	ImGui::DragFloat("Rotate Speed", &rotateSpeed, 0.05f, 1.0f, 10.0f);
	//ImGui::DragFloat3("Rotate Speed", (float*)&rotateSpeed, 0.05f, 0.0f, 10.0f);
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

void GameModel::DeleteDiffuseMap()
{
	for (Material* material : model->Materials())
		material->DeleteDiffuseMap();
}

void GameModel::DeleteSpecularMap()
{
	for (Material* material : model->Materials())
		material->DeleteSpecularMap();
}

void GameModel::DeleteNormalMap()
{
	for (Material* material : model->Materials())
		material->DeleteNormalMap();
}

void GameModel::SetShininess(float val)
{
	for (Material* material : model->Materials())
		material->SetShininess(val);
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
