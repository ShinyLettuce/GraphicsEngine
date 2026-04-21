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
struct ID3D11DepthStencilView;

class GraphicsEngine
{
public:
	GraphicsEngine();
	~GraphicsEngine();
	bool Initialize(HWND windowHandle);
	void Update(const InputHandler& aInput, float aDeltaTime);
	void Render();
private:
	ComPtr<ID3D11Device> myDevice;
	ComPtr<ID3D11DeviceContext> myContext;
	ComPtr<IDXGISwapChain> mySwapChain;
	ComPtr<ID3D11RenderTargetView> myBackBuffer;
	ComPtr<ID3D11DepthStencilView> myDepthBuffer;

	ComPtr<ID3D11Buffer> myPerFrameBuffer;

	Camera myCamera;
	Mesh myPyramidMesh;
	Mesh myCubeMesh;
	Mesh myHandMesh;
	Mesh myDragonMesh;

	float myTime{ 0.0f };
};