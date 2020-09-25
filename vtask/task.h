#pragma once
#include <thread>
#include <functional>
#include <any>
#include "RingQueue.h"
#include "../vcontainer/vec.h"

namespace valkyr {

	struct Task {
		std::function<void(std::any[])> fun;
		std::any params[8];

		Task(std::function<void(std::any[])> f) {
			fun = f;
		}
	};

	struct TaskGroup {
		Vec<Task> tasks;
	};

	using TaskSeq = Vec<TaskGroup>;

	struct WorkerCtx {
		RingQueue<Task&>& taskQueue;
		std::atomic_size_t currGroupCompletedTask;
		std::atomic_size_t currGroupSize;
		bool running;

		void workLoop(int no) {
			while (running) {
				auto taskItem = taskQueue.get();
				if (taskItem) {
					auto task = taskItem.value();
					task.fun(task.params);
				}
			}
			std::this_thread::yield();
		}

		WorkerCtx(RingQueue<Task&>& queue):taskQueue(queue),running(false)
			,currGroupCompletedTask(0), currGroupSize(0)
		{
		}
	};

	//С�����߳�ֱ��ִ�л�detach�����߳��˳�ʱ���̻߳���ִ�У������ڴ�й©�����쳣
	class TaskSys {
	public:
		void Init() {
			
		}

		template <typename Task_t>
		void Exec(TaskGroup& group) {
			
		}

		int m_maxWorkers;

	private:
		void doWork() {

		}



	};
}
