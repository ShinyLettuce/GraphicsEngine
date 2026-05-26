#include "GraphicsEngine.h"

#include "ObjLoader.h"
#include "stb_image.h"

#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")

#include <filesystem>
#include <cmath>

#include "uppgift_1_5_helper.h"
#include "Vector.h"

#define REPORT_DX_WARNINGS

GraphicsEngine::GraphicsEngine() = default;
GraphicsEngine::~GraphicsEngine() = default;

float Clamp01(float aValue)
{
	if (aValue > 1.0f)
	{
		return 1.0f;
	}
	else if (aValue < 0.0f)
	{
		return 0.0f;
	}
	else
	{
		return aValue;
	}
}

bool GraphicsEngine::Initialize(HWND windowHandle)
{
	HRESULT	result;

	// Init device and swapchain
	{
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
	}

	// Init back buffer
	{
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
	}

	// Init depth stencil and viewport
	{
		ComPtr<ID3D11Texture2D> backBufferTexture;

		result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
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

		D3D11_VIEWPORT viewport = { };
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = static_cast<float>(backBufferDesc.Width);
		viewport.Height = static_cast<float>(backBufferDesc.Height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		myContext->RSSetViewports(1, &viewport);
	}

	// Init per frame buffer
	{
		D3D11_BUFFER_DESC perFrameBuffer{ 0 };
		perFrameBuffer.Usage = D3D11_USAGE_DYNAMIC;
		perFrameBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		perFrameBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		perFrameBuffer.ByteWidth = sizeof(Buffer::PerFrameBuffer);
		result = myDevice->CreateBuffer(&perFrameBuffer, nullptr, &myPerFrameBuffer);
		if (FAILED(result))
		{
			return false;
		}
	}

	// Init sampler state
	{
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

		result = myDevice->CreateSamplerState(&samplerDesc, &mySamplerState);
		if (FAILED(result))
		{
			return false;
		}
	}

	myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), myDepthBuffer.Get());

	D3D11_RASTERIZER_DESC defaultRasterizerDesc{ };
	defaultRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	defaultRasterizerDesc.CullMode = D3D11_CULL_BACK;

	myDevice->CreateRasterizerState(&defaultRasterizerDesc, &myDefaultRasterizerState);

	D3D11_RASTERIZER_DESC raymarchRasterizerDesc{ };
	raymarchRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	raymarchRasterizerDesc.CullMode = D3D11_CULL_NONE;

	myDevice->CreateRasterizerState(&raymarchRasterizerDesc, &myRaymarchRasterizerState);

	D3D11_RASTERIZER_DESC planarReflectionRasterizerState{ };
	planarReflectionRasterizerState.FillMode = D3D11_FILL_SOLID;
	planarReflectionRasterizerState.CullMode = D3D11_CULL_FRONT;

	myDevice->CreateRasterizerState(&planarReflectionRasterizerState, &myPlanarReflectionRasterizerState);

	myContext->VSSetSamplers(0, 1, mySamplerState.GetAddressOf());
	myContext->PSSetSamplers(0, 1, mySamplerState.GetAddressOf());

	int initSize = 16;
	std::vector<float> noise(initSize * initSize, 0.0f);
	int octaves = 4;
	float noiseAmount = 1.0f;

	for (int i = 0; i < octaves; ++i)
	{
		AddNoise(noise, noiseAmount);
		noise = Upsample2X(noise, initSize * (1 << i));
		noiseAmount *= 0.25f;
	}

	std::vector<unsigned char> texture;
	for (float f : noise)
	{
		texture.emplace_back((float)Clamp01(f * 0.5f + 0.5f) * 255.0f);
		texture.emplace_back((float)Clamp01(f * 0.5f + 0.5f) * 255.0f);
		texture.emplace_back((float)Clamp01(f * 0.5f + 0.5f) * 255.0f);
		texture.emplace_back(0xff);
	}

	bool success;

	success = myPlaneMesh.InitPlane(myDevice.Get(), "PbrModelShaderVS.cso", "PbrModelShaderPS.cso", 128.0f, 128.0f, 256, 256, noise, initSize * (1 << octaves));

	success = myLesserPlaneMesh.Init(myDevice.Get(), "PbrModelShaderVS.cso", "WaterShaderPS.cso",
		{
			{ { -0.5f, 0.0f, -0.5f,  1.0f } },
			{ { -0.5f, 0.0f,  0.5f,  1.0f } },
			{ {  0.5f, 0.0f,  0.5f,  1.0f } },
			{ {  0.5f, 0.0f, -0.5f,  1.0f } },

		},
		{
			0,1,2,
			0,2,3
		});

	success = myCubeMesh.Init(myDevice.Get(), "VertexShader.cso", "RayMarchWater.cso",
	{
				{ { -1.0f, -1.0f, -1.0f,  1.0f } },
				{ {  1.0f, -1.0f, -1.0f,  1.0f } },
				{ { -1.0f, -1.0f,  1.0f,  1.0f } },
				{ {  1.0f, -1.0f,  1.0f,  1.0f } },
				{ { -1.0f,  1.0f, -1.0f,  1.0f } },
				{ {  1.0f,  1.0f, -1.0f,  1.0f } },
				{ { -1.0f,  1.0f,  1.0f,  1.0f } },
				{ {  1.0f,  1.0f,  1.0f,  1.0f } },
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

	success = myFullscreenQuad.Init(myDevice.Get(), "ShadowVS.cso", "ShadowPS.cso",
		{
			{ { -1.0f, -1.0f, 0.0f, 1.0f }, {}, {}, {}, { 0.0f, 0.0f } },
			{ { -1.0f,  1.0f, 0.0f, 1.0f }, {}, {}, {}, { 0.0f, 1.0f } },
			{ {  1.0f,  1.0f, 0.0f, 1.0f }, {}, {}, {}, { 1.0f, 1.0f } },
			{ {  1.0f, -1.0f, 0.0f, 1.0f }, {}, {}, {}, { 1.0f, 0.0f } },

		},
		{
			0,1,2,
			0,2,3
		});

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

	if (!myNoiseTexture.Initialize(myDevice.Get(), myContext.Get(), texture.data(), initSize * (1 << octaves), initSize * (1 << octaves)))
	{
		return false;
	}

	if (!myNormalTexture.Initialize(myDevice.Get(), myContext.Get(), "Textures/testnormal.png", false))
	{
		return false;
	}

	if (!myGrassTexture.Initialize(myDevice.Get(), myContext.Get(), "Textures/Grass_c.png", true))
	{
		return false;
	}

	if (!myGrassNormalTexture.Initialize(myDevice.Get(), myContext.Get(), "Textures/Grass_n.png", false))
	{
		return false;
	}

	if (!myGrassMaterialTexture.Initialize(myDevice.Get(), myContext.Get(), "Textures/Grass_m.png", false))
	{
		return false;
	}

	if (!myRockTexture.Initialize(myDevice.Get(), myContext.Get(), "Textures/Rock_c.png", true))
	{
		return false;
	}

	if (!myRockNormalTexture.Initialize(myDevice.Get(), myContext.Get(), "Textures/Rock_n.png", false))
	{
		return false;
	}

	if (!myRockMaterialTexture.Initialize(myDevice.Get(), myContext.Get(), "Textures/Rock_m.png", false))
	{
		return false;
	}

	if (!mySnowTexture.Initialize(myDevice.Get(), myContext.Get(), "Textures/Snow_c.png", true))
	{
		return false;
	}

	if (!mySnowNormalTexture.Initialize(myDevice.Get(), myContext.Get(), "Textures/Snow_n.png", false))
	{
		return false;
	}

	if (!mySnowMaterialTexture.Initialize(myDevice.Get(), myContext.Get(), "Textures/Snow_m.png", false))
	{
		return false;
	}

	if (!myCubeMap.InitializeDds(myDevice.Get(), myContext.Get(), L"Textures/cube_1024_preblurred_angle3_Skansen3.dds", false))
	{
		return false;
	}

	// Reflection render target
	{
		ComPtr<ID3D11Texture2D> backBufferTexture;

		result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
		if (FAILED(result))
		{
			return false;
		}
		D3D11_TEXTURE2D_DESC backBufferDesc{ 0 };
		backBufferTexture->GetDesc(&backBufferDesc);

		HRESULT result;
		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = backBufferDesc.Width;
		desc.Height = backBufferDesc.Height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		ID3D11Texture2D* texture;
		result = myDevice->CreateTexture2D(&desc, nullptr, &texture);
		assert(SUCCEEDED(result));
		result = myDevice->CreateShaderResourceView(texture, nullptr, &myWaterReflectionRenderTarget.shaderResourceView);
		assert(SUCCEEDED(result));
		result = myDevice->CreateRenderTargetView(texture, nullptr, &myWaterReflectionRenderTarget.renderTargetView);
		texture->Release();
	}

	// Shadow render target
	{
		ComPtr<ID3D11Texture2D> backBufferTexture;

		result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
		if (FAILED(result))
		{
			return false;
		}
		D3D11_TEXTURE2D_DESC backBufferDesc{ 0 };
		backBufferTexture->GetDesc(&backBufferDesc);

		HRESULT result;
		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = backBufferDesc.Width;
		desc.Height = backBufferDesc.Height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32G32_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		ID3D11Texture2D* texture;
		result = myDevice->CreateTexture2D(&desc, nullptr, &texture);
		assert(SUCCEEDED(result));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

		result = myDevice->CreateShaderResourceView(texture, nullptr, &myShadowMap.shaderResourceView);
		assert(SUCCEEDED(result));

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		result = myDevice->CreateRenderTargetView(texture, &rtvDesc, &myShadowMap.renderTargetView);
		texture->Release();
	}

	myNoiseTexture.Bind(myContext.Get(), 0);

	myGrassTexture.Bind(myContext.Get(), 1);
	myGrassNormalTexture.Bind(myContext.Get(), 2);
	myGrassMaterialTexture.Bind(myContext.Get(), 3);

	myRockTexture.Bind(myContext.Get(), 4);
	myRockNormalTexture.Bind(myContext.Get(), 5);
	myRockMaterialTexture.Bind(myContext.Get(), 6);

	mySnowTexture.Bind(myContext.Get(), 7);
	mySnowNormalTexture.Bind(myContext.Get(), 8);
	mySnowMaterialTexture.Bind(myContext.Get(), 9);

	myCubeMap.Bind(myContext.Get(), 10);

	ID3D11RenderTargetView* nullResource = nullptr;

	float color[4]{ 0.0f, 0.0f, 0.0f, 1.0f };

	myContext->OMSetRenderTargets(1, myShadowMap.renderTargetView.GetAddressOf(), nullptr);
	myContext->ClearRenderTargetView(myShadowMap.renderTargetView.Get(), color);
	myContext->RSSetState(myDefaultRasterizerState.Get());

	myFullscreenQuad.Render(myContext.Get(), { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });

	myContext->OMSetRenderTargets(1, &nullResource, nullptr);

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
	if (mouseDelta.Length() > 0)
	{
		deltaRotationAroundY += (float)mouseDelta.x;
		deltaRotationAroundX += (float)mouseDelta.y;
	}

	deltaDir.Normalize();

	deltaDir *= cameraSpeed * aDeltaTime;
	deltaDir = deltaDir * Matrix3x3<float>::CreateRotationAroundX(myCamera.GetRotation().x) * Matrix3x3<float>::CreateRotationAroundY(myCamera.GetRotation().y);

	myCamera.SetPosition3(myCamera.GetPosition() + deltaDir);
	myCamera.SetRotation({ myCamera.GetRotation() + Vector3<float>{deltaRotationAroundX, deltaRotationAroundY, 0.0f } *0.01f });
}

void GraphicsEngine::Render()
{
	const float color[4]{ 0.9f, 0.6f, 0.8f, 1.0f };
	const float black[4]{ 0.0f, 0.0f, 0.0f, 1.0f };

	{
		Buffer::PerFrameBuffer perFrameBuffer;
		perFrameBuffer.time = myTime;

		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		myContext->Map(myPerFrameBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, &perFrameBuffer, sizeof(Buffer::PerFrameBuffer));
		myContext->Unmap(myPerFrameBuffer.Get(), 0);
		myContext->PSSetConstantBuffers(2, 1, myPerFrameBuffer.GetAddressOf());
		myContext->VSSetConstantBuffers(2, 1, myPerFrameBuffer.GetAddressOf());
	}

	myNoiseTexture.Bind(myContext.Get(), 0);

	myGrassTexture.Bind(myContext.Get(), 1);
	myGrassNormalTexture.Bind(myContext.Get(), 2);
	myGrassMaterialTexture.Bind(myContext.Get(), 3);

	myRockTexture.Bind(myContext.Get(), 4);
	myRockNormalTexture.Bind(myContext.Get(), 5);
	myRockMaterialTexture.Bind(myContext.Get(), 6);

	mySnowTexture.Bind(myContext.Get(), 7);
	mySnowNormalTexture.Bind(myContext.Get(), 8);
	mySnowMaterialTexture.Bind(myContext.Get(), 9);

	myCubeMap.Bind(myContext.Get(), 10);

	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11RenderTargetView* nullRTV = nullptr;

	myContext->OMSetRenderTargets(1, myWaterReflectionRenderTarget.renderTargetView.GetAddressOf(), myDepthBuffer.Get());
	myContext->ClearRenderTargetView(myWaterReflectionRenderTarget.renderTargetView.Get(), color);
	myContext->ClearDepthStencilView(myDepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	myContext->RSSetState(myPlanarReflectionRasterizerState.Get());

	myCamera.BindUpsideDown(myContext.Get(), myTime);
	myPlaneMesh.Render(myContext.Get(), { 80.0f, 0.0f, 0.0f }, Vector3<float>{ 1.0f, 1.0f, 1.0f });

	myContext->OMSetRenderTargets(1, &nullRTV, nullptr);

	myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), myDepthBuffer.Get());
	myContext->ClearRenderTargetView(myBackBuffer.Get(), color);
	myContext->ClearDepthStencilView(myDepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	myContext->RSSetState(myDefaultRasterizerState.Get());

	myContext->PSSetShaderResources(11, 1, myWaterReflectionRenderTarget.shaderResourceView.GetAddressOf());
	myContext->PSSetShaderResources(12, 1, myShadowMap.shaderResourceView.GetAddressOf());

	myCamera.Bind(myContext.Get());
	myPlaneMesh.Render(myContext.Get(), { 80.0f, 0.0f, 0.0f }, Vector3<float>{ 1.0f, 1.0f, 1.0f });
	myLesserPlaneMesh.Render(myContext.Get(), { 80.0f, 0.0f, 0.0f }, Vector3<float>{ 128.f, 1.f, 128.f });

	myContext->PSSetShaderResources(11, 1, &nullSRV);
	myContext->PSSetShaderResources(12, 1, &nullSRV);

	mySwapChain->Present(1, 0);
}