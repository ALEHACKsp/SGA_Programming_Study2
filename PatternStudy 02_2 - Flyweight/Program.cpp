#include "stdafx.h"
#include "Program.h"

#include "./Viewer/Freedom.h"

#include "./Executes/Export.h"

#include "./Executes/TestFlyweight.h"

Program::Program()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	values = new ExecuteValues();
	values->ViewProjection = new ViewProjectionBuffer();
	values->GlobalLight = new LightBuffer();
	values->Perspective = new Perspective(desc.Width, desc.Height);
	values->Viewport = new Viewport(desc.Width, desc.Height);

	values->MainCamera = new Freedom();
	values->MainCamera->Position(0, 0, -10);

	values->GlobalLight->Data.Direction = D3DXVECTOR3(-1, -1, 1);

	//executes.push_back(new Export(values));

	executes.push_back(new TestFlyweight(values));

	selectExecute = executes.size() - 1;
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
	values->MainCamera->Update();

	//for (Execute* exe : executes)
	//	exe->Update();
	executes[selectExecute]->Update();
}

void Program::PreRender()
{
	//D3DXMATRIX view, projection;
	//values->MainCamera->Matrix(&view);
	//values->Perspective->GetMatrix(&projection);

	//values->ViewProjection->SetView(view);
	//values->ViewProjection->SetProjection(projection);
	//values->ViewProjection->SetVSBuffer(0);

	//for (Execute* exe : executes)
	//	exe->PreRender();
}

void Program::Render()
{
	ImGui::Text("FPS : %.2f", Time::Get()->FPS());
	//D3DXVECTOR3 pos;
	//values->MainCamera->Position(&pos);
	//ImGui::Text("CameraPos %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);
	//D3DXVECTOR2 rot;
	//values->MainCamera->RotationDegree(&rot);
	//ImGui::Text("CameraRot %.2f, %.2f", rot.x, rot.y);
	//ImGui::SliderFloat3("Direction",
	//	(float *)&values->GlobalLight->Data.Direction, -1, 1);

	ImGui::Text("Flyweight Design Pattern Test 2");

	ImGui::Separator();

	D3DXMATRIX view, projection;
	values->MainCamera->Matrix(&view);
	values->Perspective->GetMatrix(&projection);

	values->ViewProjection->SetView(view);
	values->ViewProjection->SetProjection(projection);
	values->ViewProjection->SetVSBuffer(0);

	values->GlobalLight->SetPSBuffer(0);
	values->Viewport->RSSetViewport();

	//for (Execute* exe : executes)
	//	exe->Render();
	executes[selectExecute]->Render();
}

void Program::PostRender()
{
	//for (Execute* exe : executes)
	//	exe->PostRender();

	executes[selectExecute]->PostRender();

	//int space = 13;
	//wstring str = String::Format(L"FPS : %.0f", ImGui::GetIO().Framerate);
	//RECT rect = { 0,0,300,300 };
	//DirectWrite::RenderText(str, rect, 12);
	//
	//rect.top += space;
	//D3DXVECTOR3 vec;
	//values->MainCamera->Position(&vec);
	//str = String::Format(L"CameraPos %.2f, %.2f, %.2f", vec.x, vec.y, vec.z);
	//DirectWrite::RenderText(str, rect, 12);
	//
	//rect.top += space;
	//D3DXVECTOR2 rot;
	//values->MainCamera->RotationDegree(&rot);
	//str = String::Format(L"CameraRot %.2f, %.2f", rot.x, rot.y);
	//DirectWrite::RenderText(str, rect, 12);
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