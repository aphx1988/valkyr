#pragma once
#include <thread>
#include "../vcontainer/vec.h"

namespace valkyr {

	using Worker = std::thread;

	template <class F>
	struct Task {
		F func;
	};

	class Scheduler {
	private:
		Vec<Worker> workers;
		Vec<unsigned int> freeWorkerIds;
	};
}
