#pragma once
#include <memory>

namespace valkyr {
	//expectation: single producer, multi consumers
	template <typename T>
	struct RingQueue {
		T* buff;
		unsigned int head;
		unsigned int tail;
		unsigned int len;

		RingQueue(unsigned int length):head(0),tail(0),len(length) {
			buff = new T[len];
			memset(buff, 0, len);
		}

		unsigned int size() {
			return (tail + len - head) % len;
		}

		bool isFull() {
			return head == ((tail + 1) % len);
		}

		bool isEmpty() {
			return head == tail;
		}

		T get() {
			if (!isEmpty()) {
				T t = buff[head];
				head = (head + 1) % len;
				return t;
			}
			else
				return 0;
		}

		void put(T t) {
			if (!isFull()) {
				buff[tail] = t;
				tail = (tail + 1) % len;
			}
		}

		/*void put(T&& t) {
			put(std::forward(t));
		}*/
	};
}
