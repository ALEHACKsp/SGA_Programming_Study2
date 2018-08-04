#include "stdafx.h"
#include "Program.h"

#include "./Viewer/Freedom.h"

#include "./Executes/ExeGrid.h"

Program::Program()
{
	States::Create();

	D3DDesc desc;
	D3D::GetDesc(&desc);

	values = new ExecuteValues();
	values->ViewProjection = new ViewProjectionBuffer();
	values->Perspective = new Perspective(desc.Width, desc.Height);
	values->Viewport = new Viewport(desc.Width, desc.Height);

	values->mainCamera = new Freedom(100, 1.5f);
	//values->mainCamera->Position(2.23f, 1.26, -3.93f);

	//values->mainCamera->Rotation(0.57f, 1.56f);
	//values->mainCamera->Position(-134.6f, 184.4f, 129.8f);

	values->mainCamera->Rotation(0.54f, 0);
	values->mainCamera->Position(144.5f, 163.0f, -146.6f);

	executes.push_back(new ExeGrid(values));

	//position = D3DXVECTOR3(0, 0, 0);
	//position = D3DXVECTOR3(2.23f, 1.26f, -3.93f);
	// 이 방법도 가능함
	//position = { 0, 0, 0 };

	// -134.6 184.4, 129.8
	// 0.57 1.56

	// 144.5 163.0 -146.6
	// 0.54 0
}

Program::~Program()
{
	for (Execute* exe : executes)
		SAFE_DELETE(exe);

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

	for (Execute* exe : executes)
		exe->Render();
}

void Program::PostRender()
{
	for (Execute* exe : executes)
		exe->PostRender();

	ImGui::Text("FPS : %f", Time::Get()->FPS());

	D3DXVECTOR3 vec;
	values->mainCamera->Position(&vec);
	ImGui::Text("Camera Pos: %.1f %.1f %.1f", vec.x, vec.y, vec.z);

	D3DXVECTOR2 vec2;
	values->mainCamera->Rotation(&vec2);
	ImGui::Text("Camera Pos: %.2f %.2f", vec2.x, vec2.y);
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