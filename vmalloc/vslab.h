#pragma once

#include <atomic>
#include <thread>
//#include <memory>
/*
* can use allocate_shared/unique with a simple bump alloc drived from std::allocator
*/

namespace valkyr {
	template<class T>
	struct Node {
		T* data;
		Node<T>* next;
		Node<T>* prev;
	};

	template<class T>
	struct LinkedList {
		Node<T>* head;
		Node<T>* tail;
		size_t num;
	};

	template<class T>
	struct Stack {
		LinkedList<T>* link;
	};

	template<class T>
	struct Slab {
		LinkedList<T>* obj_list;
		LinkedList<T>* free_list
	};
}
