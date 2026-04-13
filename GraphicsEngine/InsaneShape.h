#pragma once
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;

class InsaneShape
{
public:
	InsaneShape();
	~InsaneShape();

	bool Initialize(ID3D11Device* aDevice, const float aScreenPosition[2], const float aColor[3]);
	void Render(ID3D11DeviceContext* aDeviceContext);
private:
	ComPtr<ID3D11Buffer> myVertexBuffer;
	ComPtr<ID3D11Buffer> myIndexBuffer;
	ComPtr<ID3D11VertexShader> myVertexShader;
	ComPtr<ID3D11PixelShader> myPixelShader;
	ComPtr<ID3D11InputLayout> myInputLayout;
};