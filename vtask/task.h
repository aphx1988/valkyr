#pragma once
#include <thread>
#include <functional>
#include <any>
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
					task.exec(task.params);
				}
			}
			std::this_thread::yield();
		}

		WorkerCtx(RingQueue<Task>& queue):taskQueue(queue),running(false)
			,currGroupCompletedTasks(0)
		{
		}
	};

	//小心子线程直接执行或detach后，主线程退出时子线程还在执行，导致内存泄漏或者异常
	class Scheduler {
	public:
		Scheduler(unsigned maxWorkers):m_maxWorkers(maxWorkers),m_taskQueue(maxWorkers),
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
				//要去重
				m_unusedTasks.push_back(task);
			}
		}

		void update() {
			for (auto it = m_unusedTasks.begin(); it < m_unusedTasks.end(); it++) {
				Add(*it);
			}
		}

		unsigned m_maxWorkers;
		RingQueue<Task> m_taskQueue;
		
	private:
		Vec<Task> m_unusedTasks;
		TaskSeq m_taskSeq;
		TaskGroup m_currTaskGroup;
	};
}
