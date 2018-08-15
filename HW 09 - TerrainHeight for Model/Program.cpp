#include "stdafx.h"
#include "Program.h"

#include "./Viewer/Freedom.h"
#include "./Viewer/Orbit.h"

#include "./Executes/ExportModel.h"
#include "./Executes/ExportMesh.h"
#include "./Executes/DrawLandScape.h"

#include "./Objects/GameModel.h"

Program::Program()
	: camStartPos(D3DXVECTOR3(82.18f, 27.00f, -29.00f))
	, camStartRot(D3DXVECTOR2(25, 0))
	, camPos2(D3DXVECTOR3(-74.2f, 186.78f, 124.72f))
	, camRot2(D3DXVECTOR2(47.62f, 89.97f))
	, cameraState(0)
{
	States::Create();

	D3DDesc desc;
	D3D::GetDesc(&desc);

	values = new ExecuteValues();
	values->ViewProjection = new ViewProjectionBuffer();
	values->GlobalLight = new LightBuffer();
	values->Perspective = new Perspective(desc.Width, desc.Height);
	values->Viewport = new Viewport(desc.Width, desc.Height);

	freedom = new Freedom();
	orbit = new Orbit();

	values->MainCamera = freedom;
	values->MainCamera->RotationDegree(camStartRot.x, camStartRot.y);
	values->MainCamera->Position(
		camStartPos.x, camStartPos.y, camStartPos.z);

	values->MainCamera = orbit;

	DrawLandScape* drawLandScape = new DrawLandScape(values);
	GameModel* target = drawLandScape->GetTank();
	orbit->SetTargetPos(target->PostionPtr());
	orbit->RotationDegree(50, 0);

	//executes.push_back(new ExportModel(values));
	//executes.push_back(new ExportMesh(values));
	executes.push_back(drawLandScape);

	//position = D3DXVECTOR3(0, 0, 0);
	//position = D3DXVECTOR3(2.23f, 1.26f, -3.93f);
	// 이 방법도 가능함
	//position = { 0, 0, 0 };

	// 82.18f, 27.00f, -29.00f
	// 25, 0

	// -74.2f, 186.78f, 124.72f
	// 47.62f, 89.97f
}

Program::~Program()
{
	for (Execute* exe : executes)
		SAFE_DELETE(exe);

	SAFE_DELETE(values->GlobalLight);
	SAFE_DELETE(values->ViewProjection);
	SAFE_DELETE(values->Perspective);
	SAFE_DELETE(values->Viewport);
	SAFE_DELETE(values);

	SAFE_DELETE(freedom);
	SAFE_DELETE(orbit);

	States::Delete();
}

void Program::Update()
{
	values->MainCamera->Update();

	for (Execute* exe : executes)
		exe->Update();
}

void Program::PreRender()
{

}

void Program::Render()
{
	D3DXMATRIX view, projection;
	values->MainCamera->Matrix(&view);
	values->Perspective->GetMatrix(&projection);

	values->ViewProjection->SetView(view);
	values->ViewProjection->SetProjection(projection);
	values->ViewProjection->SetVSBuffer(0);

	values->GlobalLight->SetPSBuffer(0);

	for (Execute* exe : executes)
		exe->Render();
}

void Program::PostRender()
{
	ImGui::Text("FPS : %f", Time::Get()->FPS());
	// 구분자 생김
	ImGui::Separator();
	ImGui::SliderFloat3("Direction", 
	(float *)&values->GlobalLight->Data.Direction, -1, 1);
	ImGui::Separator();
	
	D3DXVECTOR3 vec;
	values->MainCamera->Position(&vec);
	ImGui::LabelText("CamearPos", "%.2f, %.2f, %.2f", 
		vec.x, vec.y, vec.z);

	D3DXVECTOR2 rot;
	values->MainCamera->RotationDegree(&rot);
	ImGui::LabelText("CameraRot", "%.2f, %.2f", rot.x, rot.y);

	if (ImGui::SliderInt("Camera State", &cameraState, 0, 1))
	{
		if (cameraState == 0)
			values->MainCamera = freedom;
		else if (cameraState == 1)
			values->MainCamera = orbit;
	}

	if(ImGui::Button("Camera Reset")) {
		values->MainCamera = freedom;
		values->MainCamera->RotationDegree(camStartRot.x, camStartRot.y);
		values->MainCamera->Position(
			camStartPos.x, camStartPos.y, camStartPos.z);
		cameraState = 0;
	}

	if (ImGui::Button("Camera Freedom Setting2"))
	{
		values->MainCamera = freedom;
		values->MainCamera->RotationDegree(camRot2.x, camRot2.y);
		values->MainCamera->Position(
			camPos2.x, camPos2.y, camPos2.z);
		cameraState = 0;
	}

	ImGui::Separator();
	for (Execute* exe : executes)
		exe->PostRender();
}

void Program::ResizeScreen()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	values->Perspective->Set(desc.Width, desc.Height);
	values->Viewport->Set(desc.Width, desc.Height);

	for (Execute* exe : executes)
		exe->ResizeScreen();
}