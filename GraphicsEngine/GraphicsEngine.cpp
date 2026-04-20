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

	bool success;

	success = myPyramidMesh.Init(myDevice.Get(),
		{
			{ -1.0f, 0.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
			{ 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
			{ 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
			{ -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f },
			{ 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f },
		},
		{
			// Top faces
			0, 4, 1,
			1, 4, 2,
			2, 4, 3,
			3, 4, 0,
			// Bottom face
			2, 3, 0,
			0, 1, 2
		});

	success = myCubeMesh.Init(myDevice.Get(),
		{
			{ -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f },
			{ 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
			{ -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
			{ 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f },
			{ -1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
			{ 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f },
			{ -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
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

	Obj::Obj obj = Obj::LoadFromFile("LittleGuy.model");

	std::vector<Mesh::Vertex> vertices;
	for (const auto& objVertex : obj.vertices)
	{
		Mesh::Vertex vertex;
		vertex.position.x = objVertex.x;
		vertex.position.y = objVertex.y;
		vertex.position.z = objVertex.z;
		vertex.position.w = 1.0f;
		vertex.color.x = 1.0f;
		vertex.color.y = 1.0f;
		vertex.color.z = 1.0f;
		vertex.color.w = 1.0f;

		vertices.emplace_back(vertex);
	}

	std::vector<Mesh::Index> indices;
	for (const auto& objIndex: obj.indices)
	{
		Mesh::Index index = objIndex;

		indices.emplace_back(index);
	}

	success = myLittleGuyMesh.Init(myDevice.Get(), std::move(vertices), std::move(indices));

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

	float deltaRotationAroundY = 0.f;
	float deltaRotationAroundX = 0.f;

	Vector3<float> deltaDir{ 0.0f, 0.0f, 0.0f };

	if (!aInput.isButtonDown(2))
	{
		return;
	}

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

	if (aInput.IsKeyDown('Q'))
	{
		deltaDir.y -= 1.f;
	}
	if (aInput.IsKeyDown('E'))
	{
		deltaDir.y += 1.f;
	}
	
	Vector2<int> mouseDelta = { (int)aInput.GetDeltaMousePosition().x, (int)aInput.GetDeltaMousePosition().y };
	if (mouseDelta.Length() > 0 )
	{
		deltaRotationAroundY += (float)mouseDelta.x;
		deltaRotationAroundX += (float)mouseDelta.y;
	}

	deltaDir.Normalize();

	deltaDir *= aDeltaTime * cameraSpeed;
	deltaDir = deltaDir * Matrix3x3<float>::CreateRotationAroundX(myCamera.GetRotation().x) * Matrix3x3<float>::CreateRotationAroundY(myCamera.GetRotation().y);

	myCamera.SetPosition3(myCamera.GetPosition() + deltaDir);
	myCamera.SetRotation({ myCamera.GetRotation() + Vector3<float>{deltaRotationAroundX, deltaRotationAroundY, 0.f} * aDeltaTime });
}

void GraphicsEngine::Render()
{
	float color[4] = { 1.0f, 0.3f, 0.2f, 1.0f };
	myContext->ClearRenderTargetView(myBackBuffer.Get(), color);

	Matrix4x4<float> transform = {
		Matrix4x4<float>::CreateRotationAroundX(myCamera.GetRotation().x) *
		Matrix4x4<float>::CreateRotationAroundY(myCamera.GetRotation().y) *
		Matrix4x4<float>{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		myCamera.GetPosition().x,myCamera.GetPosition().y,myCamera.GetPosition().z,1,
		}
	};

	BufferData::FrameBufferData bufferData = { transform.GetFastInverse() * myCamera.GetFrameBufferData().worldToClipMatrix };

	myPyramidMesh.Render(myContext.Get(), { 2.0f, 1.0f, 10.0f }, bufferData);
	myCubeMesh.Render(myContext.Get(), { -1.0f, 0.0f, 4.0f }, bufferData);
	myLittleGuyMesh.Render(myContext.Get(), { 0.0f, -3.0f, 6.0f }, bufferData);

	mySwapChain->Present(1, 0);
}