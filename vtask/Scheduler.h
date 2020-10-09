#pragma once
#include <thread>
#include <functional>
#include <any>
#include <queue>
#include "TaskQueue.h"
#include "../vcontainer/vec.h"


namespace valkyr {

	using TaskGroup = Vec<vptr<Task>>;

	using TaskSeq = std::queue<vptr<TaskGroup>>;

	template <size_t N>
	struct WorkerCtx {
		TaskQueue<N>& taskQueue;
		std::atomic_size_t currGroupCompletedTasks;
		//unsigned sleepingTime;
		bool running;
		bool canUpdate;
		

		void workLoop(int no) {
			while (running) {
				//只有canUpdate且没有task在执行时才可执行
				if (!canUpdate) {
					continue;
				}
				auto taskItem = taskQueue.get();
				if (taskItem) {
					auto task = taskItem.value();
					task.exec();
				}
				//std::this_thread::sleep_for(std::chrono::milliseconds(sleepingTime));
			}
			std::this_thread::yield();
		}

		WorkerCtx(TaskQueue<N>& queue):taskQueue(queue),running(true), canUpdate(true)
			,currGroupCompletedTasks(0)
		{
		}
	};

	template <size_t N>
	class Scheduler {
	public:
		Scheduler():m_taskQueue(),m_maxWorkers(m_taskQueue.len),m_threadWaitingTime(300),
			m_taskSeq(),m_currTaskGroup(),m_unusedTasks()
		{
			m_workerCtx = vmake_ptr<WorkerCtx<N>>(m_taskQueue);
		}

		void Add(TaskSeq seq) {
			while (!seq.empty()) {
				m_taskSeq.push(seq.pop());
			}
		}

		void ExecSeq() {
			m_workerCtx->currGroupCompletedTasks = 0;
			//m_taskSeq = seq;
			m_currTaskGroup = m_taskSeq.pop();
			for (auto it = m_currTaskGroup.begin(); it < m_currTaskGroup.end(); it++) {
				Add(*it);
			}
		}

		void Add(vptr<Task> task) {
			if (!m_taskQueue.put(task)) {
				m_unusedTasks.emplace_back(task);
			}
		}

		void tick() {
			for (auto it = m_unusedTasks.begin(); it < m_unusedTasks.end(); i++) {
				Task task = m_unusedTasks[i];
				if (m_taskQueue.put(task)) {
					m_unusedTasks.erase(it);
				}
			}
			if (m_workerCtx->currGroupCompletedTasks >= m_currTaskGroup.size()&&!m_taskSeq.empty()) {
				ExecSeq();
			}
		}

		unsigned m_maxWorkers;
		unsigned m_threadWaitingTime;
		TaskQueue<N> m_taskQueue;
		vptr<WorkerCtx<N>> m_workerCtx;
		
	private:
		Vec<vptr<Task>> m_unusedTasks;
		TaskSeq m_taskSeq;
		TaskGroup m_currTaskGroup;
	};
}
