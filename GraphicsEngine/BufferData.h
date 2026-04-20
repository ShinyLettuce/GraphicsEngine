#pragma once
#include "Matrix4x4.h"
#include "Matrix3x3.h"

namespace BufferData
{
	struct alignas(16) FrameBufferData
	{
		Matrix4x4<float> worldToClipMatrix;
	};
	struct alignas(16) ObjectBufferData
	{
		Matrix4x4<float> modelToWorldMatrix;
	};
}