#pragma once
#include <thread>
#include <functional>
#include <any>
#include "../vcontainer/vec.h"

namespace valkyr {

	struct Task {
		std::function<void(Vec<std::any>)> fun;
		Vec<std::any> params;

		Task(std::function<void(Vec<std::any>)> f) {
			fun = f;
		}
	};

	struct TaskGroup {
		std::atomic<unsigned int> completedCount;
		Vec<Task> tasks;
	};

	struct Worker {
		bool free;
		bool neverSleep;

		void doWork(Task task) {
			free = false;
			task.fun(task.params);
			free = true;
			if(!neverSleep)
				std::this_thread::yield();
		}
	};

	//小心子线程直接执行或detach后，主线程退出时子线程还在执行，导致内存泄漏或者异常
	class TaskSys {
	public:
		void Init() {
			m_maxWorkers = std::thread::hardware_concurrency();
			for (int i = 1; i < m_maxWorkers; i++) {
				m_workers.push_back(Worker());
			}
		}

		template <typename Task_t>
		void Exec(TaskGroup& group) {
			for (auto it = m_freeWorkerNos.begin(); it != m_freeWorkerNos.end(); it++) {
				m_workers[it].detach();
			}
		}

		int m_maxWorkers;

	private:
		void doWork() {

		}

		Vec<Worker> m_workers;
		Vec<bool> m_workerIsFree;
		Vec<int> m_freeWorkerNos;


	};
}
