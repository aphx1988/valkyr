#pragma once

#include "ContainerChunkNode.h"
#include "../vmalloc/ChunkMgr.h"
#include "algorithm.h"

namespace valkyr {
	const uint32_t NUM_PRECREATED = 8;
	struct Entt {
		unsigned int id;
		alignas(4) bool isZero;
		Entt* prevZero;
		Entt* nextZero;
	};

	struct PoolProxy {
		uint32_t capacity;
		uint32_t zeroCount;
		uint32_t numZeroUsed;
		uint32_t autoEnttId;
		Chunk* chunk;
		Entt* topZeroEntt;
		PoolProxy* prev;
		PoolProxy* next;
	};

	struct Pool {
		Chunk* createdChunk;
		ChunkMgr* chunkMgr;
		PoolProxy* firstProxy;
		PoolProxy* lastProxy;
	};

	class PoolUtil {
	public:
		template <typename T>
		static bool CanContain(Chunk* chunk) {
			return CHUNK_SIZE - ChunkUtil::GetInfo(chunk)->usedSize >= sizeof(T)+sizeof(Entt);
		}

		template <typename T>
		static bool CanCreate(Chunk* chunk) {
			return CHUNK_SIZE - ChunkUtil::GetInfo(chunk)->usedSize >= sizeof(Pool)+sizeof(PoolProxy)+sizeof(T) + sizeof(Entt);
		}

		template <typename T>
		static bool CanCreateProxy(Chunk* chunk) {
			return CHUNK_SIZE - ChunkUtil::GetInfo(chunk)->usedSize >= sizeof(PoolProxy) + sizeof(T) + sizeof(Entt);
		}

		template <typename T>
		static Pool* Create(ChunkMgr* chunkMgr) {
			//int numNeededChunks = num * size(T);
			Pool* pool = nullptr;
			PoolProxy* firstProxy = nullptr;
			Chunk* chunk = nullptr;
			chunk = chunkMgr->m_currChunk;
			if (!CanCreate<T>(chunk)) {
				chunk = chunkMgr->UseNextChunk(0);
			}
			pool = ChunkUtil::NewObjFrom<Pool>(chunk);
			pool->createdChunk = chunk;
			pool->chunkMgr = chunkMgr;
			firstProxy = ChunkUtil::NewObjFrom<PoolProxy>(chunk);
			firstProxy->autoEnttId = 0;
			firstProxy->capacity = 0;
			firstProxy->zeroCount = 0;
			firstProxy->numZeroUsed = 0;
			firstProxy->chunk = chunk;
			firstProxy->prev = nullptr;
			firstProxy->next = nullptr;
			pool->firstProxy = firstProxy;
			pool->lastProxy = firstProxy;

			for (auto i = 0u; i < NUM_PRECREATED; i++) {
				if (!CanContain<T>(chunk)) break;
				T* t = ChunkUtil::NewObjFrom<T>(chunk);
				Entt* entt = ChunkUtil::NewObjFrom<Entt>(chunk);
				entt->id = 
				if (i == 0u) firstProxy->topZeroEntt = entt;
				firstProxy->capacity++;
			}
			firstProxy->autoEnttId = firstProxy->capacity;
			firstProxy->numZeroUsed = 0;
			firstProxy
			return pool;
		}

		/*template <typename T>
		static void Push(T* t,Pool<T> pool) {
			ContainerChunkNode<T>* node = pool->lastChunkNode;

		}*/

		template <typename T>
		static inline void Push(T* t, SpanEntity* entity,PoolProxy* proxy) {
			entity->isZero = true;

			memset(t, 0, sizeof(T));
		}

		template <typename T>
		static inline void Push(T* t) {
			SpanEntity* entity = SpanUtil::GetEntitiy(t);
			Push(t, entity);
		}

		template <typename T>
		static std::pair<T*,SpanEntity*> Pop(Pool<T>* pool) {
			std::pair<T*,SpanEntity*> azero;
			ContainerChunkNode<T>* node = pool->lastChunkNode;
			while (node != nullptr && azero.first == nullptr) {
				azero = ContainerChunkNodeUtil::PickZeroWithEntity<T>(node,pool->autoEnttId,[&pool](Span<T>* span) {
					pool->capacity += span->count;
					pool->autoEnttId = pool->capacity;
					});
				node = node->prev;
			}
			if (azero.first != nullptr) {
				return azero;
			}
			node = pool->lastChunkNode;
			Chunk* chunk = pool->chunkMgr->ImmediateUseNextChunk(0);
			ContainerChunkNode<T>* chunkNode = ChunkUtil::NewObjFrom<ContainerChunkNode<T>>(chunk);
			chunkNode->firstSpan = SpanUtil::Create<T>(chunk, NUM_PRECREATED,pool->autoEnttId);
			int capacity = NUM_PRECREATED;
			if (!chunkNode->firstSpan) {
				chunkNode->firstSpan = SpanUtil::Create<T>(chunk, 1, pool->autoEnttId);
				capacity = 1;
			}
			chunkNode->spanCount = 1;
			chunkNode->chunk = chunk;
			chunkNode->chunkInfo = ChunkUtil::GetInfo(chunk);
			chunkNode->prev = node;
			node->next = chunkNode?chunkNode:nullptr;
			pool->chunkCount++;
			pool->lastChunkNode = chunkNode;
			pool->capacity += capacity;
			pool->autoEnttId = pool->capacity;
			azero = SpanUtil::GetWithEntity(0, chunkNode->firstSpan);
			azero.second->isZero = false;
			return azero;
		}

	private:
	};
}