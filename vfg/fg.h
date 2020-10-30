#pragma once
#include "../vcontainer/graph.h"
#include "../vcontainer/vec.h"
#include "../vcontainer/map.h"
#include "../vtask/Scheduler.h"
#include "cmd.h"

namespace valkyr {
	struct Res {
		unsigned id;
		unsigned format;
		unsigned type;
		unsigned typeListIdx;
		unsigned width;
		unsigned height;
		unsigned flag;
		unsigned userTag;
	};

	struct Pass {
		std::function<void(Vec<unsigned>, Vec<unsigned>)> setupFunc;
		std::function<void(CmdList cmdList,Vec<unsigned>,Vec<unsigned>)> renderFunc;
	};

	class Fg {
		Vec<Res> resList;
		Vec<Pass> passList;
		Map<std::string_view, unsigned> passMap;
	};
}
