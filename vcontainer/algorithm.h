#pragma once

#include "span.h"
#include "../vmalloc/ChunkMgr.h"

namespace valkyr {


	void vfor_each(ChunkMgr* container, std::function<void(Chunk*, int)> f) {
		Chunk* curr = container->m_firstChunk;
		int i = 0;
		while (curr != nullptr) {
			f(curr,i);
			Chunk* next = ChunkUtil::GetNext(curr);
			curr = next;
			i++;
		}
	}

	template <class Node>
	void vdo_while(Node* first, std::function<void(Node*)> f, std::function<bool(Node*)> cond) {
		Node* node = first;
		while (node != nullptr && cond(node)) {
			f(node);
			node = node->next;
		}
	}

	template <class Node>
	void vdo_while(Node* first, std::function<void(Node*)> f) {
		Node* node = first;
		while (node != nullptr) {
			f(node);
			node = node->next;
		}
	}

	template  <class Node,class T>
	T* vpick(Node* first, std::function<T* (Node*)> f) {
		Node* node = first;
		T* val = nullptr;
		while (node != nullptr && val == nullptr) {
			val = f(node);
			node = node->next;
		}
		if (f) f(node);
		return val;
	}

}