#pragma once
#include "ResBuilder.h"
#include <string_view>

namespace valkyr {
	namespace render {
		
		struct RenderSetting {
			std::string_view title;
			unsigned width;
			unsigned height;
			unsigned num_fb;
			bool fullscreen;
			bool software;
		};

		class Renderer {
		public:
			Renderer(){}
			~Renderer(){}

			virtual void init(RenderSetting setting) = 0;
			virtual void perFrame() = 0;
			virtual void destroy() = 0;

		private:
			FrameGraph<RRes, CmdList> fg;
			vptr<ResBuilder> res_builder;
			vptr<CmdList> cmd_list;
		};
	}
}