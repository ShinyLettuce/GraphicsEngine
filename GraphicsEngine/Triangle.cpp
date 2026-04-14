#include "Triangle.h"

#include <d3d11.h>
#include <fstream>

#include <cmath>

struct alignas(16) Vertex
{
	float x, y, z, w;
	float r, g, b, a;
};

Triangle::Triangle() = default;
Triangle::~Triangle() = default;

bool Triangle::Initialize(ID3D11Device* aDevice)
{
	HRESULT result;

	Vertex vertices[3]
	{
		{ -0.2f, -0.2f, 0, 1, 1, 0, 0, 1},
		{ 0.0f, 0.2f, 0, 1, 0, 1, 0, 1},
		{ 0.2f, -0.2f, 0, 1, 0, 0, 1, 1}
	};

	unsigned int indices[3]
	{
		0, 1, 2
	};

	float farClip = 10.f;
	float nearClip = 0.1f;
	float Yfov = 2.44f;

	float zoomY = 1.f / tan(Yfov * 0.5f);
	float zoomX = zoomY / 1.77777f;

	myCamera = {
		zoomX, 0.f, 0.f, 0.f,
		0.f, zoomY, 0.f, 0.f,
		0.f, 0.f, -(farClip + nearClip) / (farClip - nearClip), -(2.f * nearClip * farClip) / (farClip - nearClip),
		0.f, 0.f, 0.f, 0.f
	};

	/*myCamera = {
	1.f, 0.f, 0.f, 0.f,
	0.f, 2.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
	};*/

	{
		D3D11_BUFFER_DESC vertexBufferDesc{ 0 };
		vertexBufferDesc.ByteWidth = sizeof vertices;
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexData{ 0 };
		vertexData.pSysMem = vertices;

		result = aDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);

		if (FAILED(result))
		{
			return false;
		}
	}

	{
		D3D11_BUFFER_DESC indexBufferDesc{ 0 };
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof indices;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA indexData{ 0 };
		indexData.pSysMem = indices;

		result = aDevice->CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);

		if (FAILED(result))
		{
			return false;
		}
	}

	{
		D3D11_BUFFER_DESC bufferDescription{ 0 };
		bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
		bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDescription.ByteWidth = sizeof(BufferData::FrameBufferData);
		result = aDevice->CreateBuffer(&bufferDescription, nullptr, &myFrameBuffer);
		if (FAILED(result))
		{
			return false;
		}
		bufferDescription.ByteWidth = sizeof(BufferData::ObjectBufferData);
		result = aDevice->CreateBuffer(&bufferDescription, nullptr, &myObjectBuffer);
		if (FAILED(result))
		{
			return false;
		}
	}

	std::string vsData;
	{
		std::ifstream vsFile;
		vsFile.open("VertexShader.cso", std::ios::binary);
		vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
		result = aDevice->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &myVertexShader);

		if (FAILED(result))
		{
			return false;
		}

		vsFile.close();

		std::ifstream psFile;
		psFile.open("PixelShader.cso", std::ios::binary);
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
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		result = aDevice->CreateInputLayout(layout, 2, vsData.data(), vsData.size(), &myInputLayout);
		if (FAILED(result))
		{
			return false;
		}
	}

	return true;
}

void Triangle::Render(ID3D11DeviceContext* aDeviceContext)
{
	{
		//set up camera on the GPU

		BufferData::FrameBufferData frameBufferData = {};
		frameBufferData = myCamera;
		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		aDeviceContext->Map(myFrameBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, &frameBufferData, sizeof(BufferData::FrameBufferData));
		aDeviceContext->Unmap(myFrameBuffer.Get(), 0);
		aDeviceContext->VSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
	}

	{
		//Prepare ObjectBufferData, set up transform of the object on the GPU
		BufferData::ObjectBufferData objectBufferData = {};
		objectBufferData = {1.f, 0.f, 0.f, 0.f,
							0.f, 1.f, 0.f, 0.f,
							0.f, 0.f, 1.f, 0.f,
							0.f, 0.f, 0.f, 1.f};

		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		aDeviceContext->Map(myObjectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, &objectBufferData, sizeof(BufferData::ObjectBufferData));
		aDeviceContext->Unmap(myObjectBuffer.Get(), 0);

		aDeviceContext->VSSetConstantBuffers(1, 1, myObjectBuffer.GetAddressOf());
	}

	//Code to render mesh, REPLACE

	aDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	aDeviceContext->IASetInputLayout(myInputLayout.Get());
	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;
	aDeviceContext->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
	aDeviceContext->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	aDeviceContext->VSSetShader(myVertexShader.Get(), nullptr, 0);
	aDeviceContext->PSSetShader(myPixelShader.Get(), nullptr, 0);
	aDeviceContext->DrawIndexed(3, 0, 0);
}
