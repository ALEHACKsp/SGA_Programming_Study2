#include "stdafx.h"
#include "ExeHomework.h"

#include "../Fbx/Exporter.h"

#include "../Objects/GameRect.h"
#include "../Objects/GameSphere.h"
#include "../Objects/GameTank.h"

ExeHomework::ExeHomework(ExecuteValues * values)
	:Execute(values)
{
	// Model Add
	{
		Models::AddMaterial(Materials + L"Meshes/Capsule.material");
		Models::AddMesh(Models + L"Meshes/Capsule.mesh");

		Models::AddMaterial(Materials + L"Meshes/Cube.material");
		Models::AddMesh(Models + L"Meshes/Cube.mesh");

		Models::AddMaterial(Materials + L"Meshes/Cylinder.material");
		Models::AddMesh(Models + L"Meshes/Cylinder.mesh");

		Models::AddMaterial(Materials + L"Meshes/Sphere.material");
		Models::AddMesh(Models + L"Meshes/Sphere.mesh");

		Models::AddMaterial(Materials + L"Meshes/Teapot.material");
		Models::AddMesh(Models + L"Meshes/Teapot.mesh");

		Models::AddMaterial(Materials + L"Meshes/Plane.material");
		Models::AddMesh(Models + L"Meshes/Plane.mesh");

		Models::AddMaterial(Materials + L"Meshes/Quad.material");
		Models::AddMesh(Models + L"Meshes/Quad.mesh");

		Models::AddMaterial(Models + L"Tank/Tank.material");
		Models::AddMesh(Models + L"Tank/Tank.mesh");
	}

	wstring matFolder = Materials + L"Meshes/";
	wstring meshFolder = Models + L"Meshes/";

	// Plane
	models.push_back(new GameRect(matFolder, meshFolder, L"Plane"));
	models[0]->SetDiffuseMap(Textures + L"Dirt.png");
	models[0]->Position(D3DXVECTOR3(0, -0.1f, 0));
	models[0]->Scale(D3DXVECTOR3(3.0f, 1.0f, 3.0f));

	// Tank
	models.push_back(new GameTank(Models + L"Tank/", Models + L"Tank/", L"Tank"));

	D3DXVECTOR3 min, max;
	models[0]->GetModel()->MinMaxVertex(&min, &max);
	D3DXVECTOR3 scale = models[0]->Scale();
	min.x *= scale.x;
	min.y *= scale.y;
	min.z *= scale.z;
	models[1]->Position(min);
	models[1]->Scale(D3DXVECTOR3(0.5f, 0.5f, 0.5f));

	// Sphere 구 충돌 테스트용
	models.push_back(new GameSphere(matFolder, meshFolder, L"Sphere"));
	max.x *= scale.x;
	max.y *= scale.y;
	max.z *= scale.z;
	models[2]->Position(max);

	// Sphere2 구 충돌 테스트용 2 충돌 시 이넘 파괴하게 할 꺼
	models.push_back(new GameSphere(matFolder, meshFolder, L"Sphere"));
	models.back()->SetDiffuseMap(Textures + L"Box.png");
	models[3]->Position(max - D3DXVECTOR3(3, 0, 0));
}

ExeHomework::~ExeHomework()
{
	for (UINT i = 0; i < models.size(); i++)
		SAFE_DELETE(models[i]);
}

void ExeHomework::Update()
{
	for (UINT i = 0; i < models.size(); i++)
		models[i]->Update();

	vector<GameModel* >::iterator iter = models.begin() + 3;
	for (; iter != models.end();)
	{
		// 탱크와 충돌 체크
		if ((*iter)->IsCollision(models[1]))
		{
			SAFE_DELETE(*iter);
			iter = models.erase(iter);
		}
		// 구와 충돌 체크
		else if ((*iter)->IsCollision(models[2]))
		{
			SAFE_DELETE(*iter);
			iter = models.erase(iter);
		}
		else
			iter++;
	}
}

void ExeHomework::PreRender()
{
	
}

void ExeHomework::Render()
{
	for (UINT i = 0; i < models.size(); i++)
		models[i]->Render();
}

void ExeHomework::PostRender()
{
	D3DXVECTOR3 min, max;
	if (models.size() > 0) {
		models[0]->GetModel()->MinMaxVertex(&min, &max);
	}

	wstring matFolder = Materials + L"Meshes/";
	wstring meshFolder = Models + L"Meshes/";

	ImGui::Begin("Model", 0, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Capsule", "Create Capsule Model"))
			{
				models.push_back(new GameSphere(
					matFolder, meshFolder, L"Capsule"));

				models.back()->SetDiffuseMap(Textures + L"Box.png");
				models.back()->Position(D3DXVECTOR3(
					Math::Random(min.x, max.x), 0, 
					Math::Random(min.z, max.z)));
			}
			if (ImGui::MenuItem("Cube", "Create Cube Model")) {
				models.push_back(new GameRect(
					matFolder, meshFolder, L"Cube"));

				models.back()->SetDiffuseMap(Textures + L"Box.png");

				models.back()->Position(D3DXVECTOR3(
					Math::Random(min.x, max.x), 0.5f,
					Math::Random(min.z, max.z)));
				models.back()->RootAxis(D3DXVECTOR3(0, Math::Random(30.0f, 60.0f), 0));
			}
			if (ImGui::MenuItem("Cylinder", "Create Cylinder Model")) {
				models.push_back(new GameSphere(
					matFolder, meshFolder, L"Cylinder"));

				models.back()->SetDiffuseMap(Textures + L"Box.png");
				models.back()->Position(D3DXVECTOR3(
					Math::Random(min.x, max.x), 1.0f,
					Math::Random(min.z, max.z)));
			}
			if (ImGui::MenuItem("Sphere", "Create Sphere Model")) {
				models.push_back(new GameSphere(
					matFolder, meshFolder, L"Sphere"));

				models.back()->SetDiffuseMap(Textures + L"Box.png");
				models.back()->Position(D3DXVECTOR3(
					Math::Random(min.x, max.x), 0.0f,
					Math::Random(min.z, max.z)));
			}
			if (ImGui::MenuItem("Teapot", "Create Teapot Model")) {
				models.push_back(new GameSphere(
					matFolder, meshFolder, L"Teapot"));

				models.back()->SetDiffuseMap(Textures + L"Box.png");
				models.back()->Position(D3DXVECTOR3(
					Math::Random(min.x, max.x), 0.0f,
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
				for (UINT i = 0; i < models.size(); i++) {
					SAFE_DELETE(models[i]);
				}
				models.clear();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Model Info");
	
	for (UINT i = 0; i < models.size(); i++) {
		string name = String::ToString(models[i]->Name()) + " " + to_string(i);
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

