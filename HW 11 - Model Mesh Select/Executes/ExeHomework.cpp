#include "stdafx.h"
#include "ExeHomework.h"

#include "../LandScape/Terrain.h"

#include "../Objects/GameTank.h"

#include "../Objects/MeshCube.h"
#include "../Objects/MeshPlane.h"

#include "../Renders/Line.h"

ExeHomework::ExeHomework(ExecuteValues * values)
	:Execute(values)
{
	// Create Sphere
	//{
	//	for (int i = 0; i < 5; i++) {
	//		models.push_back(new MeshCube());

	//		models.back()->Position(D3DXVECTOR3(
	//			Math::Random(-20, 20), 0,
	//			Math::Random(-20, 20)));
	//	}
	//}

	// Create Tank
	{
		models.push_back(new GameTank(Models + L"Tank/", L"Tank.material"));
	}

	selectModel = -1;
}

ExeHomework::~ExeHomework()
{

	for (GameModel * model : models)
		SAFE_DELETE(model);
}

void ExeHomework::Update()
{
	for (GameModel * model : models)
		model->Update();
}

void ExeHomework::PreRender()
{

}

void ExeHomework::Render()
{
	for (GameModel * model : models)
		model->Render();
}

void ExeHomework::PostRender()
{
	//D3DXVECTOR3 picked;
	//terrain->Y(&picked);

	//ImGui::LabelText("Picked", "%.2f, %.2f, %.2f", 
	//	picked.x, picked.y, picked.z);

	// Model 정보
	ImGui::Begin("Model Info");
	for (UINT i=0; i<models.size(); i++)
	{
		GameModel * model = models[i];
		vector<Material*> materials = model->GetModel()->Materials();
		
		if (Keyboard::Get()->Press(VK_LSHIFT)
			&& Mouse::Get()->Down(0)) 
		{
			if (model->IsPickModelMesh(
				values->MainCamera, values->Viewport, values->Perspective,
				selectModelMesh))
			{
				selectModel = i;
				break;
			}
			else
				selectModel = -1;
		}

		if (selectModel == i) 
		{
			ImGui::TextColored(ImVec4(1, 1, 0, 1), 
				("Model : " + model->GetName()).c_str());
			ImGui::Text("Transform");
			
			D3DXVECTOR3 scale, translation;
			D3DXQUATERNION rotation;
			D3DXMatrixDecompose(&scale, &rotation, &translation, 
				&model->Transformed());

			float pitch, yaw, roll;
			Math::QuatToPitchYawRoll(rotation, pitch, yaw, roll);

			float t[] = { translation.x, translation.y, translation.z };
			float r[] = { yaw, pitch, roll };
			float s[] = { scale.x, scale.y, scale.z };

			D3DXMATRIX S, R, T;

			ImGui::Text("Position");
			ImGui::DragFloat3(" T X Y Z", t, 0.1f);
			D3DXMatrixTranslation(&T, t[0], t[1], t[2]);

			ImGui::Text("Rotation");
			ImGui::DragFloat3(" R X Y Z", r, 0.01f);
			D3DXMatrixRotationYawPitchRoll(&R, r[0], r[1], r[2]);

			ImGui::Text("Scale");
			ImGui::DragFloat3(" S X Y Z", s, 0.01f);
			D3DXMatrixScaling(&S, s[0],s[1],s[2]);

			model->World(S * R * T);
		}
	}
	ImGui::End();

	// Model의 Matrial 정보
	ImGui::Begin("Model Material Info");
	{
		if (selectModel != -1)
		{
			GameModel* model = models[selectModel];
			ImGui::TextColored(ImVec4(1, 1, 0, 1),
				(model->GetName() + " Materials").c_str());

			vector<Material*> materials = model->GetModel()->Materials();
			for (UINT i = 0; i < materials.size(); i++)
			{
				ImGui::Separator();
				if (model->GetModel()->MeshByName(selectModelMesh)->MaterialName()
					!= materials[i]->Name())
				{
					ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1),
						String::ToString(materials[i]->Name()).c_str());
				}
				else
				{
					ImGui::Text(String::ToString(materials[i]->Name()).c_str());
					D3DXCOLOR* color = materials[i]->GetDiffuse();
					float colorF[] = { color->r, color->g, color->b, color->a };
					ImGui::ColorEdit4("Color", colorF);
					color->r = colorF[0];
					color->g = colorF[1];
					color->b = colorF[2];
					color->a = colorF[3];
				}
			}
		}
	}
	ImGui::End();

	// Model의 Mesh 정보
	ImGui::Begin("Model Mesh Info");
	{
		if (selectModel != -1) {
			GameModel* model = models[selectModel];
			ImGui::TextColored(ImVec4(1, 1, 0, 1), 
				(model->GetName() + " Meshes").c_str());

			vector<wstring> meshNames = model->GetMeshNames();
			for (UINT i = 0; i < meshNames.size(); i++) {
				ImGui::Separator();
				if(meshNames[i] != selectModelMesh)
					ImGui::TextColored(ImVec4(0.5f,0.5f,0.5f,1),
						String::ToString(meshNames[i]).c_str());
				else
				{
					ImGui::Text(String::ToString(meshNames[i]).c_str());
					ModelBone* bone = model->GetModel()->BoneByName(selectModelMesh);
					D3DXMATRIX matWorld = bone->Local();

					D3DXVECTOR3 scale, translation;
					D3DXQUATERNION rotation;
					D3DXMatrixDecompose(&scale, &rotation, &translation,
						&matWorld);

					float pitch, yaw, roll;
					Math::QuatToPitchYawRoll(rotation, pitch, yaw, roll);

					float t[] = { translation.x, translation.y, translation.z };
					float r[] = { yaw, pitch, roll };
					float s[] = { scale.x, scale.y, scale.z };

					D3DXMATRIX S, R, T;

					ImGui::Text("Position");
					ImGui::DragFloat3(" T X Y Z", t, 0.5f);
					D3DXMatrixTranslation(&T, t[0], t[1], t[2]);

					ImGui::Text("Rotation");
					ImGui::DragFloat3(" R X Y Z", r, 0.01f);
					D3DXMatrixRotationYawPitchRoll(&R, r[0], r[1], r[2]);

					ImGui::Text("Scale");
					ImGui::DragFloat3(" S X Y Z", s, 0.01f);
					D3DXMatrixScaling(&S, s[0], s[1], s[2]);

					bone->Local(S * R * T);
				}
			}

			//D3DXVECTOR3 scale, translation;
			//D3DXQUATERNION rotation;
			//D3DXMatrixDecompose(&scale, &rotation, &translation, &matWorld);

			//float t[] = { translation.x, translation.y, translation.z };
			//float r[] = { rotation.x, rotation.y, rotation.z };
			//float s[] = { scale.x, scale.y, scale.z };

			//ImGui::Text(String::ToString(selectModelMesh).c_str());

			//ImGui::DragFloat3("Mesh Trans", t);
			////ImGui::DragFloat3("Mehs Trans", t);
			//ImGui::DragFloat3("Mesh Scale", s);

			//matWorld._41 = t[0];
			//matWorld._42 = t[1];
			//matWorld._43 = t[2];

			//matWorld._11 = s[0];
			//matWorld._22 = s[1];
			//matWorld._33 = s[2];

			//bone->Local(matWorld);
			//model->Update();
		}
	}
	ImGui::End();
}

void ExeHomework::ResizeScreen()
{
}

