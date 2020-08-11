#pragma once

#include "ContainerChunkNode.h"
#include "../vmalloc/ChunkMgr.h"
#include "algorithm.h"

namespace valkyr {
	template <typename T>
	struct Pool {
		unsigned int capacity;
		unsigned int chunkCount;
		ContainerChunkNode<T>* firstChunkNode;
		ContainerChunkNode<T>* lastChunkNode;
		Chunk* createdChunk;
		ChunkMgr* chunkMgr;
		int autoEntityId=0;
		int pad = 0;
	};

	class PoolUtil {
	public:
		template <typename T>
		static bool CanCreate(Chunk* chunk) {
			auto* info = ChunkUtil::GetInfo(chunk);
			bool r =  CHUNK_SIZE - info->usedSize >= sizeof(T) * NUM_PRECREATED
				+ sizeof(ContainerChunkNode<T>) + sizeof(Pool<T>);
			if (!r) {
				r = CHUNK_SIZE - info->usedSize >= sizeof(T)
					+ sizeof(ContainerChunkNode<T>) + sizeof(Pool<T>);
			}
			return r;
		}

		template <typename T>
		static inline Pool<T>* Create(ChunkMgr* chunkMgr) {
			//int numNeededChunks = num * size(T);
			Pool<T>* list = nullptr;
			Chunk* chunk = nullptr;
			if (CanCreate<T>(chunkMgr->m_currChunk)) {
				chunk = chunkMgr->m_currChunk;
			}
			//it's rare that the new created vec using more than one chunks,so here need just one chunk
			else {
				chunk = chunkMgr->UseNextChunk(0);
			}
			list = ChunkUtil::NewObjFrom<Pool<T>>(chunk);
			list->autoEntityId = 0;
			ContainerChunkNode<T>* chunkNode = ChunkUtil::NewObjFrom<ContainerChunkNode<T>>(chunk);
			chunkNode->firstSpan = SpanUtil::Create<T>(chunk, NUM_PRECREATED,list->autoEntityId);
			int size = NUM_PRECREATED;
			if (!chunkNode->firstSpan) {
				chunkNode->firstSpan = SpanUtil::Create<T>(chunk, 1,list->autoEntityId);
				size = 1;
			}
			chunkNode->spanCount = 1;
			chunkNode->chunk = chunk;
			chunkNode->chunkInfo = ChunkUtil::GetInfo(chunk);
			list->capacity = size;
			list->autoEntityId = list->capacity;
			list->chunkCount = 1;
			list->createdChunk = chunk;
			list->firstChunkNode = chunkNode;
			list->lastChunkNode = chunkNode;
			list->chunkMgr = chunkMgr;
			return list;
		}

		/*template <typename T>
		static void Push(T* t,Pool<T> pool) {
			ContainerChunkNode<T>* node = pool->lastChunkNode;

		}*/

		template <typename T>
		static inline void Push(T* t, SpanEntity* entity) {
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
				azero = ContainerChunkNodeUtil::PickZeroWithEntity<T>(node,pool->autoEntityId,[&pool](Span<T>* span) {
					pool->capacity += span->count;
					pool->autoEntityId = pool->capacity;
					});
				node = node->prev;
			}
			if (azero.first != nullptr) {
				return azero;
			}
			node = pool->lastChunkNode;
			Chunk* chunk = pool->chunkMgr->ImmediateUseNextChunk(0);
			ContainerChunkNode<T>* chunkNode = ChunkUtil::NewObjFrom<ContainerChunkNode<T>>(chunk);
			chunkNode->firstSpan = SpanUtil::Create<T>(chunk, NUM_PRECREATED,pool->autoEntityId);
			int capacity = NUM_PRECREATED;
			if (!chunkNode->firstSpan) {
				chunkNode->firstSpan = SpanUtil::Create<T>(chunk, 1, pool->autoEntityId);
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
			pool->autoEntityId = pool->capacity;
			azero = SpanUtil::GetWithEntity(0, chunkNode->firstSpan);
			azero.second->isZero = false;
			return azero;
		}

	private:
	};
}