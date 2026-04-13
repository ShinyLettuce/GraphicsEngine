#pragma once

#include "Triangle.h"
#include "Object.h"
#include "InsaneShape.h"

#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;

class GraphicsEngine
{
public:
	GraphicsEngine();
	~GraphicsEngine();
	bool Initialize(HWND windowHandle);
	void Render();
private:
	ComPtr<ID3D11Device> myDevice;
	ComPtr<ID3D11DeviceContext> myContext;
	ComPtr<IDXGISwapChain> mySwapChain;
	ComPtr<ID3D11RenderTargetView>myBackBuffer;

	Triangle myTriangle;
	InsaneShape mySquare;
	Object myObject;
};