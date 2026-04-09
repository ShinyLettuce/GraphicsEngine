#include "CoolerWindows.h"
#include "GraphicsEngine.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
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

int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE,
	_In_ LPWSTR,
	_In_ int nCmdShow
)
{
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

	// SUPER COMMENT TIME

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

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

			graphicsEngine.Render();
		}
	}

	return (int)msg.wParam;
}