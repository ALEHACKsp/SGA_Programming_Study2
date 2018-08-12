#include "stdafx.h"
#include "ExeHomework.h"

#include "../Fbx/Exporter.h"

#include "../Objects/GameModel.h"

ExeHomework::ExeHomework(ExecuteValues * values)
	:Execute(values)
{
	// Model Add
	values->GlobalModel->AddModel(L"Capsule", Models + L"Meshes/Capsule/", 
		L"Capsule.material", L"Capsule.mesh");
	values->GlobalModel->AddModel(L"Cube", Models + L"Meshes/Cube/",
		L"Cube.material", L"Cube.mesh");
	values->GlobalModel->AddModel(L"Cylinder", Models + L"Meshes/Cylinder/",
		L"Cylinder.material", L"Cylinder.mesh");
	values->GlobalModel->AddModel(L"Sphere", Models + L"Meshes/Sphere/",
		L"Sphere.material", L"Sphere.mesh");
	values->GlobalModel->AddModel(L"Teapot", Models + L"Meshes/Teapot/",
		L"Teapot.material", L"Teapot.mesh");
	values->GlobalModel->AddModel(L"Plane", Models + L"Meshes/Plane/",
		L"Plane.material", L"Plane.mesh");
	values->GlobalModel->AddModel(L"Quad", Models + L"Meshes/Quad/",
		L"Quad.material", L"Quad.mesh");

	values->GlobalModel->AddModel(L"Tank", Models + L"Tank/",
		L"Tank.material", L"Tank.mesh");

	// Plane
	models.push_back(new GameModel(L"Plane", values->GlobalModel->GetModel(L"Plane"), 
		values, true));
	vector<Material*> mats = models.back()->GetModel()->Materials();
	for (Material* mat : mats) {
		mat->SetDiffuseMap(Textures + L"Dirt.png");
	}
	
	models[0]->Position(D3DXVECTOR3(0, -0.1f, 0));
	models[0]->Scale(D3DXVECTOR3(3.0f, 1.0f, 3.0f));

	// Tank
	models.push_back(new GameModel(L"Tank", values->GlobalModel->GetModel(L"Tank"),
		values, true, true));

	D3DXVECTOR3 min, max;
	models[0]->GetModel()->MinMaxVertex(&min, &max);
	D3DXVECTOR3 scale = models[0]->Scale();
	min.x *= scale.x;
	min.y *= scale.y;
	min.z *= scale.z;
	models[1]->Position(min);
	models[1]->Scale(D3DXVECTOR3(0.5f, 0.5f, 0.5f));

	// Sphere 구 충돌 테스트용
	models.push_back(new GameModel(L"Sphere", values->GlobalModel->GetModel(L"Sphere"),
		values, false, false));
	max.x *= scale.x;
	max.y *= scale.y;
	max.z *= scale.z;
	models[2]->Position(max);

	// Sphere 2 구 충돌 테스트용 2 충돌 시 이넘 파괴하게 할 꺼
	models.push_back(new GameModel(L"Sphere", values->GlobalModel->GetModel(L"Sphere"),
		values, false, false));
	mats = models[3]->GetModel()->Materials();
	for (Material* mat : mats) {
		mat->SetDiffuseMap(Textures + L"Box.png");
	}
	models[3]->Position(max - D3DXVECTOR3(3,0,0));
}

ExeHomework::~ExeHomework()
{
	for (int i = 0; i < models.size(); i++)
		SAFE_DELETE(models[i]);
}

void ExeHomework::Update()
{
	for (int i = 0; i < models.size(); i++)
		models[i]->Update();

	vector<GameModel* >::iterator iter = models.begin() + 3;
	for (; iter != models.end();)
	{
		if ((*iter)->GetIsRectCollider())
		{
			if (Collision::IsOverlapRectRect(
				&models[1]->Transformed(), (RectCollider*)models[1]->Collider(),
				&(*iter)->Transformed(), (RectCollider*)(*iter)->Collider()))
			{
				SAFE_DELETE(*iter);
				iter = models.erase(iter);
			}
			else if (Collision::IsOverlapRectCircle(
				&(*iter)->Transformed(), (RectCollider*)(*iter)->Collider(),
				&models[2]->Transformed(), (CircleCollider*)models[2]->Collider()))
			{
				SAFE_DELETE(*iter);
				iter = models.erase(iter);
			}
			else
				iter++;
		}
		else
		{
			if (Collision::IsOverlapRectCircle(
				&models[1]->Transformed(), (RectCollider*)models[1]->Collider(),
				&(*iter)->Transformed(), (CircleCollider*)(*iter)->Collider()))
			{
				SAFE_DELETE(*iter);
				iter = models.erase(iter);
			}
			else if (Collision::IsOverlapCircleCircle(
				&models[2]->Transformed(), (CircleCollider*)models[2]->Collider(),
				&(*iter)->Transformed(), (CircleCollider*)(*iter)->Collider()))
			{
				SAFE_DELETE(*iter);
				iter = models.erase(iter);
			}
			else
				iter++;
		}
	}
}

void ExeHomework::PreRender()
{
	
}

void ExeHomework::Render()
{
	for (int i = 0; i < models.size(); i++)
		models[i]->Render();
}

void ExeHomework::PostRender()
{
	D3DXVECTOR3 min, max;
	if (models.size() > 0) {
		models[0]->GetModel()->MinMaxVertex(&min, &max);
	}

	ImGui::Begin("Model", 0, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Capsule", "Create Capsule Model"))
			{
				models.push_back(new GameModel(
					L"Capsule",
					values->GlobalModel->GetModel(L"Capsule"), values));

				vector<Material*> mats = models.back()->GetModel()->Materials();
				for (Material* mat : mats) {
					mat->SetDiffuseMap(Textures + L"Box.png");
				}
				models.back()->Position(D3DXVECTOR3(
					Math::Random(min.x, max.x), 0, 
					Math::Random(min.z, max.z)));
			}
			if (ImGui::MenuItem("Cube", "Create Cube Model")) {
				models.push_back(new GameModel(
					L"Cube",
					values->GlobalModel->GetModel(L"Cube"), values, true));

				vector<Material*> mats = models.back()->GetModel()->Materials();
				for (Material* mat : mats) {
					mat->SetDiffuseMap(Textures + L"Box.png");
				}
				models.back()->Position(D3DXVECTOR3(
					Math::Random(min.x, max.x), 0.5f,
					Math::Random(min.z, max.z)));
				//models.back()->RootAxis(D3DXVECTOR3(0, 45.0f, 0));
				models.back()->Rotate(D3DXVECTOR2(Math::Random(30.0f, 60.0f), 0));
			}
			if (ImGui::MenuItem("Cylinder", "Create Cylinder Model")) {
				models.push_back(new GameModel(
					L"Cylinder",
					values->GlobalModel->GetModel(L"Cylinder"), values));

				vector<Material*> mats = models.back()->GetModel()->Materials();
				for (Material* mat : mats) {
					mat->SetDiffuseMap(Textures + L"Box.png");
				}
				models.back()->Position(D3DXVECTOR3(
					Math::Random(min.x, max.x), 1.0f,
					Math::Random(min.z, max.z)));
			}
			if (ImGui::MenuItem("Sphere", "Create Sphere Model")) {
				models.push_back(new GameModel(
					L"Sphere",
					values->GlobalModel->GetModel(L"Sphere"), values));

				vector<Material*> mats = models.back()->GetModel()->Materials();
				for (Material* mat : mats) {
					mat->SetDiffuseMap(Textures + L"Box.png");
				}
				models.back()->Position(D3DXVECTOR3(
					Math::Random(min.x, max.x), 0,
					Math::Random(min.z, max.z)));
			}
			if (ImGui::MenuItem("Teapot", "Create Teapot Model")) {
				models.push_back(new GameModel(
					L"Teapot",
					values->GlobalModel->GetModel(L"Teapot"), values));

				vector<Material*> mats = models.back()->GetModel()->Materials();
				for (Material* mat : mats) {
					mat->SetDiffuseMap(Textures + L"Box.png");
				}
				models.back()->Position(D3DXVECTOR3(
					Math::Random(min.x, max.x), 0,
					Math::Random(min.z, max.z)));
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Delete"))
		{
			if (ImGui::MenuItem("Last", "Last Model Delete"))
			{
				SAFE_DELETE(models.back());
				models.pop_back();
			}
			if (ImGui::MenuItem("All", "All Model Delete"))
			{
				for (int i = 0; i < models.size(); i++) {
					SAFE_DELETE(models[i]);
				}
				models.clear();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Model Info");
	
	for (int i = 0; i < models.size(); i++) {
		string name = String::ToString(models[i]->GetName())
			+ " " + to_string(i);
		ImGui::TextColored(ImVec4(0, 1, 0, 1), name.c_str());
		D3DXVECTOR3 pos = models[i]->Position();
		float fPos[] = { pos.x, pos.y,pos.z };
		ImGui::DragFloat3((name + " Pos").c_str(), fPos, 0.005f);
		models[i]->Position(fPos);

		D3DXVECTOR3 scale = models[i]->Scale();
		float fScale[] = { scale.x, scale.y, scale.z };
		ImGui::DragFloat3((name + " Scale").c_str(), fScale, 0.01f);
		models[i]->Scale(fScale);
	
		//ImGui::DragFloat2((name + " Angle").c_str(), models[i]->Angle());
		//if(ImGui::Button((name + " Rotate").c_str()))
		//{
		//	models[i]->Rotate(models[i]->Angle());
		//}
	}

	ImGui::End();
}

void ExeHomework::ResizeScreen()
{
}

