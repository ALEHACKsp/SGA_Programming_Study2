#include "stdafx.h"
#include "ExHomework.h"

#include "../Objects/GameAnimModel.h"
#include "../Objects/Player.h"
#include "../Objects/Monster.h"
#include "../Model/ModelClip.h"

#include "../Renders/Lines/DebugDrawSphere2.h"

#include "../Viewer/Orbit.h"

ExHomework::ExHomework(ExecuteValues * values)
	:Execute(values)
{
	shader = new Shader(Shaders + L"Homework/Animation.hlsl");

	visibleBuffer = new VisibleBuffer();

	// Create Model
	{
		models.push_back(new Player(
			Models + L"Paladin/", L"Paladin.material",
			Models + L"Paladin/", L"Paladin.mesh"));
		models.back()->SetShader(shader);
		models.back()->Name("Paladin");
		models.back()->Scale(D3DXVECTOR3(0.2f, 0.2f, 0.2f));
		models.back()->RotationDegree(D3DXVECTOR3(0, 180, 0));
		models.back()->Position(0, -3, 0);

		models.push_back(new Monster(
			Models + L"Jill/", L"Jill.material",
			Models + L"Jill/", L"Jill.mesh"));
		models.back()->SetShader(shader);
		models.back()->Name("Jill");
		models.back()->Scale(D3DXVECTOR3(0.2f, 0.2f, 0.2f));
		models.back()->Position(D3DXVECTOR3(0, 0, 50));
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
			models[i]->Play(0, true, 0, 5);
		}
	}

	selectModel = 0;

	models[0]->SetOtherModel(models[1]);
	models[1]->SetOtherModel(models[0]);
}

ExHomework::~ExHomework()
{
	for (GameAnimModel* model : models)
		SAFE_DELETE(model);

	SAFE_DELETE(shader);

	SAFE_DELETE(visibleBuffer);
}

void ExHomework::Update()
{
	for (GameAnimModel* model : models)
		model->Update();
}

void ExHomework::PreRender()
{

}

void ExHomework::Render()
{
	visibleBuffer->SetPSBuffer(10);
	for (GameAnimModel* model : models) {
		values->GlobalLight->SetPSBuffer(0);
		model->Render();
	}
}

void ExHomework::PostRender()
{
	ImGui::RadioButton("Player", &selectModel, 0);
	ImGui::SameLine(120);
	ImGui::RadioButton("Monster", &selectModel, 1);

	models[selectModel]->PostRender();
}
