#include "stdafx.h"
#include "ExeHomework.h"

#include "../Fbx/Exporter.h"

#include "../Objects/GameModel.h"

ExeHomework::ExeHomework(ExecuteValues * values)
	:Execute(values)
{
	//models.push_back( new GameModel(
	//	Models + L"Meshes/Cube/", L"Cube.material",
	//	Models + L"Meshes/Cube/", L"Cube.mesh"
	//));

	//vector<Material*> mats = models[0]->GetModel()->Materials();
	//for (Material* mat : mats) {
	//	mat->SetDiffuseMap(Textures + L"Box.png");
	//}

	//models.push_back(new GameModel(
	//	Models + L"/Tank/", L"Tank.material",
	//	Models + L"/Tank/", L"Tank.mesh"
	//));

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

	models.push_back(new GameModel(L"Plane", values->GlobalModel->GetModel(L"Plane")));
	vector<Material*> mats = models.back()->GetModel()->Materials();
	for (Material* mat : mats) {
		mat->SetDiffuseMap(Textures + L"Dirt.png");
	}
	models[0]->Position(D3DXVECTOR3(0, -0.6f, 0));
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
	ImGui::Begin("Model", 0, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Capsule", "Create Capsule Model"))
			{
				models.push_back(new GameModel(
					L"Capsule",
					values->GlobalModel->GetModel(L"Capsule")));

				vector<Material*> mats = models.back()->GetModel()->Materials();
				for (Material* mat : mats) {
					mat->SetDiffuseMap(Textures + L"Box.png");
				}
			}
			if (ImGui::MenuItem("Cube", "Create Cube Model")) {
				models.push_back(new GameModel(
					L"Cube",
					values->GlobalModel->GetModel(L"Cube")));

				vector<Material*> mats = models.back()->GetModel()->Materials();
				for (Material* mat : mats) {
					mat->SetDiffuseMap(Textures + L"Box.png");
				}
			}
			if (ImGui::MenuItem("Cylinder", "Create Cylinder Model")) {
				models.push_back(new GameModel(
					L"Cylinder",
					values->GlobalModel->GetModel(L"Cylinder")));

				vector<Material*> mats = models.back()->GetModel()->Materials();
				for (Material* mat : mats) {
					mat->SetDiffuseMap(Textures + L"Box.png");
				}
			}
			if (ImGui::MenuItem("Sphere", "Create Sphere Model")) {
				models.push_back(new GameModel(
					L"Sphere",
					values->GlobalModel->GetModel(L"Sphere")));

				vector<Material*> mats = models.back()->GetModel()->Materials();
				for (Material* mat : mats) {
					mat->SetDiffuseMap(Textures + L"Box.png");
				}

				models.back()->Position(D3DXVECTOR3(0, -0.5f, 0));
			}
			if (ImGui::MenuItem("Teapot", "Create Teapot Model")) {
				models.push_back(new GameModel(
					L"Teapot",
					values->GlobalModel->GetModel(L"Teapot")));

				vector<Material*> mats = models.back()->GetModel()->Materials();
				for (Material* mat : mats) {
					mat->SetDiffuseMap(Textures + L"Box.png");
				}
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
		//ImGui::Text(name.c_str());
		D3DXVECTOR3 pos = models[i]->Position();
		float fPos[] = { pos.x, pos.y,pos.z };
		ImGui::DragFloat3((name + " Pos").c_str(), fPos, 0.005f);
		models[i]->Position(fPos);
	}

	ImGui::End();
}

void ExeHomework::ResizeScreen()
{
}

