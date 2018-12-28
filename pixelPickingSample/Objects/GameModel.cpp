#include "stdafx.h"
#include "GameModel.h"
#include "./Physics/Collider.h"
#include "./Utilities/Gizmo.h"


GameModel::GameModel(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile)
	: shader(NULL), collider(NULL), velocity(0, 0, 0)
	, bInstance(false)
{
	model = new Model();
	model->ReadMaterial(matFolder, matFile);
	model->ReadMesh(meshFolder, meshFile);

	gizmo = new Gizmo();
	gizmo->DrawGizmoDirection(this);
	gizmo->DrawGizmoRight(this);
	gizmo->DrawGizmoUp(this);
	gizmo->SetActive(false);
	
	renderBuffer = new RenderBuffer();
}

GameModel::~GameModel()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(renderBuffer);
	SAFE_DELETE(gizmo);
	SAFE_DELETE(model);
	SAFE_DELETE(collider);
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

	model->CopyGlobalBoneTo(boneTransforms, t);

	gizmo->Update();
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

	gizmo->Render();
}

void GameModel::DebugRender()
{
	ImGui::Begin("Object");
	{
		ImGui::Separator();
		ImGui::Checkbox("Show Gizmo", GetGizmo()->SetActive());
		if (*GetGizmo()->SetActive() == true)
		{
			
		}
		ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "Transform");
		{
			D3DXVECTOR3 val;

			val = Position();
			ImGui::Spacing(); ImGui::SameLine();
			if (ImGui::DragFloat3("Position", (float*)&val, 1.0f))
				Position(val);

			val = RotationDegree();
			ImGui::Spacing(); ImGui::SameLine();
			if (ImGui::DragFloat3("Rotation", (float*)&val, 1.0f))
				RotationDegree(val);

			ImGui::Spacing(); ImGui::SameLine();
			val = Scale();
			if (ImGui::DragFloat3("Scale", (float*)&val, 1.0f))
			{
				Scale(val);
			}
		}
		ImGui::Separator();
		ImGui::NewLine();

		ImGui::TextColored(ImVec4(1.0, 0.5f, 0.5f, 1.0f), "Texture");
		{
			ImGui::Separator();
			ImGui::NewLine();
			ImGui::SameLine(30);
			ImGui::Text("Diffuse Map");
			ImGui::SameLine(140);
			ImGui::Text("Specular Map");
			ImGui::SameLine(250);
			ImGui::Text("Normal Map");
			ImGui::NewLine();
			ImGui::SameLine(20);
			if (GetDiffuseMap() != NULL)
			{
				if (ImGui::ImageButton(GetDiffuseMap()->GetView(), ImVec2(84, 84)))
				{
					Path::OpenFileDialog(L"", L"PNG File(.png)\0*.png", Textures, bind(&GameModel::SetDiffuseMap, this, placeholders::_1));
				}
			}
			else
			{
				if (ImGui::Button("No Texture", ImVec2(90, 90)))
				{
					Path::OpenFileDialog(L"", L"PNG File(.png)\0*.png", Textures, bind(&GameModel::SetDiffuseMap, this, placeholders::_1));
				}
			}
			ImGui::SameLine(130);
			if (GetSpecularMap() != NULL)
			{
				if (ImGui::ImageButton(GetSpecularMap()->GetView(), ImVec2(84, 84)))
				{
					Path::OpenFileDialog(L"", L"PNG File(.png)\0*.png", Textures, bind(&GameModel::SetSpecularMap, this, placeholders::_1));
				}
			}
			else
			{
				if (ImGui::Button("No Texture", ImVec2(90, 90)))
				{
					Path::OpenFileDialog(L"", L"PNG File(.png)\0*.png", Textures, bind(&GameModel::SetSpecularMap, this, placeholders::_1));
				}
			}
			ImGui::SameLine(240);
			if (GetNormalMap() != NULL)
			{
				if (ImGui::ImageButton(GetNormalMap()->GetView(), ImVec2(84, 84)))
				{
					Path::OpenFileDialog(L"", L"PNG File(.png)\0*.png", Textures, bind(&GameModel::SetNormalMap, this, placeholders::_1));
				}
			}
			else
			{
				if (ImGui::Button("No Texture", ImVec2(90, 90)))
				{
					Path::OpenFileDialog(L"", L"PNG File(.png)\0*.png", Textures, bind(&GameModel::SetNormalMap, this, placeholders::_1));
				}
			}

			if (ImGui::ColorEdit3("Diffuse Color", (float*)GetDiffuse()))
				SetDiffuse(*GetDiffuse());

			if (ImGui::ColorEdit3("Specular Color", (float*)GetSpecular()))
				SetSpecular(*GetSpecular());

			if (ImGui::SliderFloat("Specular Shininess", (float*)GetShininess(), 0.1f, 20.0f))
				SetShininess(*GetShininess());
			ImGui::Separator();
			ImGui::NewLine();
		}
		ImGui::Separator();
	}
	ImGui::End();
}

void GameModel::BoundingBox(D3DXVECTOR3 * min, D3DXVECTOR3 * max)
{
	model->BoundingBox(min, max);
}

void GameModel::SetShader(Shader * shader)
{
	for (Material* material : model->Materials())
		material->SetShader(shader);
}

void GameModel::SetShader(wstring file, string vsName, string psName)
{
	for (Material* material : model->Materials())
		material->SetShader(file, vsName, psName);
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

D3DXCOLOR * GameModel::GetDiffuse(UINT index)
{
	return model->Materials()[index]->GetDiffuse();
}

void GameModel::SetDiffuseMap(wstring file)
{
	for (Material* material : model->Materials())
		material->SetDiffuseMap(file);
}

Texture * GameModel::GetDiffuseMap(UINT index)
{
	return model->Materials()[index]->GetDiffuseMap();
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

D3DXCOLOR * GameModel::GetSpecular(UINT index)
{
	return model->Materials()[index]->GetSpecular();
}

void GameModel::SetSpecularMap(wstring file)
{
	for (Material* material : model->Materials())
		material->SetSpecularMap(file);
}

Texture * GameModel::GetSpecularMap(UINT index)
{
	return model->Materials()[index]->GetSpecularMap();
}

void GameModel::SetNormalMap(wstring file)
{
	for (Material* material : model->Materials())
		material->SetNormalMap(file);
}

Texture * GameModel::GetNormalMap(UINT index)
{
	return model->Materials()[index]->GetNormalMap();
}

void GameModel::SetShininess(float val)
{
	for (Material* material : model->Materials())
		material->SetShininess(val);
}

float* GameModel::GetShininess(UINT index)
{
	return model->Materials()[index]->GetShininess();
}

void GameModel::SetInstance(ID3D11Buffer * instanceBuffer, UINT instanceCount)
{
	bInstance = true;

	model->SetInstance(instanceBuffer, instanceCount);
}

void GameModel::SetInstanceBuffer(vector<vector<D3DXMATRIX>>& instanceWorlds)
{
	vector<D3DXMATRIX> tempInstance;
	tempInstance.resize(instanceWorlds.size());
	for (UINT i = 0; i < instanceWorlds[0].size(); i++) {
		for (UINT j = 0; j < instanceWorlds.size(); j++)
			tempInstance[j] = instanceWorlds[j][i];

		// test
		//for (int j = 0; j < boneTransforms.size(); j++)	
		//	tempInstance[i] = boneTransforms[j];

		model->SetInstanceBuffer(tempInstance, i);
	}
}

void GameModel::GetBoneTransformByName(wstring name, D3DXMATRIX * transformOut)
{
	model->CopyGlobalBoneToName(name, transformOut);
	*transformOut = (*transformOut) * Transformed();
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
