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

	//thx: https://zhuanlan.zhihu.com/p/73640419
	template <size_t N>
	struct TaskQueue {
		std::array<vptr<Task>,N> buff;
		unsigned in;
		unsigned out;
		unsigned len;
		//unsigned int maxOut;
		std::mutex mtx;

		TaskQueue() :buff(), len(buff.size()), in(0), out(0){
			buff.fill(nullptr);
		}

		/*unsigned int size() {
			return (tail + len - head) % len;
		}*/

		inline bool isFull(){ 
			//return head == ((tail + 1) % len);
			return (in-out)==len;
		}

		inline bool isEmpty() {
			//std::lock_guard lock(mtx);
			return in == out;
		}

		inline int step(int i) {
			return ++i;
		}

		std::optional<vptr<Task>> get(){
			std::lock_guard<std::mutex> guard(mtx);
			if (!isEmpty()) {
				std::optional<vptr<Task>> o(buff.at(out&(len-1)));
				out = step(out);
				if (o.value() == nullptr) {
					return std::nullopt;
				}
				return o;
			}
			else {
				in = out = 0;
				return std::nullopt;
			}
		}

		bool put(vptr<Task> t) {
			if (isFull()) {
				return false;
			}
			buff.at(in & (len - 1)) = t;
			/*if (isFull()) {
				out = step(out);
			}*/
			in = step(in);
		}

		/*void put(T&& t) {
			put(std::forward(t));
		}*/
	};
}
