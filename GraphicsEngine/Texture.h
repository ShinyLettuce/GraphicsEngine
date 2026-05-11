#pragma once
#include <wrl/client.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;

using Microsoft::WRL::ComPtr;

class Texture
{
public:
	bool Initialize(ID3D11Device* aDevice, ID3D11DeviceContext* aContext, unsigned char* aRGBAPixels, int aWidth, int aHeight, bool aUseSRGB = true);

	void Bind(ID3D11DeviceContext* context, int slot);
private:
	ComPtr<ID3D11ShaderResourceView> myShaderResourceView;
};