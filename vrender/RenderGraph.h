#pragma once
#include "../vcontainer/FrameGraph.h"
#include "cmd.h"

namespace valkyr {
	namespace render {
		using RPass = Pass<unsigned,CmdList>;

		const unsigned RES_RTV = 0, RES_FrameRT = 1, RES_DSV = 2, RES_SRV = 3, RES_UAV = 4, 
			RES_CBV = 5, RES_Sampler = 6, RES_VB = 7, RES_IB = 8;

		struct Tex
		{
			unsigned width;
			unsigned height;
			unsigned format;
		};

		using RRes = Res<Tex>;

		using RenderGraph = FrameGraph<RRes,CmdList>;
	}
}