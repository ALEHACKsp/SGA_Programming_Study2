#include "stdafx.h"
#include "ExeHomework.h"

#include "../LandScape/Terrain.h"

#include "../Objects/MeshCube.h"
#include "../Objects/MeshPlane.h"

#include "../Renders/Line.h"

ExeHomework::ExeHomework(ExecuteValues * values)
	:Execute(values)
{
	// Create Terrain
	{
		models.push_back(new MeshPlane());

		D3DXMATRIX S;
		D3DXMatrixScaling(&S, 3.0f, 1, 3.0f);
		models.back()->RootAxis(S);
	}

	D3DXVECTOR3 min, max;
	models[0]->GetModel()->MinMaxVertex(&min, &max);
	D3DXVec3TransformCoord(&min, &min, &models[0]->Transformed());
	D3DXVec3TransformCoord(&max, &max, &models[0]->Transformed());

	// Create Cubes
	{
		for (int i = 0; i < 5; i++) {
			models.push_back(new MeshCube());
			models.back()->Position(
				D3DXVECTOR3(Math::Random(min.x, max.x),
					0.5f,
					Math::Random(min.z, max.z)));
		}
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

	ImGui::Begin("Model");
	for (UINT i=1; i<models.size(); i++)
	{
		GameModel * model = models[i];
		vector<Material*> materials = model->GetModel()->Materials();
		
		if (model->IsMouseCollision(
			values->MainCamera, values->Viewport, values->Perspective))
		{
			if (Mouse::Get()->Down(0))
				selectModel = i;
		}

		if (selectModel == i) 
		{
			for (Material* material : materials)
			{
				material->SetDiffuse(D3DXCOLOR(1, 0, 0, 1));
			}
			ImGui::Text(model->GetName().c_str());
			ImGui::TextColored(ImVec4(1, 1, 0, 0), "Transform");
			
			D3DXVECTOR3 scale, translation;
			D3DXQUATERNION rotation;
			D3DXMatrixDecompose(&scale, &rotation, &translation, &model->Transformed());

			D3DXVECTOR2 rootAxis = model->RootAxis();

			float t[] = { translation.x, translation.y, translation.z };
			float r[] = { rotation.x, rotation.y, rotation.z };
			float s[] = { scale.x, scale.y, scale.z };

			ImGui::Text("Position");
			if (ImGui::DragFloat3("  T X Y Z", t, 0.5f)) {
				model->Position(t);
			}

			ImGui::Text("Rotation");
			// 이 방법 현재 문제있음 좀더 구조 정리해서 회전 구현해보자
			if (ImGui::DragFloat3("  R X Y Z", r, 0.1f)) {
				D3DXMATRIX matRotate;
				D3DXMatrixRotationQuaternion(&matRotate,
					&D3DXQUATERNION(r[0], r[1], r[2], rotation.w));
				model->World(matRotate * model->World());
			}

			// 스케일 값 모델에서 따로 관리해줘야함 이렇게하면 문제있음
			ImGui::Text("Scale");
			if (ImGui::DragFloat3("  S X Y Z", s, 0.1f)) {
				D3DXMATRIX S;
				D3DXMatrixScaling(&S, s[0],s[1],s[2]);
				model->RootAxis(S);
			}
		}
		else
		{
			for (Material* material : materials)
			{
				material->SetDiffuse(D3DXCOLOR(0, 0, 0, 1));
			}
		}
	}
	ImGui::End();

}

void ExeHomework::ResizeScreen()
{
}

