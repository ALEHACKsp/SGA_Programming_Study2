#include "stdafx.h"
#include "Program.h"

#include "./Viewer/Freedom.h"

#include "./Executes/ExeGrid.h"
#include "./Executes/ExeSphere.h"

Program::Program()
{
	States::Create();

	D3DDesc desc;
	D3D::GetDesc(&desc);

	values = new ExecuteValues();
	values->ViewProjection = new ViewProjectionBuffer();
	values->Perspective = new Perspective(desc.Width, desc.Height);
	values->Viewport = new Viewport(desc.Width, desc.Height);

	values->mainCamera = new Freedom(20.0f, 1.5f);

	values->mainCamera->Rotation(0.43f, -0.33f);
	values->mainCamera->Position(6.4f, 3.7f, -5.4f);

	// 0.43 -0.33
	// 6.4 3.7 -5.4

	executes.push_back(new ExeSphere(values));
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