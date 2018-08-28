#include "stdafx.h"
#include "Program.h"

#include "./Viewer/Freedom.h"
#include "./Viewer/Orbit.h"

#include "./Executes/ExportModel.h"
#include "./Executes/ExportMesh.h"
#include "./Executes/DrawLandScape.h"

Program::Program()
	: startPos(-16.88f, 90.62f, -24.15f)
	, startRot(29.04f, 47.11f)
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	values = new ExecuteValues();
	values->ViewProjection = new ViewProjectionBuffer();
	values->GlobalLight = new LightBuffer();
	values->Perspective = new Perspective(desc.Width, desc.Height);
	values->Viewport = new Viewport(desc.Width, desc.Height);

	camKind = 0;
	freedom = new Freedom();
	freedom->RotationDegree(startRot.x, startRot.y);
	freedom->Position(startPos.x, startPos.y, startPos.z);
	//freedom->Position(0, 0, -10);
	orbit = new Orbit();

	values->MainCamera = orbit;
	//values->MainCamera->RotationDegree(24, 0);
	//values->MainCamera->Position(82.18f, 27.00f, -29.00f);
	values->GlobalLight->Data.Direction = D3DXVECTOR3(-0.917f, -0.542f, 1.0f);

	//executes.push_back(new ExportModel(values));
	//executes.push_back(new ExportMesh(values));
	executes.push_back(new DrawLandScape(values));

	// direction 
	// -0.917f, -0.542f, 1.0f

	// pos
	// -16.88f, 90.62f, -24.15f
	// 29.04f, 47.11f
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
	for (Execute* exe : executes)
		exe->Update();

	values->MainCamera->Update();

	switch (camKind)
	{
	case 0:
		values->MainCamera = freedom;
		break;
	case 1:
		values->MainCamera = orbit;
		break;
	}
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
	// ±¸ºÐÀÚ »ý±è
	ImGui::Separator();
	ImGui::SliderFloat3("Direction", 
	(float *)&values->GlobalLight->Data.Direction, -1, 1);
	ImGui::Separator();

	ImGui::Text("Camera");
	ImGui::RadioButton("Freedom", &camKind, 0);
	ImGui::SameLine(120);
	ImGui::RadioButton("Orbit", &camKind, 1);
	ImGui::SameLine(200);
	if (ImGui::SmallButton("Reset"))
	{
		values->MainCamera->RotationDegree(startRot.x, startRot.y);
		values->MainCamera->Position(startPos.x, startPos.y, startPos.z);
	}

	D3DXVECTOR3 vec;
	values->MainCamera->Position(&vec);
	ImGui::LabelText("CamearPos", "%.2f, %.2f, %.2f", 
		vec.x, vec.y, vec.z);

	D3DXVECTOR2 rot;
	values->MainCamera->RotationDegree(&rot);
	ImGui::LabelText("CameraRot", "%.2f, %.2f", rot.x, rot.y);

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