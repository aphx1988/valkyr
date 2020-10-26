#pragma once
#include "../cmd.h"
#include "../../vcore/vptr.h"
#include "d3d12Renderer.h"

using Microsoft::WRL::ComPtr;

namespace valkyr {
	class d3d12CmdList :CmdList {
	public:
		d3d12CmdList(vptr<d3d12Renderer> renderer):m_renderer(renderer){
		}

		void ClearRTV(unsigned resId, float* color);
		void ClearDSV(unsigned resId, float* color);

		vptr<d3d12Renderer> m_renderer;

	};

}