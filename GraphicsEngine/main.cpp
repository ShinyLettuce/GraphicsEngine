#include "CoolerWindows.h"
#include "GraphicsEngine.h"
#include "InputHandler.h"
#include "Timer.h"

#include "ObjLoader.h"

static InputHandler* locInput = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	locInput->UpdateEvents(message, wParam, lParam);
	switch (message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int main()
{
	InputHandler inputHandler;
	locInput = &inputHandler;
	Timer timer;

	HINSTANCE hInstance = GetModuleHandleA(nullptr);

	WNDCLASSEXW wcex = {};
	wcex.cbSize = sizeof wcex;
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszClassName = L"TGP_EXEMPEL_CLASS";

	RegisterClassExW(&wcex);

	int width = 1280;
	int height = 720;

	HWND hWnd = CreateWindowW(L"TGP_EXEMPEL_CLASS", L"TGP_EXEMPEL",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return 0;
	}

	ShowWindow(hWnd, 1);
	UpdateWindow(hWnd);
	inputHandler.InitHandle(hWnd);

	GraphicsEngine graphicsEngine;
	if (!graphicsEngine.Initialize(hWnd))
	{
		return 0;
	}

	MSG msg{};
	bool shouldRun = true;
	while (shouldRun)
	{
		while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);


			if (msg.message == WM_QUIT)
			{
				shouldRun = false;
			}
		}
		timer.Update();
		inputHandler.UpdateInput();

		graphicsEngine.Update(inputHandler, timer.GetDeltaTime());
		graphicsEngine.Render();
	}

	return (int)msg.wParam;
}