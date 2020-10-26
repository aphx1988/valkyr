#pragma once
#include "../vcontainer/graph.h"
#include "../vcontainer/vec.h"
#include "../vcontainer/map.h"
#include "../vtask/Scheduler.h"

namespace valkyr {
	struct Res {
		unsigned id;
		unsigned type;
		unsigned width;
		unsigned height;
	};

	struct Pass {
		std::function<void(Vec<unsigned>, Vec<unsigned>)> setupFunc;
		std::function<void(Vec<unsigned>,Vec<unsigned>)> renderFunc;
	};

	class Fg {
		Vec<Res> resList;
		Vec<Pass> passList;
	};
}
