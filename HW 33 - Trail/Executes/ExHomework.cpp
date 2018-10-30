#include "stdafx.h"
#include "ExHomework.h"

#include "../Objects/GameAnimModel.h"
#include "../Objects/Player.h"
#include "../Objects/Monster.h"
#include "../Model/ModelClip.h"

#include "../Renders/Lines/DebugDrawSphere2.h"

#include "./LandScape/Sky.h"

#include "../Viewer/Orbit.h"

ExHomework::ExHomework(ExecuteValues * values)
	:Execute(values)
{
	shader = new Shader(Shaders + L"Homework/Animation.hlsl");

	visibleBuffer = new VisibleBuffer();

	// Create Model
	{
		// Player
		{
			models.push_back(new Player(
				Models + L"Paladin2/", L"Paladin.material",
				Models + L"Paladin2/", L"Paladin.mesh"));
			models.back()->SetShader(shader);
			models.back()->Name("Paladin");
			models.back()->Scale(D3DXVECTOR3(0.2f, 0.2f, 0.2f));
			models.back()->RotationDegree(D3DXVECTOR3(0, 180, 0));
			models.back()->Position(0, -3, 0);
		}

		//// Monster
		//{
		//	// 1
		//	models.push_back(new Monster(
		//		Models + L"Jill/", L"Jill.material",
		//		Models + L"Jill/", L"Jill.mesh"));
		//	models.back()->SetShader(shader);
		//	models.back()->Name("Jill");
		//	models.back()->Scale(D3DXVECTOR3(0.2f, 0.2f, 0.2f));
		//	models.back()->Position(D3DXVECTOR3(0, 0, 65));

		//	// 2
		//	models.push_back(new Monster(
		//		Models + L"Jill/", L"Jill.material",
		//		Models + L"Jill/", L"Jill.mesh"));
		//	models.back()->SetShader(shader);
		//	models.back()->Name("Jill");
		//	models.back()->Scale(D3DXVECTOR3(0.2f, 0.2f, 0.2f));
		//	models.back()->Position(D3DXVECTOR3(0, 0, 200));

		//	// 3
		//	models.push_back(new Monster(
		//		Models + L"Jill/", L"Jill.material",
		//		Models + L"Jill/", L"Jill.mesh"));
		//	models.back()->SetShader(shader);
		//	models.back()->Name("Jill");
		//	models.back()->Scale(D3DXVECTOR3(0.2f, 0.2f, 0.2f));
		//	models.back()->RotationDegree(D3DXVECTOR3(0, 180, 0));
		//	models.back()->Position(D3DXVECTOR3(0, 0, -100));
		//	models.back()->SetDirection(GameAnimModel::Direction::RIGHT);
		//}
	}

	// Create Animation
	{
		for (int i = 0; i < models.size(); i++) {
			string aniFile[] = { "Idle.anim", "Walking.anim", "Attack.anim", 
				"Hitted.anim", "Dying.anim"};
			for (int j = 0; j < 5; j++) {
				wstring file = Models + String::ToWString(models[i]->Name() + "/" + aniFile[j]);
				models[i]->AddClip(file);
			}
			models[i]->Play(0, true, 0.0f, 5);
		}
	}

	selectModel = 0;

	//models[0]->SetOtherModel(models[1]);
	//models[0]->SetOtherModel(models[2]);
	//models[0]->SetOtherModel(models[3]);

	//models[1]->SetOtherModel(models[0]);
	//models[2]->SetOtherModel(models[0]);
	//models[3]->SetOtherModel(models[0]);

	free = values->MainCamera;

	orbit = new Orbit();
	orbit->SetOrbitDist(100.0f);
	values->MainCamera = orbit;

	sky = new Sky(values);
	sky->Set(D3DXCOLOR(0.216f, 0.1f, 0.098f, 1), D3DXCOLOR(0.66f, 0.14f, 0.196f, 1), 4.545f);
}

ExHomework::~ExHomework()
{
	for (GameAnimModel* model : models)
		SAFE_DELETE(model);

	SAFE_DELETE(shader);

	SAFE_DELETE(visibleBuffer);

	SAFE_DELETE(orbit);

	SAFE_DELETE(sky);
}

void ExHomework::Update()
{
	orbit->SetTarget(models[0]->Position());
	orbit->Update();
	
	sky->Update();

	for (GameAnimModel* model : models)
		model->Update();
}

void ExHomework::PreRender()
{

}

void ExHomework::Render()
{
	sky->Render();

	visibleBuffer->SetPSBuffer(10);
	for (GameAnimModel* model : models) {
		values->GlobalLight->SetPSBuffer(0);
		model->Render();
	}
}

void ExHomework::PostRender()
{
	ImGui::RadioButton("Player", &selectModel, 0);
	//ImGui::SameLine(120);
	//ImGui::RadioButton("Monster", &selectModel, 3);

	models[selectModel]->PostRender();
}
