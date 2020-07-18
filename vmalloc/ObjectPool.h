#pragma once
#include "MemoryArena.h"

namespace valkyr {
	template <typename T>
	struct PoolNode {
		T* obj;
		alignas(4) bool dirty;
		PoolNode* next;
		int pad;
	};

	template <typename T>
	struct Pool {
		PoolNode<T>* firstNode;
		PoolNode<T>* lastNode;
		Arena* arena;
		size_t nodeCount;
	};

	class PoolUtil {
	public:
		template <typename T>
		static inline Pool<T>* CreatePool(Arena* arena) {
			Pool<T>* pool = ArenaUtil::NewObjFrom<Pool<T>>(arena);
			pool->arena = arena;
			pool->nodeCount = 0;
			pool->firstNode = nullptr;
			pool->lastNode = nullptr;
			return pool;
		}

		template <typename T>
		static inline T* Spawn(Pool<T>* pool) {
			return PoolUtil::Spawn(pool, nullptr);
		}

		template <typename T>
		static inline T* Spawn(Pool<T>* pool,std::function<void(T*)> initAction) {
			if (pool == nullptr || pool->arena == nullptr) return nullptr;
			PoolNode<T>* node = nullptr;
			T* obj = nullptr;
			node = PoolUtil::FindCleanNode<T>(pool);
			if (node == nullptr) {
				obj = ArenaUtil::NewObjFrom<T>(pool->arena);
				PoolUtil::AddNewNode(pool,obj);
			}
			else {
				node->dirty = true;
				obj = node->obj;
			}
			if (initAction)
				initAction(obj);
			return obj;
		}

		template<typename T>
		static inline void Recycle(T* obj,Pool<T>* pool,std::function<void(T*)> initAction) {
			if (pool == nullptr||pool->arena==nullptr) return;
			PoolNode<T>* node = PoolUtil::FindCleanNode<T>(pool);
			if (node == nullptr) {
				PoolUtil::AddNewNode(pool, obj);
			}
			else {
				node->dirty = true;
				node->obj = obj;
			}
			if (initAction)
				initAction(obj);
		}

		template <typename T>
		static inline void AddNewNode(Pool<T>* pool, T* obj) {
			PoolNode<T>* node = ArenaUtil::NewObjFrom<PoolNode<T>>(pool->arena);
			node->obj = obj;
			node->next = nullptr;
			if (pool->lastNode != nullptr) {
				pool->lastNode->next = node;
			}
			else if (pool->firstNode != nullptr) {
				pool->firstNode->next = node;
			}
			else
			{
				pool->firstNode = node;
			}
			pool->lastNode = node;
			pool->nodeCount++;
		}

		template <typename T>
		static inline void Traverse(Pool<T>* pool, std::function<void(PoolNode<T>*, int)> action) {
			if (pool == nullptr || pool->arena == nullptr) return;
			PoolNode<T>* curr = pool->firstNode;
			for (int i = 0; i < pool->nodeCount; i++) {
				if (curr == nullptr) return;
				PoolNode<T>* next = curr->next;
				if (action)
					action(curr, i);
				if (next == nullptr) return;
				curr = next;
			}
		}

		template <typename T>
		static inline PoolNode<T>* FindCleanNode(Pool<T>* pool) {
			if (pool == nullptr ||pool->nodeCount<1|| pool->arena == nullptr) return nullptr;
			PoolNode<T>* curr = pool->firstNode;
			while (curr != nullptr) {
				if (!curr->dirty) {
					return curr;
				}
				else {
					curr = curr->next;
				}
			}
			return nullptr;
		}

	};
}