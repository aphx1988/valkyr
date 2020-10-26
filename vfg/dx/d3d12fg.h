#pragma once

#include "../fg.h"
#include "d3dcore.h"

namespace valkyr {

	using Microsoft::WRL::ComPtr;

	struct d3d12ResImpl {
		//ComPtr<ID3D12DescriptorHeap> descriptorHeap;
		ComPtr<ID3D12Resource> buff;

	};

	struct PassImpl {
		ComPtr<ID3D12PipelineState> pipelineState;
	};

	class d3d12Fg : public Fg {
		Map<unsigned, d3d12ResImpl> resImplMap;
	};
}
