#include "stdafx.h"
#include "Main.h"
#include "Systems\Window.h"
#include "Viewer\Freedom.h"

#include "Executes\TestTerrain.h"

void Main::Initialize()
{
	Context::Create();

	Context::Get()->GetMainCamera()->RotationDegree(0, 0);
	Context::Get()->GetMainCamera()->Position(0, 0, -5);

	((Freedom*)Context::Get()->GetMainCamera())->MoveSpeed(100.0f);
	((Freedom*)Context::Get()->GetMainCamera())->RotationSpeed(20.0f);

	Push(new TestTerrain());
}

void Main::Ready()
{
}

void Main::Destroy()
{
	for (IExecute* exe : executes) {
		exe->Destroy();
		SAFE_DELETE(exe);
	}

	Context::Delete();

	Textures::Delete();
	Models::Delete();
	Shaders::Delete();
}

void Main::Update()
{
	Context::Get()->Update();

	for (IExecute* exe : executes)
		exe->Update();
}

void Main::PreRender()
{
	for (IExecute* exe : executes)
		exe->PreRender();
}

void Main::Render()
{
	// Viewport ¹Ù²Ü²¨¶ó ¼¼ÆÃÇÏ´Â°Å
	Context::Get()->GetViewport()->RSSetViewport();

	ImGui::Text("FPS : %f", Time::Get()->FPS());

	D3DXVECTOR3 vec;
	Context::Get()->GetMainCamera()->Position(&vec);
	ImGui::Text("CameraPos %.2f, %.2f, %.2f", vec.x, vec.y, vec.z);

	D3DXVECTOR2 rot;
	Context::Get()->GetMainCamera()->RotationDegree(&rot);
	ImGui::Text("CameraRot %.2f, %.2f", rot.x, rot.y);

	if (ImGui::DragFloat3("Direction", (float*)&Context::Get()->GetGlobalLight()->Direction, 0.1f))
		Context::Get()->ChangeGlobalLight();

	for (IExecute* exe : executes)
		exe->Render();
}

void Main::PostRender()
{
	for (IExecute* exe : executes)
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

void Main::ResizeScreen()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	if (Context::Get() != NULL) {
		Context::Get()->GetPerspective()->Set(desc.Width, desc.Height);
		Context::Get()->GetViewport()->Set(desc.Width, desc.Height);
	}
	
	for (IExecute* exe : executes)
		exe->ResizeScreen();
}

void Main::Push(IExecute * execute)
{
	executes.push_back(execute);
	
	execute->Initialize();
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR param, int command)
{
	D3DDesc desc;
	desc.AppName = L"D3D Game";
	desc.Instance = instance;
	desc.bFullScreen = false;
	desc.bVsync = false;
	desc.Handle = NULL;
	desc.Width = 1280;
	desc.Height = 720;
	D3D::SetDesc(desc);


	Main* main = new Main();
	WPARAM wParam = Window::Run(main);

	SAFE_DELETE(main);

	return wParam;
}