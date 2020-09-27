#pragma once
//#include <memory>
#include <optional>
#include <mutex>

namespace valkyr {
	//expectation: single producer, multi consumers
	template <typename T>
	struct RingQueue {
		T* buff;
		unsigned int head;
		unsigned int tail;
		unsigned int len;
		std::mutex mtx;

		RingQueue(unsigned int length):len(length),head(0),tail(0){ 
			buff = new T[len];
			memset(buff, 0, len);
		}

		unsigned int size() {
			return (tail + len - head) % len;
		}

		bool isFull(){ 
			//std::lock_guard lock(mtx);
			return head == ((tail + 1) % len);
		}

		bool isEmpty() {
			std::lock_guard lock(mtx);
			return head == tail;
		}

		std::optional<T> get(){
			if (!isEmpty()) {
				std::optional<T> o(buff[head]);
				head = (head + 1) % len;
				return o;
			}
			else
				return std::nullopt;
		}

		bool put(T t) {
			if (!isFull()) {
				buff[tail] = t;
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
