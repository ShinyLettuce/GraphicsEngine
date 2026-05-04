#include "Mesh.h"

#include <string>
#include <fstream>

#include <d3d11.h>

bool Mesh::Init(ID3D11Device* aDevice, const char* aVertexShaderPath, const char* aPixelShaderPath, const std::vector<Vertex>& aVertices, const std::vector<Index>& aIndices)
{
	myVertices = aVertices;
	myIndices = aIndices;

	HRESULT result;

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
		const int numElements = 4;

		D3D11_INPUT_ELEMENT_DESC layout[numElements]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		result = aDevice->CreateInputLayout(layout, numElements, vsData.data(), vsData.size(), &myInputLayout);
		if (FAILED(result))
		{
			return false;
		}
	}

	return true;
}

bool Mesh::InitPlane(ID3D11Device* aDevice, const char* aVertexShaderPath, const char* aPixelShaderPath, float aWidth, float aHeight, int aResolutionWidth, int aResolutionHeight, const std::vector<float>& aTexture, int aTextureWidth)
{
	std::vector<Vertex> vertices;

	for (int j = 0; j < aResolutionWidth; ++j)
	{
		for (int i = 0; i < aResolutionHeight; ++i)
		{
			Vertex vertex;
			vertex.position.x = (float)i * (aWidth / aResolutionWidth) - aWidth * 0.5f;
			vertex.position.y = 0.0f;
			vertex.position.z = (float)j * (aHeight / aResolutionHeight) - aHeight * 0.5f;
			vertex.position.w = 1.0f;
			vertex.color.x = 1.0f;
			vertex.color.y = 1.0f;
			vertex.color.z = 1.0f;
			vertex.color.w = 1.0f;
			vertex.uv.u = (float)(j / (float)aResolutionWidth);
			vertex.uv.v = (float)(i / (float)aResolutionWidth);

			int index = static_cast<int>(std::floor(vertex.uv.u * aTextureWidth + vertex.uv.v * aTextureWidth * aTextureWidth));

			float normalizedHeight = aTexture[index];
			vertex.position.y = normalizedHeight;

			vertices.emplace_back(std::move(vertex));
		}
	}

	for (int j = 0; j < aResolutionWidth - 1; ++j)
	{
		for (int i = 0; i < aResolutionHeight - 1; ++i)
		{
			int indices[3]
			{
				i + j * aResolutionWidth,
				(i + 1) + j * aResolutionWidth,
				i + (j + 1) * aResolutionWidth
			};
			
			Vector3<float> positions[3]
			{
				{ vertices[indices[0]].position.x, vertices[indices[0]].position.y, vertices[indices[0]].position.z },
				{ vertices[indices[1]].position.x, vertices[indices[1]].position.y, vertices[indices[1]].position.z },
				{ vertices[indices[2]].position.x, vertices[indices[2]].position.y, vertices[indices[2]].position.z },
			};

			Vector3<float> normal = (positions[0] - positions[1]).Cross((positions[0] - positions[2])).GetNormalized();

			vertices[indices[0]].normal.x = normal.x;
			vertices[indices[0]].normal.y = normal.y;
			vertices[indices[0]].normal.z = normal.z;
		}
	}

	std::vector<Index> indices;

	for (int i = 0; i < aResolutionWidth - 1; ++i)
	{
		for (int j = 0; j < aResolutionHeight - 1; ++j)
		{
			Index face[4]
			{
				j * aResolutionWidth + i,
				j * aResolutionWidth + i + 1,
				(j + 1) * aResolutionWidth + i + 1,
				(j + 1) * aResolutionWidth + i,
			};

			indices.push_back(face[2]);
			indices.push_back(face[1]);
			indices.push_back(face[0]);
			indices.push_back(face[2]);
			indices.push_back(face[0]);
			indices.push_back(face[3]);
		}
	}

	return Init(aDevice, aVertexShaderPath, aPixelShaderPath, vertices, indices);
}

void Mesh::Render(ID3D11DeviceContext* aDeviceContext, Vector3<float> aTranslation, Vector3<float> aScaling)
{
	{
		Buffer::PerObjectBufferData objectBufferData{
			Matrix4x4<float>
			{
				aScaling.x, 0.f, 0.f, 0.f,
				0.f, aScaling.y, 0.f, 0.f,
				0.f, 0.f, aScaling.z, 0.f,
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
