#include "GraphicsEngine.h"

#include "ObjLoader.h"
#include "stb_image.h"

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

	D3D11_BUFFER_DESC pixelBufferDesc{ 0 };
	pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelBufferDesc.ByteWidth = sizeof(Buffer::PerFrameBuffer);
	result = myDevice->CreateBuffer(&pixelBufferDesc, nullptr, &myPerFrameBuffer);

	if (FAILED(result))
	{
		return false;
	}

	{
		// Create a texture sampler state description.
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		// Create the texture sampler state.
		result = myDevice->CreateSamplerState(&samplerDesc, &mySamplerState);
		if (FAILED(result))
		{
			return false;
		}

		myContext->VSSetSamplers(0, 1, mySamplerState.GetAddressOf());
		myContext->PSSetSamplers(0, 1, mySamplerState.GetAddressOf());
	}

	myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), myDepthBuffer.Get());

	D3D11_VIEWPORT viewport = { };
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(backBufferDesc.Width);
	viewport.Height = static_cast<float>(backBufferDesc.Height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	myContext->RSSetViewports(1, &viewport);

	D3D11_RASTERIZER_DESC defaultRasterizerDesc{ };
	defaultRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	defaultRasterizerDesc.CullMode = D3D11_CULL_BACK;

	myDevice->CreateRasterizerState(&defaultRasterizerDesc, &myDefaultRasterizerState);

	D3D11_RASTERIZER_DESC raymarchRasterizerDesc{ };
	raymarchRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	raymarchRasterizerDesc.CullMode = D3D11_CULL_NONE;

	myDevice->CreateRasterizerState(&raymarchRasterizerDesc, &myRaymarchRasterizerState);

	bool success;

	success = myPyramidMesh.Init(myDevice.Get(), "VertexShader.cso", "PixelShader.cso",
		{
			{ -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
			{ 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
			{ 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
			{ -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f },
			{ 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 1.0f },
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

	success = myCubeMesh.Init(myDevice.Get(), "VertexShader.cso", "RayMarchWater.cso",
		{
			{ -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
			{ 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
			{ -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f },
			{ 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f },
			{ -1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
			{ 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f },
			{ -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f },
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
	
	//{
	//	Obj::Obj obj = Obj::LoadFromFile("Hand.model");
	//
	//	std::vector<Mesh::Vertex> vertices;
	//	std::vector<Mesh::Index> indices;
	//
	//	vertices.reserve(obj.vertices.size());
	//	vertices.resize(obj.vertices.size());
	//
	//	for (const auto& face : obj.faces)
	//	{
	//		vertices.at(face.vertex).position.x = obj.vertices.at(face.vertex).x;
	//		vertices.at(face.vertex).position.y = obj.vertices.at(face.vertex).y;
	//		vertices.at(face.vertex).position.z = obj.vertices.at(face.vertex).z;
	//		vertices.at(face.vertex).position.w = 1.0f;
	//
	//		vertices.at(face.vertex).normal.x = obj.normals.at(face.normal).x;
	//		vertices.at(face.vertex).normal.y = obj.normals.at(face.normal).y;
	//		vertices.at(face.vertex).normal.z = obj.normals.at(face.normal).z;
	//
	//		indices.push_back(face.vertex);
	//	}
	//
	//	success = myHandMesh.Init(myDevice.Get(), "VertexShader.cso", "CrazyPixelShader.cso", vertices, indices);
	//
	//	if (!success)
	//	{
	//		return false;
	//	}
	//}
	
	//{
	//	{
	//		Obj::Obj obj = Obj::LoadFromFile("Dragon.model");
	//
	//		std::vector<Mesh::Vertex> vertices;
	//		std::vector<Mesh::Index> indices;
	//
	//		vertices.reserve(obj.vertices.size());
	//		vertices.resize(obj.vertices.size());
	//
	//		for (const auto& face : obj.faces)
	//		{
	//			vertices.at(face.vertex).position.x = obj.vertices.at(face.vertex).x;
	//			vertices.at(face.vertex).position.y = obj.vertices.at(face.vertex).y;
	//			vertices.at(face.vertex).position.z = obj.vertices.at(face.vertex).z;
	//			vertices.at(face.vertex).position.w = 1.0f;
	//
	//			vertices.at(face.vertex).normal.x = obj.normals.at(face.normal).x;
	//			vertices.at(face.vertex).normal.y = obj.normals.at(face.normal).y;
	//			vertices.at(face.vertex).normal.z = obj.normals.at(face.normal).z;
	//
	//			indices.push_back(face.vertex);
	//		}
	//
	//		success = myDragonMesh.Init(myDevice.Get(), "VertexShader.cso", "InsanePixelShader.cso", vertices, indices);
	//
	//		if (!success)
	//		{
	//			return false;
	//		}
	//	}
	//}
	
	if (!success)
	{
		return false;
	}

	const float farClip = 1000.f;
	const float nearClip = 0.1f;
	const float Yfov = 90.f;
	const float aspect = (9.0f / 16.0f);
	success = myCamera.Init(myDevice.Get(), farClip, nearClip, Yfov, aspect);
	if (!success)
	{
		return false;
	}

	int width;
	int height;
	int channels;
	unsigned char* image = stbi_load("qrikko.jpg", &width, &height, &channels, 4);
	if (image == nullptr)
	{
		return false;
	}

	success = myLoadedTexture.Initialize(myDevice.Get(), image, width, height);

	stbi_image_free(image);

	if (!success)
	{	
		return false;
	}

	struct PixelColor
	{
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};

	constexpr int arrWidth = 256;
	unsigned char codeTexture[arrWidth * arrWidth * sizeof(PixelColor)] = {0};

	for (int i = 0; i < arrWidth; ++i)
	{
		for (int j = 0; j < arrWidth; ++j)
		{
			PixelColor color = {.r = (unsigned char)j, .g = (unsigned char)i, .b = 0, .a = 1};

			int currentPixel = (i * arrWidth + j) * sizeof(PixelColor);

			codeTexture[currentPixel] = color.r;
			codeTexture[currentPixel + 1] = color.g;
			codeTexture[currentPixel + 2] = color.b;
			codeTexture[currentPixel + 3] = color.a;
		}
	}

	success = myCodeTexture.Initialize(myDevice.Get(), codeTexture, 256, 256);

	if (!success)
	{
		return false;
	}

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
	const float color[4]{ 0.05f, 0.05f, 0.05f, 1.0f };
	myContext->ClearRenderTargetView(myBackBuffer.Get(), color);
	myContext->ClearDepthStencilView(myDepthBuffer.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	{
		Buffer::PerFrameBuffer perFrameBuffer;
		perFrameBuffer.time = myTime;

		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		myContext->Map(myPerFrameBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, &perFrameBuffer, sizeof(Buffer::PerFrameBuffer));
		myContext->Unmap(myPerFrameBuffer.Get(), 0);
		myContext->PSSetConstantBuffers(1, 1, myPerFrameBuffer.GetAddressOf());
	}

	myCamera.Bind(myContext.Get());

	myLoadedTexture.Bind(myContext.Get(), 0);
	myCodeTexture.Bind(myContext.Get(), 1);


	myContext->RSSetState(myDefaultRasterizerState.Get());
	myPyramidMesh.Render(myContext.Get(), { 15.0f, 0.2f, 0.f });
	//myHandMesh.Render(myContext.Get(), { 0.0f, -0.2f, 7.0f });
	//myDragonMesh.Render(myContext.Get(), { 0.0f, 0.0f, 5.5f });

	myContext->RSSetState(myRaymarchRasterizerState.Get());
	myCubeMesh.Render(myContext.Get(), myCamera.GetPosition());

	mySwapChain->Present(1, 0);
}