#include "stdafx.h"
#include "./Systems/Window.h"

#include <crtdbg.h>

#ifdef _DEBUG
#define new new(_CLIENT_BLOCK,__FILE__, __LINE__)
#endif

int WINAPI WinMain(
	HINSTANCE instance, HINSTANCE prevInstance, LPSTR param, int command)
{
	// 프로그램 실행시 최초 한번만 세팅
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	///_CrtSetBreakAlloc(3168);
		// xml document delete 안해서 그런거
		//_CrtSetBreakAlloc(840);
		//_CrtSetBreakAlloc(839);
		//_CrtSetBreakAlloc(838);
		//_CrtSetBreakAlloc(837);
		//_CrtSetBreakAlloc(836);
	///_CrtSetBreakAlloc(823);

	// Shaders Delete 안해주고 있음
	//_CrtSetBreakAlloc(2962);
	//_CrtSetBreakAlloc(1686);

	// TestEnv에서 perspective delete 안해줘서 그런거
	//_CrtSetBreakAlloc(2910);

	//_CrtSetBreakAlloc(1222);

	//_CrtSetBreakAlloc(1578);

	//_CrtSetBreakAlloc(14362);

	//_CrtSetBreakAlloc(1012);

	srand((UINT)time(NULL));

	D3DDesc desc; // dx11 에서는 세팅하는 구조체 desc라는 이름 붙음
	// 선생님이 구조체 만드신거

	desc.AppName = L"D3D Game";
	desc.Instance = instance; 
	desc.bFullScreen = false;
	desc.bVsync = false;
	desc.Handle = NULL; // 윈도우 만들어지면 핸들값 넣을꺼 아직은 안만들어짐
	desc.Width = 1280; // 화면 크기 아님 렌더링될 영역 크기
	desc.Height = 720; // 보통 HD 비율(1280 x 720)인데 이거 많이 사용함 
	D3D::SetDesc(desc);

	Window* window = new Window();
	WPARAM wParam = window->Run();
	SAFE_DELETE(window);

	// wParam : 프로그램의 종료 상태를 가지고 있음
	// 게임에서 사실 중요한건 아님
	return wParam;
}