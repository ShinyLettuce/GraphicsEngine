#pragma once
#include "Matrix4x4.h"
#include "Matrix3x3.h"

namespace BufferData
{
	struct alignas(16) VertexFrameBufferData
	{
		Matrix4x4<float> worldToClipMatrix;
	};

	struct alignas(16) PixelFrameBufferData
	{
		float time;
	};

	struct alignas(16) PerObjectBufferData
	{
		Matrix4x4<float> modelToWorldMatrix;
	};
}