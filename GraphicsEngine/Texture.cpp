#include "Texture.h"

#include <d3d11.h>

bool Texture::Initialize(ID3D11Device* device, unsigned char* rgbaPixels, int width, int height)
{
	ID3D11Texture2D* texture = nullptr;
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA subresourceDesc = {};
	subresourceDesc.pSysMem = (void*)rgbaPixels;
	subresourceDesc.SysMemPitch = width * 4;
	subresourceDesc.SysMemSlicePitch = width * height * 4;
	if (FAILED(device->CreateTexture2D(&desc, &subresourceDesc, &texture)))
		return false;
	HRESULT hr = device->CreateShaderResourceView(texture, NULL, &myShaderResourceView);
	if (FAILED(hr))
	{
		return false;
	}
	texture->Release();
	return true;
}

void Texture::Bind(ID3D11DeviceContext* context, int slot)
{
	context->PSSetShaderResources(slot, 1, myShaderResourceView.GetAddressOf());
}
