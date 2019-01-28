#include "stdafx.h"
#include "Main.h"
#include "Systems\Window.h"
#include "Viewer\Freedom.h"

#include "Executes\TestByteAddress.h"

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#ifdef _DEBUG
#define DBG_NEW new(_CLIENT_BLOCK,__FILE__, __LINE__)
#endif

void Main::Initialize()
{
	srand((UINT)time(NULL));

	Context::Create();

	Context::Get()->GetGlobalLight()->Direction = D3DXVECTOR3(-1, -1, 1);
	Context::Get()->ChangeGlobalLight();

	Context::Get()->GetMainCamera()->RotationDegree(15, 0);
	Context::Get()->GetMainCamera()->Position(0, 1.5f, -3);

	((Freedom*)Context::Get()->GetMainCamera())->MoveSpeed(10.0f);
	//((Freedom*)Context::Get()->GetMainCamera())->RotationSpeed(20.0f);

	Push(new TestByteAddress);
}

void Main::Ready()
{
	for (IExecute* exe : executes)
		exe->Ready();
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
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//_CrtSetBreakAlloc(521); // xml document
	
	// IExecute ¼Ò¸êÀÚ virtual ¹®Á¦
	//_CrtSetBreakAlloc(2197);
	//_CrtSetBreakAlloc(478);

	//_CrtSetBreakAlloc(487);
	
	//_CrtSetBreakAlloc(486);

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