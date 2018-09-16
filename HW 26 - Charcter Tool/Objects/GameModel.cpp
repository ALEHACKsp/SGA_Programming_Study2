#include "stdafx.h"
#include "GameModel.h"

#include "../Renders/Lines/DebugDrawSphere.h"
#include "../Renders/Lines/DebugDrawSphere2.h"

GameModel::GameModel(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile)
	: velocity(0,0,0), shader(NULL)
{
	model = new Model();
	model->ReadMaterial(matFolder, matFile);
	model->ReadMesh(meshFolder, meshFile);

	renderBuffer = new RenderBuffer();

	D3DXVECTOR3 center = { 0,0,0 };
	float radius = 1.0f;
	debug = new DebugDrawSphere2(center, radius);

	showBoneDebug = false;
	depthMode[0] = new DepthStencilState();
	depthMode[1] = new DepthStencilState();
	depthMode[1]->DepthEnable(false);

	//BuildBoneTree();

	initBoneTransforms.assign(model->Bones().size(), D3DXMATRIX());

	for (int i = 0; i < model->BoneCount(); i++) {
		ModelBone* bone = model->BoneByIndex(i);
		if (bone->Name().find(L"Hips") != wstring::npos)
			selectBone = i;
		initBoneTransforms[i] = bone->Local();
	}

	isAnimMode = false;
}

GameModel::~GameModel()
{
	SAFE_DELETE(renderBuffer);

	SAFE_DELETE(shader);

	SAFE_DELETE(model);

	SAFE_DELETE(debug);

	SAFE_DELETE(depthMode[0]);
	SAFE_DELETE(depthMode[1]);
}	

void GameModel::Velocity(D3DXVECTOR3 & vec)
{
	velocity = vec;
}

D3DXVECTOR3 GameModel::Velocity()
{
	return velocity;
}

//void GameModel::Scale(float x, float y, float z)
//{
//	Scale(D3DXVECTOR3(x, y, z));
//}
//
//void GameModel::Scale(D3DXVECTOR3 vec)
//{
//	model->Buffer()->Scale(vec);
//}

void GameModel::Update()
{
	CalcPosition();

	D3DXMATRIX t = Transformed();
	// �θ���� �ڽı��� �� ���ִ°�
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

	if (showBoneDebug) {
		depthMode[1]->OMSetDepthStencilState();
		for (int i = 0; i < boneTransforms.size(); i++) {
			ModelBone* bone = model->BoneByIndex(i);
			D3DXMATRIX matrix = bone->Global() * boneTransforms[i];
			D3DXVECTOR3 center = {
				matrix._41, matrix._42, matrix._43 };
			if (selectBone == i)
				debug->SetColor(D3DXCOLOR(255, 0, 0, 255));
			else
				debug->SetColor(D3DXCOLOR(255, 255, 255, 255));

			debug->Position(center);

			debug->Render();
		}
		depthMode[0]->OMSetDepthStencilState();
	}
}

void GameModel::PostRender()
{
	ImGui::Checkbox("Show Bone Debug", &showBoneDebug);
	if (showBoneDebug) {
		float radius = debug->Scale().x;
		if (ImGui::InputFloat("Debug Radius", &radius))
			debug->Scale(D3DXVECTOR3(radius, radius, radius));
	}
	
	ImGui::Separator();

	if (isAnimMode == false) {
		Model* model = GetModel();
		for (int i = 0; i < model->BoneCount(); i++) {
			ModelBone* bone = model->BoneByIndex(i);

			if (i == selectBone && boneTransforms.size() != 0) {
				string name = String::ToString(bone->Name());
				ImGui::Text(("Name : " + name).c_str());

				D3DXMATRIX matrix = bone->Local();

				D3DXVECTOR3 scale, translation;
				D3DXQUATERNION rotation;
				D3DXMatrixDecompose(&scale, &rotation, &translation,
					&matrix);

				float pitch, yaw, roll;
				Math::QuatToPitchYawRoll(rotation, pitch, yaw, roll);
				float r[] = { yaw, pitch, roll };

				D3DXMATRIX S, R, T;

				ImGui::Text("Position");
				ImGui::DragFloat3("T", (float*)translation, 0.5f);
				D3DXMatrixTranslation(&T, translation.x, translation.y, translation.z);

				ImGui::Text("Rotation");
				ImGui::DragFloat3("R", r, 0.01f);
				D3DXMatrixRotationYawPitchRoll(&R, r[0], r[1], r[2]);

				ImGui::Text("Scale");
				ImGui::DragFloat3("S", (float*)scale, 0.01f);
				D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);

				ImGui::Text("Scaling");
				if (ImGui::InputFloat("s", &scale.x)) {
					if (scale.x <= 0) scale.x = 0.01f;
					D3DXMatrixScaling(&S, scale.x, scale.x, scale.x);
				}

				bone->Local(S * R * T);
			}
		}
	}
}

void GameModel::BoneTree(int boneIndex)
{
	Model* model = GetModel();

	for (int i = 0; i < model->BoneCount(); i++) {

		ModelBone* bone = model->BoneByIndex(i);

		if (boneIndex == bone->ParentIndex()) {
			ImGui::Dummy(ImVec2(0, 20));
			if (i == selectBone) {
				ImVec2 min, max;
				min = ImGui::GetItemRectMin();
				max = ImGui::GetItemRectMax();
				max.x = ImGui::GetWindowPos().x + ImGui::GetWindowWidth() - 20;
				ImGui::GetWindowDrawList()->AddRectFilled(min, max, IM_COL32(0, 0, 255, 128));
			}

			ImGui::SameLine(0, 0);
			if (ImGui::TreeNode(String::ToString(bone->Name()).c_str()))
			{
				if (ImGui::IsItemClicked(1))
					selectBone = i;

				BoneTree(i);

				ImGui::TreePop();
			}
		}

	}
}

void GameModel::TreeRender(bool isSelect)
{
	Model* model = GetModel();
	ImGui::Dummy(ImVec2(0, 20));
	if (isSelect) {
		ImVec2 min, max;
		min = ImGui::GetItemRectMin();
		max = ImGui::GetItemRectMax();
		max.x = ImGui::GetWindowPos().x + ImGui::GetWindowWidth() - 20;
		ImGui::GetWindowDrawList()->AddRectFilled(min, max, IM_COL32(255, 255, 0, 128));
	}

	ImGui::SameLine(0, 0);
	if (ImGui::TreeNode(Name().c_str()))
	{
		BoneTree(-1);

		ImGui::TreePop();
	}
}

void GameModel::CheckPickMesh(D3DXVECTOR3 & start, D3DXVECTOR3 & dir)
{
	// InvWorld
	{
		D3DXMATRIX invWorld;
		D3DXMatrixInverse(&invWorld, NULL, &Transformed());

		D3DXVec3TransformCoord(&start, &start, &invWorld);
		D3DXVec3TransformNormal(&dir, &dir, &invWorld);
		D3DXVec3Normalize(&dir, &dir);
	}

	bool check = false;
	for (int i = 0; i < boneTransforms.size(); i++) {
		ModelBone* bone = model->BoneByIndex(i);
		D3DXMATRIX matrix = bone->Global() * boneTransforms[i];
		
		D3DXVECTOR3 center = {
			matrix._41, matrix._42, matrix._43 };
		float radius = debug->Scale().x;

		// ��å 355p
		// ��� ��ü�� ���� �浹 �������� ����
		// �׽�Ʈ�� ��
		D3DXVECTOR3 v = start - center;

		float b = 2.0f * D3DXVec3Dot(&dir, &v);
		float c = D3DXVec3Dot(&v, &v) - radius * radius;

		// �Ǻ��� ã��
		float discriminant = (b*b) - (4.0f * c);

		// ������ ���� ���� �׽�Ʈ
		if (discriminant < 0.0f) {
			continue;
		}

		discriminant = sqrtf(discriminant);

		float s0 = (-b + discriminant) / 2.0f;
		float s1 = (-b - discriminant) / 2.0f;

		// �ذ� >= 0�� ��� ��ü ����
		if (s0 >= 0.0f || s1 >= 0.0f) {
			selectBone = i;
			check = true;
			break;
		}
	}

	if (check == false)
		selectBone = -1;
}

void GameModel::ResetBoneLocal()
{
	for (int i = 0; i < model->BoneCount(); i++) {
		ModelBone* bone = model->BoneByIndex(i);
		bone->Local(initBoneTransforms[i]);
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

//bool cmp(ModelBone* b1, ModelBone* b2) {
//	return b1->ParentIndex() < b2->ParentIndex();
//}
//
//void GameModel::BuildBoneTree()
//{
//	sort(model->Bones().begin(), model->Bones().end(), cmp);
//}

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