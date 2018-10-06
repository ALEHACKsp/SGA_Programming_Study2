#include "stdafx.h"
#include "Window.h"

int WINAPI WinMain(
	HINSTANCE instance, HINSTANCE prevInstance, LPSTR param, int command)
{
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