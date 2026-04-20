#include "GraphicsEngine.h"

#include "ObjLoader.h"

#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")

#include <cmath>

#include "Vector.h"

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

	//const bool success = myMesh.Init(myDevice.Get(),
	//	{
	//		{ -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
	//		{  0.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
	//		{  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f },
	//	},
	//	{
	//		0, 1, 2
	//	});

	bool success;

	success = myPyramidMesh.Init(myDevice.Get(),
		{
			{ -1.0f, 0.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
			{ 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
			{ -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f },
			{ 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
			{ 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f },
		},
		{
			0, 4, 1,
			1, 4, 2,
			2, 4, 3,
			3, 4, 0
		});

	success = myCubeMesh.Init(myDevice.Get(),
		{
			{ -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
			{ 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
			{ -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
			{ 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
			{ -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
			{ 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
			{ -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
		},
		{
			// Front face
			0, 4, 5,
			5, 1, 0,
			// Top face
			4, 6, 7,
			7, 5, 4,
			// Back face
			3, 7, 6,
			6, 2, 3,
			// Bottom face
			2, 0, 1,
			1, 3, 2,
			// Left face
			2, 6, 4,
			4, 0, 2,
			// Right face
			1, 5, 7,
			7, 3, 1
		});

		if (!success)
		{
			return false;
		}

		float farClip = 1000.f;
		float nearClip = 0.1f;
		float Yfov = 90;
		float aspect = (9.0f / 16.0f);
		myCamera.Init(farClip, nearClip, Yfov, aspect);

		return true;
}

void GraphicsEngine::Update(const InputHandler& aInput, float aDeltaTime)
{
	static constexpr float cameraSpeed = 10.f;
	Vector3 deltaDir{ 0.0f, 0.0f, 0.0f };

	if (aInput.IsKeyDown('W'))
	{
		deltaDir.z += 1.f;
	}
	if (aInput.IsKeyDown('A'))
	{
		deltaDir.x -= 1.f;
	}
	if (aInput.IsKeyDown('S'))
	{
		deltaDir.z -= 1.f;
	}
	if (aInput.IsKeyDown('D'))
	{
		deltaDir.x += 1.f;
	}

	if (aInput.IsKeyDown(VK_SHIFT))
	{
		deltaDir.y -= 1.f;
	}
	if (aInput.IsKeyDown(VK_SPACE))
	{
		deltaDir.y += 1.f;
	}

	// Normalize
	float dirLength = sqrt(deltaDir.x * deltaDir.x + deltaDir.y * deltaDir.y + deltaDir.z * deltaDir.z);
	if (dirLength != 0)
	{
		deltaDir.x /= dirLength;
		deltaDir.y /= dirLength;
		deltaDir.z /= dirLength;
	}

	Vector3 CameraPosition = myCamera.GetPosition();
	CameraPosition.x -= deltaDir.x * aDeltaTime * cameraSpeed;
	CameraPosition.y -= deltaDir.y * aDeltaTime * cameraSpeed;
	CameraPosition.z -= deltaDir.z * aDeltaTime * cameraSpeed;

	myCamera.SetPosition3(CameraPosition);
}

void GraphicsEngine::Render()
{
	float color[4] = { 1.0f, 0.3f, 0.2f, 1.0f };
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

	myPyramidMesh.Render(myContext.Get(), { 2.0f, 1.0f, 10.0f }, myCamera.GetFrameBufferData());
	myCubeMesh.Render(myContext.Get(), { -1.0f, 0.0f, 4.0f }, myCamera.GetFrameBufferData());

	mySwapChain->Present(1, 0);
}