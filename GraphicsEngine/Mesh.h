#pragma once

#include "BufferData.h"
#include "Vector.h"

#include <wrl/client.h>

#include <vector>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;

using Microsoft::WRL::ComPtr;

class Mesh
{
public:
	struct alignas(16) Vertex
	{
		struct
		{
			float x, y, z, w;
		} position;
		struct
		{
			float x, y, z;
		} normal;
		struct
		{
			float x, y, z, w;
		} color;
	};

	using Index = unsigned int;

	bool Init(ID3D11Device* aDevice, const char* aVertexShaderPath, const char* aPixelShaderPath, const std::vector<Vertex>& aVertices, const std::vector<Index>& aIndices);

	void Render(ID3D11DeviceContext* aDeviceContext, Vector3<float> aTranslation);
private:
	std::vector<Vertex> myVertices;
	std::vector<Index> myIndices;

	ComPtr<ID3D11Buffer> myPerObjectBuffer;

	ComPtr<ID3D11Buffer> myVertexBuffer;
	ComPtr<ID3D11Buffer> myIndexBuffer;
	ComPtr<ID3D11VertexShader> myVertexShader;
	ComPtr<ID3D11PixelShader> myPixelShader;
	ComPtr<ID3D11InputLayout> myInputLayout;
};