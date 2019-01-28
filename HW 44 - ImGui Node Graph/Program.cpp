#include "stdafx.h"
#include "Program.h"

#include "./Model/Model.h"

#include "./Viewer/Freedom.h"
#include "./Viewer/Perspective.h"
#include "./Viewer/Viewport.h"

#include "./Executes/Export.h"

#include "./Executes/TestImGui.h"

Program::Program()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	Context::Create();

	Context::Get()->GetMainCamera()->Position(0, 0, -5);

	//executes.push_back(new Export);

	executes.push_back(new TestImGui());
}

Program::~Program()
{
	for (Execute* exe : executes)
		SAFE_DELETE(exe);

	Models::Delete();

	Textures::Delete();
	Context::Delete();

	Shaders::Delete();
}

void Program::Update()
{
	Context::Get()->Update();

	for (Execute* exe : executes)
		exe->Update();
}

void Program::PreRender()
{
	for (Execute* exe : executes)
		exe->PreRender();
}

void Program::Render()
{
	ImGui::Text("FPS : %f", Time::Get()->FPS());

	D3DXVECTOR3 vec;
	Context::Get()->GetMainCamera()->Position(&vec);
	ImGui::Text("CameraPos %.2f, %.2f, %.2f", vec.x, vec.y, vec.z);

	D3DXVECTOR2 rot;
	Context::Get()->GetMainCamera()->RotationDegree(&rot);
	ImGui::Text("CameraRot %.2f, %.2f", rot.x, rot.y);

	if (ImGui::DragFloat3("Direction", (float*)&Context::Get()->GetGlobalLight()->Direction, 0.1f))
		Context::Get()->ChangeGlobalLight();

	for (Execute* exe : executes)
		exe->Render();
}

void Program::PostRender()
{
	for (Execute* exe : executes)
		exe->PostRender();

	//int space = 13;
	//RECT rect = { 0,0,300,300 };

	//wstring str = String::Format(L"FPS : %.0f", ImGui::GetIO().Framerate);
	//DirectWrite::RenderText(str, rect, 12);

	//rect.top += space;
	//D3DXVECTOR3 vec;
	//Context::Get()->GetMainCamera()->Position(&vec);
	//str = String::Format(L"CameraPos %.2f, %.2f, %.2f", vec.x, vec.y, vec.z);
	//DirectWrite::RenderText(str, rect, 12);

	//rect.top += space;
	//D3DXVECTOR2 rot;
	//Context::Get()->GetMainCamera()->RotationDegree(&rot);
	//str = String::Format(L"CameraRot %.2f, %.2f", rot.x, rot.y);
	//DirectWrite::RenderText(str, rect, 12);
}

void Program::ResizeScreen()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	Context::Get()->GetPerspective()->Set(desc.Width, desc.Height);
	Context::Get()->GetViewport()->Set(desc.Width, desc.Height);

	for (Execute* exe : executes)
		exe->ResizeScreen();
}