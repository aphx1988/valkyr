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
	public:
		Vec<Res> resList;
		Vec<Pass> passList;
		Map<std::string_view, unsigned> passMap;

		Fg(){}
		Fg(Fg& fg) {

		}
		Fg(Fg&& fg){
			resList = fg.resList;
			passList = fg.passList;
			passMap = fg.passMap;
		}

		Fg& operator =(Fg&& fg) {
			if (this != &fg) {
				resList = fg.resList;
				passList = fg.passList;
				passMap = fg.passMap;
			}
			return *this;
		}
	};
}
