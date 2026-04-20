#pragma once

#include "InputHandler.h"
#include "Camera.h"
#include "Mesh.h"

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
	void Update(const CommonUtilities::InputHandler& aInput);
	void Render();
private:
	ComPtr<ID3D11Device> myDevice;
	ComPtr<ID3D11DeviceContext> myContext;
	ComPtr<IDXGISwapChain> mySwapChain;
	ComPtr<ID3D11RenderTargetView>myBackBuffer;

	Camera myCamera;
	Mesh myMesh;
};