#include "stdafx.h"
#include "TestLookAt.h"

#include "../Objects/GameAnimModel.h"
#include "../Objects/Player.h"
#include "../Objects/Monster.h"
#include "../Model/ModelClip.h"

#include "../Renders/Lines/DebugDrawSphere2.h"

#include "./Objects/MeshPlane.h"

#include "./Viewer/Orbit.h"

TestLookAt::TestLookAt(ExecuteValues * values)
	:Execute(values)
	, bLookAt(false)
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

	free = values->MainCamera;
	//values->MainCamera = orbit;
	
	bOrbit = false;
	orbit = new Orbit();
	orbit->SetOrbitDist(300);
	orbit->SetTarget(models.back()->Position());
}

TestLookAt::~TestLookAt()
{
	for (GameAnimModel* model : models)
		SAFE_DELETE(model);

	SAFE_DELETE(shader);

	SAFE_DELETE(visibleBuffer);

	SAFE_DELETE(planeShader);
	SAFE_DELETE(plane);

	if (values->MainCamera != free)
		values->MainCamera = free;
	SAFE_DELETE(orbit);
}

void TestLookAt::Update()
{

#pragma region LookAtTest
	// lookat Test
	//if (Keyboard::Get()->Press('C')) {
	//	D3DXVECTOR3 camPos;
	//	D3DXVECTOR3 camUp, camForward;
	//	values->MainCamera->Position(&camPos);
	//	values->MainCamera->Up(&camUp);
	//	D3DXVec3Normalize(&camUp, &camUp);
	//	values->MainCamera->Forward(&camForward);
	//	D3DXVec3Normalize(&camForward, &camForward);
	//
	//	// look at test
	//	D3DXQUATERNION quat = Math::LookAt(
	//		models.back()->Position(), camPos, camUp);
	//
	//	//D3DXQUATERNION quat = Math::LookAt(
	//	//	models.back()->Position(), camPos, camForward, camUp);
	//
	//	D3DXVECTOR3 rot;
	//	Math::toEulerAngle(quat, rot);
	//	rot.y = 2.0f * D3DX_PI - rot.y;
	//	rot.x = -rot.x;
	//	D3DXVECTOR3 modelRot = models.back()->Rotation();
	//	//modelRot.y = rot.y;
	//	modelRot = rot;
	//	models.back()->Rotation(modelRot);
	//}

	// lookat Test2
	//if (Keyboard::Get()->Press('C')) {
	//	D3DXVECTOR3 camPos;
	//	D3DXVECTOR2 camRot;
	//
	//	values->MainCamera->Position(&camPos);
	//	values->MainCamera->Rotation(&camRot);
	//	
	//	D3DXVECTOR3 modelPos, modelUp, modelForward;
	//	modelPos = models.back()->Position();
	//	modelUp = models.back()->Up();
	//	D3DXVec3Normalize(&modelUp, &modelUp);
	//	modelForward = models.back()->Direction();
	//	D3DXVec3Normalize(&modelForward, &modelForward);
	//
	//	
	//	D3DXQUATERNION quat = Math::LookAt(camPos, modelPos, modelUp);
	//
	//	// 이 방법 문제 있음
	//	//D3DXQUATERNION quat = Math::LookAt(
	//	//	camPos, modelPos, modelForward, modelUp);
	//
	//	D3DXVECTOR3 rot;
	//	Math::toEulerAngle(quat, rot);
	//
	//	camRot.x = -rot.x;
	//	camRot.y = 2.0f * D3DX_PI - rot.y;
	//
	//	values->MainCamera->Rotation(camRot.x, camRot.y);
	//}
#pragma endregion

	if (bLookAt && !bOrbit) {
		D3DXVECTOR3 camPos;
		D3DXVECTOR2 camRot;
		D3DXVECTOR3 camUp;
	
		values->MainCamera->Position(&camPos);
		values->MainCamera->Rotation(&camRot);
		values->MainCamera->Up(&camUp);
		D3DXVec3Normalize(&camUp, &camUp);
		
		D3DXVECTOR3 modelPos, modelUp, modelForward;
		modelPos = models.back()->Position();
		modelUp = models.back()->Up();
		D3DXVec3Normalize(&modelUp, &modelUp);
		modelForward = models.back()->Direction();
		D3DXVec3Normalize(&modelForward, &modelForward);
	

		D3DXVECTOR3 forward = modelPos - camPos;
		//D3DXVECTOR3 forward = camPos - modelPos;
		D3DXQUATERNION quat = Math::LookAt(forward, modelUp);

		//D3DXQUATERNION quat = Math::LookAt(camPos, modelPos, modelForward, modelUp);
		//D3DXQUATERNION quat = Math::LookAt(camPos, modelPos, modelUp);
	
		// 이 방법 문제 있음
		//D3DXQUATERNION quat = Math::LookAt(
		//	camPos, modelPos, modelForward, modelUp);
	
		D3DXVECTOR3 rot;
		Math::toEulerAngle(quat, rot);
	
		camRot.x = rot.x;
		camRot.y = rot.y;
		//camRot.y = 2.0f * D3DX_PI - rot.y;
	
		values->MainCamera->Rotation(camRot.x, camRot.y);
	}

	if (bOrbit)
		orbit->SetTarget(models.back()->Position());
	

	plane->Update();

	for (GameAnimModel* model : models)
		model->Update();
}

void TestLookAt::PreRender()
{

}

void TestLookAt::Render()
{
	plane->Render();

	visibleBuffer->SetPSBuffer(10);
	for (GameAnimModel* model : models) {
		values->GlobalLight->SetPSBuffer(0);
		model->Render();
	}
}

void TestLookAt::PostRender()
{
	ImGui::Checkbox("LookAt", &bLookAt);
	if (ImGui::Checkbox("Orbit", &bOrbit)) {
		if (bOrbit == false) values->MainCamera = free;
		else values->MainCamera = orbit;
	}

	ImGui::RadioButton("Player", &selectModel, 0);
	//ImGui::SameLine(120);
	//ImGui::RadioButton("Monster", &selectModel, 1);

	models[selectModel]->PostRender();

}
