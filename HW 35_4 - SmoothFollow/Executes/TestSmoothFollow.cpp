#include "stdafx.h"
#include "TestSmoothFollow.h"

#include "../Objects/GameAnimModel.h"
#include "../Objects/Player.h"
#include "../Objects/Monster.h"
#include "../Model/ModelClip.h"

#include "../Renders/Lines/DebugDrawSphere2.h"

#include "./Objects/MeshPlane.h"

#include "./Viewer/SmoothFollow.h"

TestSmoothFollow::TestSmoothFollow(ExecuteValues * values)
	:Execute(values)
	, bSmoothFollow(false)
{
	shader = new Shader(Shaders + L"Homework/Animation.hlsl");

	visibleBuffer = new VisibleBuffer();

	// Create Model
	{
		// Player
		{
			models.push_back(new Player(
				Models + L"Paladin/", L"Paladin.material",
				Models + L"Paladin/", L"Paladin.mesh"));
			models.back()->SetShader(shader);
			models.back()->Name("Paladin");
			models.back()->Scale(D3DXVECTOR3(0.2f, 0.2f, 0.2f));
			//models.back()->RotationDegree(D3DXVECTOR3(0, 180, 0));
			//models.back()->Position(0, -3, 0);
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
			models[i]->Play(0, true, 0, 5);
		}
	}

	selectModel = 0;

	planeShader = new Shader(Shaders + L"Homework/Mesh2.hlsl");
	plane = new MeshPlane();
	plane->SetShader(planeShader);
	plane->SetDiffuseMap(Textures + L"Floor.png");
	plane->Scale(50, 1, 50);

	smoothFollow = new SmoothFollow(100, 30);
	free = values->MainCamera;
	//values->MainCamera = smoothFollow;
	smoothFollow->SetTarget(
		models.back()->Position(), models.back()->Rotation(), models.back()->Up());
}

TestSmoothFollow::~TestSmoothFollow()
{
	for (GameAnimModel* model : models)
		SAFE_DELETE(model);

	SAFE_DELETE(shader);

	SAFE_DELETE(visibleBuffer);

	SAFE_DELETE(planeShader);
	SAFE_DELETE(plane);

	if (values->MainCamera != free)
		values->MainCamera = free;
	SAFE_DELETE(smoothFollow);
}

void TestSmoothFollow::Update()
{
	if (bSmoothFollow) {
		smoothFollow->SetTarget(
			models.back()->Position(), models.back()->Rotation(), models.back()->Up());

		if (Keyboard::Get()->Press('Q')) {
			smoothFollow->SetHeight(smoothFollow->GetHeight() - 1);
		}
		if (Keyboard::Get()->Press('E')) {
			smoothFollow->SetHeight(smoothFollow->GetHeight() + 1);
		}
		if (Keyboard::Get()->Press('Z')) {
			if (smoothFollow->GetDistance() >= 0)
				smoothFollow->SetDistance(smoothFollow->GetDistance() - 1);
		}
		if (Keyboard::Get()->Press('X')) {
			smoothFollow->SetDistance(smoothFollow->GetDistance() + 1);
		}
	}

	plane->Update();

	for (GameAnimModel* model : models)
		model->Update();
}

void TestSmoothFollow::PreRender()
{

}

void TestSmoothFollow::Render()
{
	plane->Render();

	visibleBuffer->SetPSBuffer(10);
	for (GameAnimModel* model : models) {
		values->GlobalLight->SetPSBuffer(0);
		model->Render();
	}
}

void TestSmoothFollow::PostRender()
{
	if (ImGui::Checkbox("SmoothFollow", &bSmoothFollow))
	{
		if (bSmoothFollow)
			values->MainCamera = smoothFollow;
		else
			values->MainCamera = free;
	}

	ImGui::RadioButton("Player", &selectModel, 0);
	//ImGui::SameLine(120);
	//ImGui::RadioButton("Monster", &selectModel, 1);

	models[selectModel]->PostRender();

	smoothFollow->PostRender();
}
