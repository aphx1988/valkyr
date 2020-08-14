#pragma once
#include "../vmalloc/chunk.h"

namespace valkyr {
	template <typename T>
	struct QueueNode {
		T* next;
		T* prev;
	};

	template <typename T>
	struct Queue {
		QueueNode<T>* front;
		QueueNode<T>* rear;
		uint32_t length;
		Chunk* chunk;
	};

	class QueueUtil {
	public:
		template <typename T>
		static Queue<T> Create(Chunk* chunk) {
			Queue<T> queue =  ChunkUtil::NewObjFrom<Queue<T>>(chunk);
			queue->front = nullptr;
			queue->rear = nullptr;
			queue->length = 0;
			queue->chunk = chunk;
			return queue;
		}

		template <typename T>
		static inline QueueNode<T> NewNode() {
			QueueNode<T> node = ChunkUtil::NewObjFrom<QueueNode<T>>(chunk);
			node->next = nullptr;
			node->prev = nullptr;
		}

		template <typename T>
		static inline void PushTo(QueueNode<T>* node,Queue<T>* queue) {
			if (queue->rear) {
				node->prev = queue->rear;
				queue->rear->next = node;
			}
			queue->rear = node;
		}

		template <typename T>
		static inline T* PopFrom(Queue<T>* queue) {
			T* r = queue->front;
			if (r->next != nullptr) {
				r->next->prev = nullptr;

			}
			queue->front->next->prev = nullptr;

			return r;
		}
	};
}
