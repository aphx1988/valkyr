#pragma once
#include "../vcontainer/vec.h"

namespace valkyr {
	namespace render {
		class CmdList {
			virtual void Clear(unsigned rtvHandle, float* color) = 0;
		};
	}
}
