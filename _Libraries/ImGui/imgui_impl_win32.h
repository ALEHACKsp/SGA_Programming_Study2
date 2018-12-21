// dear imgui: Platform Binding for Windows (standard windows API for 32 and 64 bits applications)
// This needs to be used along with a Renderer (e.g. DirectX11, OpenGL3, Vulkan..)

// Implemented features:
//  [X] Platform: Clipboard support (for Win32 this is actually part of core imgui)
//  [X] Platform: Mouse cursor shape and visibility. Disable with 'io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange'.
//  [X] Platform: Keyboard arrays indexed using VK_* Virtual Key Codes, e.g. ImGui::IsKeyPressed(VK_SPACE).
//  [X] Platform: Multi-viewport support (multiple windows). Enable with 'io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable'.
// Missing features:
//  [ ] Platform: Gamepad support (best leaving it to user application to fill io.NavInputs[] with gamepad inputs from their source of choice).

#pragma once

IMGUI_IMPL_API bool     ImGui_ImplWin32_Init(void* hwnd);
IMGUI_IMPL_API void     ImGui_ImplWin32_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplWin32_NewFrame();

// DPI-related helpers (which run and compile without requiring 8.1 or 10, neither Windows version, neither associated SDK)
IMGUI_API void        ImGui_ImplWin32_EnableDpiAwareness();
IMGUI_API float       ImGui_ImplWin32_GetDpiScaleForHwnd(void* hwnd);       // HWND hwnd
IMGUI_API float       ImGui_ImplWin32_GetDpiScaleForMonitor(void* monitor); // HMONITOR monitor
IMGUI_API float       ImGui_ImplWin32_GetDpiScaleForRect(int x1, int y1, int x2, int y2);

// Handler for Win32 messages, update mouse/keyboard data.
// You may or not need this for your implementation, but it can serve as reference for handling inputs.
// Intentionally commented out to avoid dragging dependencies on <windows.h> types. You can copy the extern declaration in your code.
/*
IMGUI_IMPL_API LRESULT  ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
*/

//IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	if (ImGui::GetCurrentContext() == NULL)
//		return 0;
//
//	ImGuiIO& io = ImGui::GetIO();
//	switch (msg)
//	{
//	case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
//	case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
//	case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
//	{
//		int button = 0;
//		if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) button = 0;
//		if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) button = 1;
//		if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) button = 2;
//		if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
//			::SetCapture(hwnd);
//		io.MouseDown[button] = true;
//		return 0;
//	}
//	case WM_LBUTTONUP:
//	case WM_RBUTTONUP:
//	case WM_MBUTTONUP:
//	{
//		int button = 0;
//		if (msg == WM_LBUTTONUP) button = 0;
//		if (msg == WM_RBUTTONUP) button = 1;
//		if (msg == WM_MBUTTONUP) button = 2;
//		io.MouseDown[button] = false;
//		if (!ImGui::IsAnyMouseDown() && ::GetCapture() == hwnd)
//			::ReleaseCapture();
//		return 0;
//	}
//	case WM_MOUSEWHEEL:
//		io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
//		return 0;
//	case WM_MOUSEHWHEEL:
//		io.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
//		return 0;
//	case WM_KEYDOWN:
//	case WM_SYSKEYDOWN:
//		if (wParam < 256)
//			io.KeysDown[wParam] = 1;
//		return 0;
//	case WM_KEYUP:
//	case WM_SYSKEYUP:
//		if (wParam < 256)
//			io.KeysDown[wParam] = 0;
//		return 0;
//	case WM_CHAR:
//		// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
//		if (wParam > 0 && wParam < 0x10000)
//			io.AddInputCharacter((unsigned short)wParam);
//		return 0;
//	case WM_SETCURSOR:
//		if (LOWORD(lParam) == HTCLIENT && ImGui_ImplWin32_UpdateMouseCursor())
//			return 1;
//		return 0;
//	}
//	return 0;
//}
