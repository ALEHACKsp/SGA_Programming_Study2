#include "stdafx.h"
#include "./Systems/Window.h"

#include <crtdbg.h>

#ifdef _DEBUG
#define new new(_CLIENT_BLOCK,__FILE__, __LINE__)
#endif

int WINAPI WinMain(
	HINSTANCE instance, HINSTANCE prevInstance, LPSTR param, int command)
{
	// ���α׷� ����� ���� �ѹ��� ����
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Material buffer
	//_CrtSetBreakAlloc(1681); // ���� �޸� ��ȣ
	//_CrtSetBreakAlloc(1061); // ���� �޸� ��ȣ
	//_CrtSetBreakAlloc(1030); // ���� �޸� ��ȣ

	//_CrtSetBreakAlloc(3024); // ���� �޸� ��ȣ
	//_CrtSetBreakAlloc(3005); // ���� �޸� ��ȣ
	//_CrtSetBreakAlloc(1133); // ���� �޸� ��ȣ
	//_CrtSetBreakAlloc(1131); // ���� �޸� ��ȣ
	//_CrtSetBreakAlloc(1130); // ���� �޸� ��ȣ
	//_CrtSetBreakAlloc(1129); // ���� �޸� ��ȣ

	D3DDesc desc; // dx11 ������ �����ϴ� ����ü desc��� �̸� ����
	// �������� ����ü ����Ű�

	desc.AppName = L"D3D Game";
	desc.Instance = instance; 
	desc.bFullScreen = false;
	desc.bVsync = false;
	desc.Handle = NULL; // ������ ��������� �ڵ鰪 ������ ������ �ȸ������
	desc.Width = 1280; // ȭ�� ũ�� �ƴ� �������� ���� ũ��
	desc.Height = 720; // ���� HD ����(1280 x 720)�ε� �̰� ���� ����� 
	D3D::SetDesc(desc);

	Window* window = new Window();
	WPARAM wParam = window->Run();
	SAFE_DELETE(window);

	// wParam : ���α׷��� ���� ���¸� ������ ����
	// ���ӿ��� ��� �߿��Ѱ� �ƴ�
	return wParam;
}