#include "Mesh.h"

#include <string>
#include <fstream>

#include <d3d11.h>

bool Mesh::Init(ID3D11Device* aDevice, const char* aVertexShaderPath, const char* aPixelShaderPath, const std::vector<Vertex>& aVertices, const std::vector<Index>& aIndices)
{
	myVertices = aVertices;
	myIndices = aIndices;

	HRESULT result;

	const float pi = 3.1415927f;
	const float deg2rad = pi / 180.f;

	float farClip = 1000.f;
	float nearClip = 0.1f;
	float Yfov = 90 * deg2rad;

	float zoomY = 1.f / tan(Yfov * 0.5f);
	float zoomX = zoomY * (9.0f / 16.0f);

	{
		D3D11_BUFFER_DESC vertexBufferDesc{ 0 };
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * (UINT)myVertices.size();
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexData{ 0 };
		vertexData.pSysMem = myVertices.data();

		result = aDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);

		if (FAILED(result))
		{
			return false;
		}
	}

	{
		D3D11_BUFFER_DESC indexBufferDesc{ 0 };
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(Index) * (UINT)myIndices.size();
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA indexData{ 0 };
		indexData.pSysMem = myIndices.data();

		result = aDevice->CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);
		if (FAILED(result))
		{
			return false;
		}
	}

	{
		D3D11_BUFFER_DESC perObjectBufferDesc{ 0 };
		perObjectBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		perObjectBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		perObjectBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		perObjectBufferDesc.ByteWidth = sizeof(Buffer::PerObjectBufferData);
		result = aDevice->CreateBuffer(&perObjectBufferDesc, nullptr, &myPerObjectBuffer);

		if (FAILED(result))
		{
			return false;
		}
	}

	std::string vsData;
	{
		std::ifstream vsFile;
		vsFile.open(aVertexShaderPath, std::ios::binary);
		vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
		result = aDevice->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &myVertexShader);

		if (FAILED(result))
		{
			return false;
		}

		vsFile.close();

		std::ifstream psFile;
		psFile.open(aPixelShaderPath, std::ios::binary);
		std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
		result = aDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &myPixelShader);

		if (FAILED(result))
		{
			return false;
		}

		psFile.close();
	}

	{
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		result = aDevice->CreateInputLayout(layout, 3, vsData.data(), vsData.size(), &myInputLayout);
		if (FAILED(result))
		{
			return false;
		}
	}

	return true;
}

void Mesh::Render(ID3D11DeviceContext* aDeviceContext, Vector3<float> aTranslation)
{
	{
		Buffer::PerObjectBufferData objectBufferData{
			Matrix4x4<float>
			{
				1.f, 0.f, 0.f, 0.f,
				0.f, 1.f, 0.f, 0.f,
				0.f, 0.f, 1.f, 0.f,
				aTranslation.x, aTranslation.y, aTranslation.z, 1.f
			} 
		};

		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		aDeviceContext->Map(myPerObjectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, &objectBufferData, sizeof(Buffer::PerObjectBufferData));
		aDeviceContext->Unmap(myPerObjectBuffer.Get(), 0);

		aDeviceContext->VSSetConstantBuffers(1, 1, myPerObjectBuffer.GetAddressOf());
	}

	aDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	aDeviceContext->IASetInputLayout(myInputLayout.Get());
	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;
	aDeviceContext->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
	aDeviceContext->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	aDeviceContext->VSSetShader(myVertexShader.Get(), nullptr, 0);
	aDeviceContext->PSSetShader(myPixelShader.Get(), nullptr, 0);
	aDeviceContext->DrawIndexed((UINT)myIndices.size(), 0, 0);
}
