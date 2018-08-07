#include "stdafx.h"
#include "Program.h"

#include "./Viewer/Freedom.h"
#include "./Viewer//Orbit.h"

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

	freedom = new Freedom();
	freedom->Position(0, 0, -150);
	orbit = new Orbit();

	values->mainCamera = orbit;

	camState = 1;

#if CASE
	ExeSphere* sphere = new ExeSphere(values);
	executes.push_back(sphere);
#else
	orbit->Position(0, 0, -150);
	orbit->SetMoveValue(-150);

	// test
	//ExeSphere* sphere = new ExeSphere(values, &D3DXVECTOR3(0,0,0));
	//executes.push_back(sphere);

	// Sun
	executes.push_back(new ExeSphere(values, new D3DXVECTOR3(0, 0, 0), 
		3, 0.25, false, 0, D3DXCOLOR(1,0,0,1)));
	// Earth
	executes.push_back(new ExeSphere(values, new D3DXVECTOR3(0, 0, 0), 
		1, 0.75, true, 75, D3DXCOLOR(0,0,1,1)));
	// Moon
	executes.push_back(new ExeSphere(values, 
		executes[1]->GetPosition(), 0.3f, 2.0f, true, 10));

#endif
}

Program::~Program()
{
	for (Execute* exe : executes)
		SAFE_DELETE(exe);

	SAFE_DELETE(values->ViewProjection);
	SAFE_DELETE(values->Perspective);
	SAFE_DELETE(values->Viewport);
	SAFE_DELETE(values);

	SAFE_DELETE(orbit);
	SAFE_DELETE(freedom);

	States::Delete();
}

void Program::Update()
{
	if (camState == 1)
		values->mainCamera = orbit;
	else
		values->mainCamera = freedom;

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

	ImGui::SliderInt("Camera", &camState, 0, 1);

	D3DXVECTOR3 vec;
	values->mainCamera->Position(&vec);
	ImGui::Text("Camera Pos: %.1f %.1f %.1f", vec.x, vec.y, vec.z);

	D3DXVECTOR2 vec2;
	values->mainCamera->Rotation(&vec2);
	ImGui::Text("Camera Rotate: %.2f %.2f", vec2.x, vec2.y);
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