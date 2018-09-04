#include "stdafx.h"
#include "Program.h"

#include "./Viewer/Freedom.h"

#include "./Executes/ExportModel.h"
#include "./Executes/ExportMesh.h"
#include "./Executes/DrawLandScape.h"

Program::Program()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	selectCam = 0;
	for (int i = 0; i < 2; i++) {
		camera[i] = new Freedom();
		perspective[i] = new Perspective(desc.Width, desc.Height);
		viewport[i] = new Viewport(desc.Width, desc.Height);
	}

	camera[0]->Position(0, 10, -15);

	// check용 카메라
	camera[1]->RotationDegree(26.24f, -46.98f);
	camera[1]->Position(111.34f, 65.33f, -87.38f);

	values = new ExecuteValues();
	values->ViewProjection = new ViewProjectionBuffer();
	values->GlobalLight = new LightBuffer();
	values->Perspective = perspective[selectCam];
	values->Viewport = viewport[selectCam];

	values->MainCamera = camera[selectCam];
	//values->MainCamera->RotationDegree(24, 0);
	//values->MainCamera->Position(82.18f, 27.00f, -29.00f);
	//values->MainCamera->Position(0, 10, -15);

	//executes.push_back(new ExportModel(values));
	//executes.push_back(new ExportMesh(values));

	executes.push_back(new DrawLandScape(values));
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
}

void Program::Update()
{
	values->Perspective = perspective[selectCam];
	values->Viewport = viewport[selectCam];
	values->MainCamera = camera[selectCam];

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
	for (Execute* exe : executes)
		exe->PostRender();

	ImGui::Text("FPS : %f", Time::Get()->FPS());
	// 구분자 생김
	ImGui::Separator();
	ImGui::SliderFloat3("Direction", 
	(float *)&values->GlobalLight->Data.Direction, -1, 1);
	ImGui::Separator();
	
	ImGui::SliderInt("Camera Select", &selectCam, 0, 1);

	D3DXVECTOR3 vec;
	values->MainCamera->Position(&vec);
	ImGui::LabelText("CamearPos", "%.2f, %.2f, %.2f", 
		vec.x, vec.y, vec.z);

	D3DXVECTOR2 rot;
	values->MainCamera->RotationDegree(&rot);
	ImGui::LabelText("CameraRot", "%.2f, %.2f", rot.x, rot.y);
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