#include "GraphicsEngine.h"

#include "ObjLoader.h"

#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")

#include <cmath>

#define REPORT_DX_WARNINGS

GraphicsEngine::GraphicsEngine() = default;
GraphicsEngine::~GraphicsEngine() = default;

bool GraphicsEngine::Initialize(HWND windowHandle)
{
	HRESULT	result;

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = windowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = true;

	UINT creationFlags = 0;
#if defined(REPORT_DX_WARNINGS)
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	result = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&mySwapChain,
		&myDevice,
		nullptr,
		&myContext
	);

	ID3D11Texture2D* backBufferTexture;
	result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
	if (FAILED(result))
	{
		return false;
	}
	result = myDevice->CreateRenderTargetView(backBufferTexture, nullptr, &myBackBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_TEXTURE2D_DESC textureDesc;
	backBufferTexture->GetDesc(&textureDesc);
	backBufferTexture->Release();

	myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), nullptr);
	D3D11_VIEWPORT viewport = { };
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(textureDesc.Width);
	viewport.Height = static_cast<float>(textureDesc.Height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	myContext->RSSetViewports(1, &viewport);

	ObjLoader::Obj obj = ObjLoader::Load("C:/Users/vilgotoscardexter.b/source/repos/GraphicsEngine/GraphicsEngine/LittleGuy.model");

	const bool success = myMesh.Init(myDevice.Get(),
		{
			{ -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
			{  0.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
			{  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f },
		},
		{
			0, 1, 2
		});

	if (!success)
	{
		return false;
	}

	float squarePos[2] = { 1000.f, 200.f };

	squarePos[0] = (squarePos[0] * 2 / static_cast<float>(textureDesc.Width)) - 1;
	squarePos[1] = (squarePos[1] * 2 / static_cast<float>(textureDesc.Height)) - 1;

	float squareColor[3] = { 0.9f, 0.0f, 0.0f };

	return true;
}

void GraphicsEngine::Render()
{
	float color[4] = { 1.0f, 0.3f, 0.2f, 1.0f};
	myContext->ClearRenderTargetView(myBackBuffer.Get(), color);

	const float pi = 3.1415927f;
	const float deg2rad = pi / 180.f;

	float farClip = 1000.f;
	float nearClip = 0.1f;
	float Yfov = 90 * deg2rad;

	float zoomY = 1.f / tan(Yfov * 0.5f);
	float zoomX = zoomY * (9.0f / 16.0f);

	BufferData::FrameBufferData frameBufferData 
	{
		zoomX, 0.f, 0.f, 0.f,
		0.f, zoomY, 0.f, 0.f,
		0.f, 0.f, (farClip) / (farClip - nearClip), 1.f,
		0.f, 0.f, (-nearClip * farClip) / (farClip - nearClip), 0.f
	};

	myMesh.Render(myContext.Get(), frameBufferData);

	mySwapChain->Present(1, 0);
}