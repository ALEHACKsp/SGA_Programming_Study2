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

	values->mainCamera = new Freedom(50.0f);
	values->mainCamera->Rotation(0.87f, 0);
	values->mainCamera->Position(125.36f, 211.03f, -73.78f);

	executes.push_back(new ExeGrid(values));

	// 125.36f, 211.03f, -73.78f
	// 0.87f 0
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
	ImGui::Text("FPS : %f", Time::Get()->FPS());
	
	D3DXVECTOR3 position;
	values->mainCamera->Position(&position);
	ImGui::Text("Camera Pos: (%.2f %.2f %.2f)",
		position.x, position.y, position.z);
	D3DXVECTOR2 rotation;
	values->mainCamera->Rotation(&rotation);
	ImGui::Text("Camera Rot: (%.2f, %.2f)",
		rotation.x, rotation.y);

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