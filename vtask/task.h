#pragma once
#include <thread>
#include <functional>
#include <any>
#include "RingQueue.h"
#include "../vcontainer/vec.h"
#include "../vcore/vptr.h"

namespace valkyr {

	struct Task {
		virtual void fun() = 0;
	};

	struct TaskGroup {
		Vec<Task> tasks;
	};

	using TaskSeq = Vec<TaskGroup>;

	struct WorkerCtx {
		RingQueue<Task>& taskQueue;
		std::atomic_size_t currGroupCompletedTask;
		std::atomic_size_t currGroupSize;
		bool running;

		void workLoop(int no) {
			while (running) {
				auto taskItem = taskQueue.get();
				if (taskItem) {
					auto task = taskItem.value();
					task.fun();
				}
			}
			std::this_thread::yield();
		}

		WorkerCtx(RingQueue<Task>& queue):taskQueue(queue),running(false)
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
