#pragma once
#include <wrl/client.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;

using Microsoft::WRL::ComPtr;

class Texture
{
public:
	Texture();
	~Texture();
	bool Initialize(ID3D11Device* device, unsigned char* rgbaPixels, int width, int height);
	void Bind(ID3D11DeviceContext* context, int slot);
private:
	ComPtr<ID3D11ShaderResourceView> myShaderResourceView;
};