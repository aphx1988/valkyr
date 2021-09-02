#pragma once
#include "ResBuilder.h"

namespace valkyr {
	namespace render {
		

		class RenderGraph :public FrameGraph<RRes, CmdList> {
		public:
			RenderGraph(){}
			~RenderGraph(){}

		private:
			vptr<ResBuilder> res_builder;
			vptr<CmdList> cmd_list;
		};
	}
}