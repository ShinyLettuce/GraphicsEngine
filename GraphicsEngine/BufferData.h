#pragma once

namespace BufferData
{
	struct alignas(16) FrameBufferData
	{
		float worldToClipMatrix[16];
	};
	struct alignas(16) ObjectBufferData
	{
		float modelToWorldMatrix[16];
	};
}