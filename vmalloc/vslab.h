#pragma once

#include <atomic>
#include <thread>
//#include <memory>
/*
* can use allocate_shared/unique with a simple bump alloc drived from std::allocator
*/

namespace valkyr {
	namespace slab {
		template<class T>
		struct Node {
			T* data;
			Node<T>* next;
			Node<T>* prev;
		};
		
		template<class T>
		struct Stack {
			Node<T>* head;
			size_t length;
			size_t capacity;
		public:
			T* pop() {

			}
			void push(T* element) {

			}
		};

		template<class T>
		struct Slab {
			Stack<T>* obj_list;
			size_t remaining_num;
		};

		template<class T>
		struct Cache {
			Stack<Slab<T>>* slabs;
			Stack<T>* free_list;
		};
	}
}
