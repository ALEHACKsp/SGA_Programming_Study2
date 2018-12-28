#pragma once

class Window
{
public:
	static WPARAM Run(class IExecute* main);
	static void Progress(float val) { progress = val; }

private:
	static void Create();
	static void Destroy();

	static void ProgressRender();
	static void MainRender();

	static LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

private:
	static class IExecute* mainExecute;

	static bool bInitialize;
	static float progress; // 나중에 클래스로 만들어줘야함
	static mutex* mu;
};