#pragma once
#include <functional>
#include <thread>
#include <queue>
#include "TaskQueue.h"
#include "../vcontainer/vec.h"


namespace valkyr {

	using TaskGroup = Vec<vptr<Task>>;

	using TaskSeq = std::queue<TaskGroup>;

	template <size_t N>
	struct WorkerCtx {
		TaskQueue<N>& taskQueue;
		std::atomic_size_t currGroupCompletedTasks;
		unsigned threadWaitingTime;
		//unsigned sleepingTime;
		bool running;
		

		void workLoop() {
			while (running) {
				auto taskItem = taskQueue.get();
				if (taskItem) {
					auto task = taskItem.value();
					task->exec();
					currGroupCompletedTasks++;
				}
				if(threadWaitingTime>0u)
					std::this_thread::sleep_for(std::chrono::milliseconds(threadWaitingTime));
			}
			std::this_thread::yield();
		}

		WorkerCtx(TaskQueue<N>& queue,unsigned waitingTime):taskQueue(queue),running(true)
			,currGroupCompletedTasks(0),threadWaitingTime(waitingTime)
		{
		}
	};

	template <size_t N>
	class Scheduler {
	public:
		Scheduler():m_taskQueue(),m_threadWaitingTime(0u),
			m_taskSeq(),m_currTaskGroup(),m_unusedTasks()
		{
			m_workerCtx = vmake_ptr<WorkerCtx<N>>(m_taskQueue,m_threadWaitingTime);
		}

		void InitWorkers() {
			for (auto i = 0u; i < m_taskQueue.len; i++) {
				std::thread th(&WorkerCtx<N>::workLoop,m_workerCtx);
				th.detach();
			}
		}

		void Add(TaskSeq seq) {
			while (!seq.empty()) {
				m_taskSeq.push(seq.front());
				seq.pop();
			}
			ExecSeq();
		}

		void ExecSeq() {
			m_workerCtx->currGroupCompletedTasks = 0;
			//m_taskSeq = seq;
			m_currTaskGroup = m_taskSeq.front();
			m_taskSeq.pop();
			for (auto it = m_currTaskGroup.begin(); it < m_currTaskGroup.end(); it++) {
				Add(*it);
			}
		}

		void Add(vptr<Task> task) {
			if (!m_taskQueue.put(task)) {
				m_unusedTasks.emplace_back(task);
			}
			//m_taskQueue.put(task);
		}

		void tick() {
			for (auto it = m_unusedTasks.begin(); it < m_unusedTasks.end(); it++) {
				if (m_taskQueue.put(*it)) {
					m_unusedTasks.erase(it);
				}
			}
			if (m_workerCtx->currGroupCompletedTasks >= m_currTaskGroup.size()&&!m_taskSeq.empty()) {
				ExecSeq();
			}
		}

		unsigned m_threadWaitingTime;
		TaskQueue<N> m_taskQueue;
		vptr<WorkerCtx<N>> m_workerCtx;
		TaskSeq m_taskSeq;
		
	private:
		Vec<vptr<Task>> m_unusedTasks;
		TaskGroup m_currTaskGroup;
	};
}
