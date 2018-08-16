#include "stdafx.h"
#include "Program.h"

#include "./Viewer/Freedom.h"

#include "./Executes/ExeHomework.h"

Program::Program()
{
	States::Create();

	D3DDesc desc;
	D3D::GetDesc(&desc);

	values = new ExecuteValues();
	values->ViewProjection = new ViewProjectionBuffer();
	values->GlobalLight = new LightBuffer();
	values->Perspective = new Perspective(desc.Width, desc.Height);
	values->Viewport = new Viewport(desc.Width, desc.Height);

	values->MainCamera = new Freedom();
	values->MainCamera->RotationDegree(53.02f, 36.97f);
	values->MainCamera->Position(-22.87f, 29.89f, -18.42f);

	values->GlobalLight->Data.Direction = D3DXVECTOR3(0.7f, -1.0f, 0.5f);

	executes.push_back(new ExeHomework(values));

	// -22.87f, 29.89f, -18.42f
	// degree
	// 53.02f, 36.97f
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
	for (Execute* exe : executes)
		exe->PostRender();

	ImGui::Text("FPS : %f", Time::Get()->FPS());
	// ±¸ºÐÀÚ »ý±è
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