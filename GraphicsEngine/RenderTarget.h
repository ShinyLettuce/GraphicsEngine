#pragma once
#include <wrl/client.h>

struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;

using Microsoft::WRL::ComPtr;

struct RenderTarget
{
	ComPtr<ID3D11RenderTargetView> renderTargetView;
	ComPtr<ID3D11ShaderResourceView> shaderResourceView;
};