#pragma once
#include <thread>
#include <functional>
#include "../vcontainer/vec.h"

namespace valkyr {

	using Worker = std::thread;

	template <typename ...Args>
	struct Task {
		std::function<void(Args...)> func;

		Task(std::function<void(Args...)> f) {
			func = f;
		}


	};

	template <typename Task_t>
	struct TaskGroup {
		std::atomic<unsigned int> completedCount;
		std::vector<Task_t> tasks;
	};

	class TaskSys {
	public:
		void Init() {
			m_maxWorkers = std::thread::hardware_concurrency();
			for (int i = 1; i < m_maxWorkers; i++) {
				m_workers.push_back(Worker([]() {

				}));
			}
		}

		template <typename Task_t>
		void Exec(TaskGroup<Task_t>& group) {
			for (auto it = m_freeWorkerNos.begin(); it != m_freeWorkerNos.end(); it++) {
				m_workers[it].detach();
			}
		}

		int m_maxWorkers;

	private:
		Vec<Worker> m_workers;
		Vec<int> m_freeWorkerNos;


	};
}
