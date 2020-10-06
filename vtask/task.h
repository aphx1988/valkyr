#pragma once
#include <thread>
#include <functional>
#include <any>
#include <queue>
#include "RingQueue.h"
#include "../vcontainer/vec.h"
#include "../vcontainer/stack.h"
#include "../vcore/vptr.h"

namespace valkyr {

	struct Task {
		std::function<void(Vec<std::any>)> exec;
		Vec<std::any> params;

		Task():exec(nullptr){}

		Task(std::function<void(Vec<std::any>)> f) {
			exec = f;
		}
	};

	using TaskGroup = Vec<Task>;

	using TaskSeq = Stack<TaskGroup>;

	struct WorkerCtx {
		RingQueue<Task>& taskQueue;
		std::atomic_size_t currGroupCompletedTasks;
		bool running;

		void workLoop(int no) {
			while (running) {
				auto taskItem = taskQueue.get();
				if (taskItem) {
					auto task = taskItem.value();
					if (task.exec) {
						task.exec(task.params);
					}
				}
			}
			std::this_thread::yield();
		}

		WorkerCtx(RingQueue<Task>& queue):taskQueue(queue),running(false)
			,currGroupCompletedTasks(0)
		{
		}
	};

	class Scheduler {
	public:
		Scheduler(unsigned maxWorkers):m_maxWorkers(maxWorkers),m_taskQueue(maxWorkers),m_threadWaitingTime(0),
			m_taskSeq(),m_currTaskGroup(), m_unusedTasks()
		{}

		void Exec(TaskSeq seq) {
			m_taskSeq = seq;
			m_currTaskGroup = seq.top();
			for (auto it = m_currTaskGroup.begin(); it < m_currTaskGroup.end(); it++) {
				Add(*it);
			}
		}

		void Add(Task task) {
			if (!m_taskQueue.put(task)) {
				m_unusedTasks.push(task);
			}
		}

		void update() {
			int visitedUnused = 0;
			for (size_t i = 0; i < m_unusedTasks.size(); i++) {
				Task task = m_unusedTasks.front();
				if (m_taskQueue.put(task)) {
					m_unusedTasks.pop();
				}
			}
		}

		unsigned m_maxWorkers;
		unsigned m_threadWaitingTime;
		RingQueue<Task> m_taskQueue;
		
	private:
		std::queue<Task> m_unusedTasks;
		TaskSeq m_taskSeq;
		TaskGroup m_currTaskGroup;
	};
}
