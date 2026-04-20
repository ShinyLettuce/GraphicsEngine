#include "GraphicsEngine.h"

#include "ObjLoader.h"

#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")

#include <filesystem>
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

	ComPtr<ID3D11Texture2D> backBufferTexture;

	result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
	if (FAILED(result))
	{
		return false;
	}

	result = myDevice->CreateRenderTargetView(backBufferTexture.Get(), nullptr, &myBackBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_TEXTURE2D_DESC backBufferDesc{ 0 };
	backBufferTexture->GetDesc(&backBufferDesc);

	D3D11_TEXTURE2D_DESC depthTextureDesc{ 0 };
	depthTextureDesc.Width = backBufferDesc.Width;
	depthTextureDesc.Height = backBufferDesc.Height;
	depthTextureDesc.MipLevels = 1;
	depthTextureDesc.ArraySize = 1;
	depthTextureDesc.SampleDesc.Count = 1;
	depthTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	D3D11_DEPTH_STENCIL_VIEW_DESC depthBufferDesc{ };
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	ComPtr<ID3D11Texture2D> depthBufferTexture;

	myDevice->CreateTexture2D(&depthTextureDesc, nullptr, &depthBufferTexture);
	myDevice->CreateDepthStencilView(depthBufferTexture.Get(), &depthBufferDesc, &myDepthBuffer);

	myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), myDepthBuffer.Get());

	D3D11_VIEWPORT viewport = { };
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(backBufferDesc.Width);
	viewport.Height = static_cast<float>(backBufferDesc.Height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	myContext->RSSetViewports(1, &viewport);

	bool success;

	success = myPyramidMesh.Init(myDevice.Get(), "VertexShader.cso", "PixelShader.cso",
		{
			{ -1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
			{ 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
			{ 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f },
			{ -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f },
			{ 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f },
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

	success = myCubeMesh.Init(myDevice.Get(), "VertexShader.cso", "PixelShader.cso",
		{
			{ -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
			{ 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
			{ -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f },
			{ 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f },
			{ -1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
			{ 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f },
			{ -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f },
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

	{
		Obj::Obj obj = Obj::LoadFromFile("Hand.model");

		std::vector<Mesh::Vertex> vertices;
		std::vector<Mesh::Index> indices;

		vertices.reserve(obj.vertices.size());
		vertices.resize(obj.vertices.size());

		for (const auto& face : obj.faces)
		{
			vertices.at(face.vertex).position.x = obj.vertices.at(face.vertex).x;
			vertices.at(face.vertex).position.y = obj.vertices.at(face.vertex).y;
			vertices.at(face.vertex).position.z = obj.vertices.at(face.vertex).z;
			vertices.at(face.vertex).position.w = 1.0f;

			vertices.at(face.vertex).normal.x = obj.normals.at(face.normal).x;
			vertices.at(face.vertex).normal.y = obj.normals.at(face.normal).y;
			vertices.at(face.vertex).normal.z = obj.normals.at(face.normal).z;

			indices.push_back(face.vertex);
		}

		success = myHandMesh.Init(myDevice.Get(), "VertexShader.cso", "CrazyPixelShader.cso", vertices, indices);

		if (!success)
		{
			return false;
		}
	}

	{
		{
			Obj::Obj obj = Obj::LoadFromFile("Dragon.model");

			std::vector<Mesh::Vertex> vertices;
			std::vector<Mesh::Index> indices;

			vertices.reserve(obj.vertices.size());
			vertices.resize(obj.vertices.size());

			for (const auto& face : obj.faces)
			{
				vertices.at(face.vertex).position.x = obj.vertices.at(face.vertex).x;
				vertices.at(face.vertex).position.y = obj.vertices.at(face.vertex).y;
				vertices.at(face.vertex).position.z = obj.vertices.at(face.vertex).z;
				vertices.at(face.vertex).position.w = 1.0f;

				vertices.at(face.vertex).normal.x = obj.normals.at(face.normal).x;
				vertices.at(face.vertex).normal.y = obj.normals.at(face.normal).y;
				vertices.at(face.vertex).normal.z = obj.normals.at(face.normal).z;

				indices.push_back(face.vertex);
			}

			success = myDragonMesh.Init(myDevice.Get(), "VertexShader.cso", "InsanePixelShader.cso", vertices, indices);

			if (!success)
			{
				return false;
			}
		}
	}

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
	myTime += aDeltaTime;

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
	myContext->ClearDepthStencilView(myDepthBuffer.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	Matrix4x4<float> transform = {
		Matrix4x4<float>::CreateRotationAroundX(myCamera.GetRotation().x) *
		Matrix4x4<float>::CreateRotationAroundY(myCamera.GetRotation().y) *
		Matrix4x4<float>{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		myCamera.GetPosition().x, myCamera.GetPosition().y, myCamera.GetPosition().z,1,
		}
	};

	BufferData::VertexFrameBufferData vertexBufferData = { transform.GetFastInverse() * myCamera.GetFrameBufferData().worldToClipMatrix };
	BufferData::PixelFrameBufferData pixelBufferData = { myCamera.GetPosition(), myTime };

	myPyramidMesh.Render(myContext.Get(), { -2.0f, -5.0f, 10.0f }, vertexBufferData, pixelBufferData);
	myCubeMesh.Render(myContext.Get(), { 2.0f, -5.0f, 10.0f }, vertexBufferData, pixelBufferData);
	myHandMesh.Render(myContext.Get(), { 0.0f, 0.0f, 20.0f }, vertexBufferData, pixelBufferData);
	myDragonMesh.Render(myContext.Get(), { 0.0f, 0.0f, 18.5f }, vertexBufferData, pixelBufferData);

	mySwapChain->Present(1, 0);
}