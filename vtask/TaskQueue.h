#pragma once
//#include <memory>
#include <optional>
#include <mutex>
#include <array>
#include "../vcore/vptr.h"

namespace valkyr {
	//expectation: single producer, multi consumers

	class Task {
	public:
		virtual void exec() {}

		Task() {}
	};


	template <size_t N>
	struct TaskQueue {
		std::array<vptr<Task>,N> buff;
		unsigned int head;
		unsigned int tail;
		unsigned int len;
		std::mutex mtx;

		TaskQueue() :buff(), len(buff.size()), head(0), tail(0){
			buff.fill(nullptr);
		}

		unsigned int size() {
			return (tail + len - head) % len;
		}

		bool isFull(){ 
			return head == ((tail + 1) % len);
		}

		bool isEmpty() {
			std::lock_guard lock(mtx);
			return head == tail;
		}

		std::optional<vptr<Task> > get(){
			if (!isEmpty()) {
				std::optional<vptr<Task> > o(buff.at(head));
				head = (head + 1) % len;
				if (o.value() == nullptr) {
					return std::nullopt;
				}
				return o;
			}
			else
				return std::nullopt;
		}

		bool put(vptr<Task> t) {
			if (!isFull()) {
				buff.at(tail) = t;
				tail = (tail + 1) % len;
				return true;
			}
			else return false;
		}

		/*void put(T&& t) {
			put(std::forward(t));
		}*/
	};
}
