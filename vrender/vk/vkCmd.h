#pragma once
#include "../cmd.h"

namespace valkyr {
	namespace render {
		class vkCmdList :CmdList {
			void Clear(unsigned rtvHandle, float* color);
		};
	}
}