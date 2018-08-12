#include "stdafx.h"
#include "Program.h"

#include "./Viewer/Freedom.h"

#include "./Executes/ExportModel.h"
#include "./Executes/ExeHomework.h"

Program::Program()
	: startCamPos(7.7f, 48.9f, -17.8f)
	, startCamRot(1.21f, -0.17f)
{
	States::Create();

	D3DDesc desc;
	D3D::GetDesc(&desc);

	values = new ExecuteValues();
	values->ViewProjection = new ViewProjectionBuffer();
	values->GlobalLight = new LightBuffer();
	values->Perspective = new Perspective(desc.Width, desc.Height);
	values->Viewport = new Viewport(desc.Width, desc.Height);

	values->mainCamera = new Freedom();
	values->mainCamera->Rotation(startCamRot.x, startCamRot.y);
	values->mainCamera->Position(
		startCamPos.x, startCamPos.y, startCamPos.z);

	values->GlobalModel = new ModelBuffer();

	//executes.push_back(new ExportModel(values));
	executes.push_back(new ExeHomework(values));

	// 7.7f, 48.9f, -17.8f
	// 1.21f, -0.17f
}

Program::~Program()
{
	for (Execute* exe : executes)
		SAFE_DELETE(exe);

	SAFE_DELETE(values->GlobalModel);
	SAFE_DELETE(values->GlobalLight);
	SAFE_DELETE(values->ViewProjection);
	SAFE_DELETE(values->Perspective);
	SAFE_DELETE(values->Viewport);
	SAFE_DELETE(values);

	States::Delete();
}

void Program::Update()
{
	values->mainCamera->Update();

	for (Execute* exe : executes)
		exe->Update();
}

void Program::PreRender()
{

}

void Program::Render()
{
	D3DXMATRIX view, projection;
	values->mainCamera->Matrix(&view);
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
	ImGui::SliderFloat3("Direction", 
		(float *)&values->GlobalLight->Data.Direction, -1, 1);

	D3DXVECTOR3 vec;
	values->mainCamera->Position(&vec);
	ImGui::Text("Camera Pos: %.1f %.1f %.1f", vec.x, vec.y, vec.z);
	D3DXVECTOR2 vec2;
	values->mainCamera->Rotation(&vec2);
	ImGui::Text("Camera Rotate: %.2f %.2f", vec2.x, vec2.y);

	if (ImGui::Button("Camera Reset"))
	{
		values->mainCamera->Rotation(startCamRot.x, startCamRot.y);
		values->mainCamera->Position(
			startCamPos.x, startCamPos.y, startCamPos.z);
	}

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