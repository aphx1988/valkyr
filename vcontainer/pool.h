#pragma once

#include <functional>
#include "../vmalloc/chunk.h"

namespace valkyr {
	const size_t NUM_PRECREATED = 8;
	template <typename T>
	struct Entt {
		size_t id;
		alignas(8) bool isZero;
		T* data;
		Entt* nextZero;
	};

	template <typename T>
	struct Pool {
		size_t autoEnttId;
		size_t capacity;
		size_t zeroCount;
		Chunk* chunk;
		Entt<T>* frontZero;
		Entt<T>* rearZero;
		Pool<T>* prev;
		Pool<T>* next;
	};

	template <typename T>
	struct PoolGroup {
		Pool<T>* firstPool;
		Pool<T>* lastPool;
		size_t poolCount;
		size_t pad;
	};

	class PoolUtil {
	public:
		template <typename T>
		static bool CanContain(Chunk* chunk) {
			return CHUNK_SIZE - ChunkUtil::GetInfo(chunk)->usedSize >= sizeof(T)+sizeof(Entt<T>);
		}

		template <typename T>
		static bool CanCreate(Chunk* chunk) {
			return CHUNK_SIZE - ChunkUtil::GetInfo(chunk)->usedSize >= sizeof(Pool<T>)+sizeof(T) + sizeof(Entt<T>);
		}

		template <typename T>
		static bool CanCreateGroup(Chunk* chunk) {
			return CHUNK_SIZE - ChunkUtil::GetInfo(chunk)->usedSize >=sizeof(PoolGroup<T>) + sizeof(Pool<T>) + sizeof(T) + sizeof(Entt<T>);
		}

		template <typename T>
		static size_t GetMaxCapacityFor(Chunk* chunk) {
			size_t unusedSize = CHUNK_SIZE - ChunkUtil::GetInfo(chunk)->usedSize;
			size_t eleSize = sizeof(T) + sizeof(Entt<T>);
			return (unusedSize / eleSize) - sizeof(Pool<T>);
		}

		template <typename T>
		static PoolGroup<T>* CreateGroup(Chunk* firstChunk,size_t initCount) {
			Chunk* chunk = firstChunk;
			if (!CanCreateGroup<T>(firstChunk)) {
				chunk = ChunkAllocator::Malloc();
				ChunkUtil::Connect(firstChunk, chunk);
			}
			PoolGroup<T>* group = ChunkUtil::NewObjFrom<PoolGroup<T>>(chunk);
			group->firstPool = Create<T>(chunk,0,initCount);
			Pool<T>* pool = group->firstPool;
			group->poolCount = 1;
			auto currCount = pool->capacity;
			while (currCount < initCount) {
				Pool<T>* lastPool = pool;
				pool = Create<T>(chunk,currCount,initCount);
				lastPool->next = pool;
				pool->prev = lastPool;
				currCount+=pool->capacity;
				group->poolCount++;
			}
			group->lastPool = pool;
			return group;
		}

		template <typename T>
		static Pool<T>* Create(Chunk* chunk, size_t baseEnttIdx, size_t initCount) {
			//int numNeededChunks = num * size(T);
			Pool<T>* pool = nullptr;
			pool = ChunkUtil::NewObjFrom<Pool<T>>(chunk);
			pool->autoEnttId = baseEnttIdx;
			pool->capacity = 0;
			pool->zeroCount = 0;
			pool->chunk = chunk;
			pool->prev = nullptr;
			pool->next = nullptr;
			pool->frontZero = nullptr;
			pool->rearZero = nullptr;
			for (auto i = 0; i < initCount; i++) {
				if (!CanContain<T>(chunk)) break;
				Entt<T>* it = NewFromAndAdd<T>(pool);
				if (i == 0) { 
					pool->frontZero = it; 
				}
			}
			return pool;
		}

		template <typename T>
		static Pool<T>* Create(Chunk* chunk, size_t initCount) {
			return Create<T>(chunk, 0, initCount);
		}

		template <typename T>
		static Pool<T>* CreateAndFill(Chunk* chunk,size_t baseEnttIdx) {
			Pool<T>* pool = nullptr;
			pool = ChunkUtil::NewObjFrom<Pool<T>>(chunk);
			pool->autoEnttId = baseEnttIdx;
			pool->capacity = 0;
			pool->zeroCount = 0;
			pool->chunk = chunk;
			pool->prev = nullptr;
			pool->next = nullptr;
			pool->frontZero = nullptr;
			pool->rearZero = nullptr;
			auto i = 0;
			while (CanContain<T>(chunk)) {
				Entt<T>* it = NewFromAndAdd<T>(pool);
				if (i == 0) pool->frontZero = it;
				i++;
			}
			return pool;
		}

		template <typename T>
		static Pool<T>* CreateAndFill(Chunk* chunk) {
			return CreateAndFill<T>(chunk, 0);
		}

		template <typename T>
		static Entt<T>* GetEntt(T* t) {
			return (Entt<T>*)(t + sizeof(T));
		}

		//实质上是回收已有的obj
		template <typename T>
		static inline void Push(Entt<T>* entity,Pool<T>* pool) {
			entity->isZero = true;
			pool->zeroCount++;
			Entt<T>* oldRear = pool->rearZero;
			if (oldRear)
				oldRear->nextZero = entity;
			pool->rearZero = entity;
			memset(entity->data, 0, sizeof(T));
		}

		template <typename T>
		static Entt<T>* NewFrom(Pool<T>* pool) {
			if (!CanContain<T>(pool->chunk)) return nullptr;
			T* t = ChunkUtil::NewObjFrom<T>(pool->chunk);
			Entt<T>* entt = ChunkUtil::NewObjFrom<Entt<T>>(pool->chunk);
			entt->id = pool->autoEnttId;
			entt->data = t;
			pool->autoEnttId++;
			pool->capacity++;
			return entt;
		}

		template <typename T>
		static Entt<T>* NewFromAndAdd(Pool<T>* pool) {
			Entt<T>* r = NewFrom<T>(pool);
			if (r) {
				Push(r,pool);
			}
			return r;
		}

		template <typename T>
		static Entt<T>* Pop(Pool<T>* pool) {
			Entt<T>* entt = pool->frontZero;
			if (entt != nullptr) {
				entt->isZero = false;
				pool->frontZero = entt->nextZero;
				pool->zeroCount--;
				entt->nextZero = nullptr;
			}
			return entt;
		}

		//push直接在pop返回的pool里操作
		template <typename T>
		static std::pair<Entt<T>*,Pool<T>*> PopOrNewFrom(PoolGroup<T>* group) {
			Pool<T>* pool = group->firstPool;
			Entt<T>* entt = nullptr;
			/*while (!entt&&pool) {
				entt = PopOrNew(pool);
				pool = pool->next;
			}*/
			for (int i = 0; i < group->poolCount; i++) {
				entt = PopOrNew(pool);
				if (entt) {
					break;
				}
				pool = pool->next;
			}
			if (!entt) {
				Chunk* lastChunk = group->lastPool->chunk;
				Chunk* chunk = ChunkAllocator::Malloc();
				ChunkUtil::Connect(lastChunk, chunk);
				pool = PoolUtil::Create<T>(chunk,100);
				group->lastPool->next = pool;
				pool->prev = group->lastPool;
				group->lastPool = pool;
				group->poolCount++;
				entt = PopOrNew<T>(pool);
			}
			return std::make_pair(entt,pool);
		}

		//这里在无可用时会创建新obj
		template <typename T>
		static Entt<T>* PopOrNew(Pool<T>* pool) {
			Entt<T>* r = Pop(pool);
			if(r == nullptr){
				r = NewFrom<T>(pool);
			}
			return r;
		}

		template <typename T>
		static std::pair<Entt<T>*, Pool<T>*> PopOrCreate(Pool<T>* pool) {
			Pool<T>* currPool = pool;
			Entt<T>* entt = PopOrNew<T>(pool);
			if (!entt) {
				Pool<T>* lastPool = pool;
				Chunk* lastChunk =pool->chunk;
				Chunk* chunk = ChunkAllocator::Malloc();
				ChunkUtil::Connect(lastChunk, chunk);
				currPool = PoolUtil::CreateAndFill<T>(chunk,lastPool->autoEnttId);
				lastPool->next = currPool;
				currPool->prev = lastPool;
				lastPool = currPool;
				entt = PopOrNew<T>(currPool);

			}
			return std::make_pair(entt, currPool);
		}

		template <typename T>
		static void TraverseZero(Pool<T>* pool,std::function<void(Entt<T>*)> f) {
			Entt<T>* curr = pool->frontZero;
			while (curr != nullptr) {
				f(curr);
				curr = curr->nextZero;
			}
		}

		template <typename T>
		static void TraverseZero(PoolGroup<T>* group, std::function<void(Entt<T>*)> f) {
			Pool<T>* pool = group->firstPool;
			while (pool) {
				TraverseZero<T>(pool, f);
				pool = pool->next;
			}
		}

		template <typename T>
		static inline void Free(Pool<T>* pool){
			if(pool->prev)
				pool->prev->next = nullptr;
			if(pool->next)
				pool->next->prev = nullptr;
			ChunkAllocator::Free(pool->chunk);
			pool = nullptr;
		}

		template <typename T>
		static void FreeAll(PoolGroup<T>* group) {
			Pool<T>* currPool = group->lastPool;
			if (group->poolCount == 1) {
				Free(currPool);
				return;
			}
			for (auto i = 0; i < group->poolCount; i++) {
				Pool<T>* prevPool = currPool->prev;
				Free(currPool);
				currPool = prevPool;
			}
		}

		template <typename T>
		static void FreeAll(Pool<T>* pool) {
			Pool<T>* curr = pool;
			while (curr) {
				Pool<T>* next = curr->next;
				Free(curr);
				curr = next;
			}
		}
	};
}
