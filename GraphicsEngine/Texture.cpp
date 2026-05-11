#include "Texture.h"

#include <d3d11.h>

bool Texture::Initialize(ID3D11Device* aDevice, ID3D11DeviceContext* aContext, unsigned char* aRGBAPixels, int aWidth, int aHeight, bool aUseSRGB)
{
	ComPtr<ID3D11Texture2D> texture = nullptr;
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = aWidth;
	desc.Height = aHeight;
	desc.MipLevels = 0;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Format = aUseSRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	if (FAILED(aDevice->CreateTexture2D(&desc, nullptr, &texture)))
	{
		return false;
	}

	HRESULT result = aDevice->CreateShaderResourceView(texture.Get(), NULL, &myShaderResourceView);
	if (FAILED(result))
	{
		return false;
	}

	aContext->UpdateSubresource(texture.Get(), 0, nullptr, (void*)aRGBAPixels, aWidth * 4, aWidth * aHeight * 4);
	aContext->GenerateMips(myShaderResourceView.Get());

	return true;
}

void Texture::Bind(ID3D11DeviceContext* context, int slot)
{
	context->PSSetShaderResources(slot, 1, myShaderResourceView.GetAddressOf());
}
