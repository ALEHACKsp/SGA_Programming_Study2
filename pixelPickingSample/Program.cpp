#include "stdafx.h"
#include "Program.h"
#include "./Viewer/Freedom.h"

#include "./Executes/ExportAnim.h"
#include "./Executes/ExportMesh.h"
#include "./Executes/TestScattering.h"
#include "./Executes/MapEditor.h"
#include "./Executes/TestGizmo.h"
#include "./Executes/AnimationEditor.h"
#include "./Executes/RTS.h"

Program::Program()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	values = new ExecuteValues();
	values->ViewProjection = new ViewProjectionBuffer();
	values->GlobalLight = new LightBuffer();
	values->Perspective = new Perspective(desc.Width, desc.Height, 0.7853981853f, 0.1000000015f, 20000.0f);
	values->Viewport = new Viewport(desc.Width, desc.Height);
	
	values->MainCamera = new Freedom();
	values->MainCamera->Position(0.0f, 0.0f, -5.0f);

	values->GlobalLight->Data.Direction = D3DXVECTOR3(-1, -1, -1);

	//executes.push_back(new ExportMesh(values));
	//executes.push_back(new ExportAnim(values));
	executes.push_back(new MapEditor(values));
}

Program::~Program()
{
	for (Execute* exe : executes)
		SAFE_DELETE(exe);

	SAFE_DELETE(values->GlobalLight);
	SAFE_DELETE(values->ViewProjection);
	SAFE_DELETE(values->Perspective);
	SAFE_DELETE(values->Viewport);
	SAFE_DELETE(values->MainCamera);
	SAFE_DELETE(values);
}

void Program::Update()
{
	values->MainCamera->Update();
	for (Execute* exe : executes)
		exe->Update();
}

void Program::PreRender()
{
	D3DXMATRIX view, projection;
	values->Perspective->GetMatrix(&projection);

	values->MainCamera->Matrix(&view);

	values->ViewProjection->SetView(view);
	values->ViewProjection->SetProjection(projection);
	values->ViewProjection->SetVSBuffer(0);

	for (Execute* exe : executes)
		exe->PreRender();
}

void Program::Render()
{
	D3DXMATRIX view, projection;
	values->Perspective->GetMatrix(&projection);

	values->MainCamera->Matrix(&view);

	values->ViewProjection->SetView(view);
	values->ViewProjection->SetProjection(projection);
	values->ViewProjection->SetVSBuffer(0);

	values->GlobalLight->SetPSBuffer(0);

	values->Viewport->RSSetViewport();

	for (Execute* exe : executes)
		exe->Render();

	ImGui::SliderFloat3("Direction", (float*)&values->GlobalLight->Data.Direction, -1, 1);
	ImGui::SliderFloat("Intensity", (float*)&values->GlobalLight->Data.Intensity, 0, 3);
}

void Program::PostRender()
{
	for (Execute* exe : executes)
		exe->PostRender();

	wstring str = String::Format(L"FPS : %.0f", ImGui::GetIO().Framerate);
	RECT rect = { 0, 0, 300, 300 };
	DirectWrite::RenderText(str, rect, 12);

	rect.top += 12;
	D3DXVECTOR3 vec;
	values->MainCamera->Position(&vec);
	str = String::Format(L"CameraPos : %.0f, %.0f, %.0f", vec.x, vec.y, vec.z);
	DirectWrite::RenderText(str, rect, 12);

	rect.top += 12;
	D3DXVECTOR2 rot;
	values->MainCamera->RotationDegree(&rot);
	str = String::Format(L"CameraRot : %.0f, %.0f", rot.x, rot.y);
	DirectWrite::RenderText(str, rect, 12);
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