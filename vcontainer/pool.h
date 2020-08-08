#pragma once

#include "ContainerChunkNode.h"
#include "../vmalloc/ChunkMgr.h"
#include "algorithm.h"

namespace valkyr {
	template <typename T>
	struct Pool {
		unsigned int size;
		unsigned int capacity;
		unsigned int chunkCount;
		ContainerChunkNode<T>* firstChunkNode;
		ContainerChunkNode<T>* lastChunkNode;
		Chunk* createdChunk;
		ChunkMgr* chunkMgr;
		int pad = 0;
	};

	class PoolUtil {
	public:
		template <typename  T>
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
			if (CanCreate(chunkMgr->m_currChunk)) {
				chunk = chunkMgr->m_currChunk;
			}
			//it's rare that the new created vec using more than one chunks,so here need just one chunk
			else {
				chunk = chunkMgr->UseNextChunk(0);
			}
			list = ChunkUtil::NewObjFrom<Pool<T>>(chunk);
			ContainerChunkNode<T>* chunkNode = ChunkUtil::NewObjFrom<ContainerChunkNode<T>>(chunk);
			chunkNode->firstSpan = SpanUtil::Create(chunk, NUM_PRECREATED);
			if (!chunkNode->firstSpan)
				chunkNode->firstSpan = SpanUtil::Create(chunk, 1);
			chunkNode->spanCount = 1;
			chunkNode->chunk = chunk;
			chunkNode->chunkInfo = ChunkUtil::GetInfo(chunk);
			list->size = 0;
			list->capacity = NUM_PRECREATED;
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
		static void Push(T* t, SpanEntity* entity, Pool<T> pool) {
			ContainerChunkNode<T>* node = pool->lastChunkNode;
			std::tuple<T*, SpanEntity*> item;
			//find the entity,need span entity has span pointer
			while (node != nullptr) {

			}
			
		}

		/*template <typename T>
		static T* Pop(Pool<T>* vec) {
			T* azero = nullptr;
			ContainerChunkNode<T>* node = vec->lastChunkNode;
			while (node != nullptr && azero == nullptr) {
				azero = ContainerChunkNodeUtil::PickZero(node, [&vec]() {
					vec->capacity += NUM_PRECREATED;
					vec->size++;
				});
				node = node->prev;
			}
			if (azero != nullptr) {
				return azero;
			}
			node = vec->lastChunkNode;
			Chunk* chunk = vec->chunkMgr->ImmediateUseNextChunk(0);
			ContainerChunkNode<T>* chunkNode = ChunkUtil::NewObjFrom<ContainerChunkNode<T>>(chunk);
			chunkNode->firstSpan = SpanUtil::Create(chunk, NUM_PRECREATED);
			int capacity = NUM_PRECREATED;
			if (!chunkNode->firstSpan) {
				chunkNode->firstSpan = SpanUtil::Create(chunk, 1);
				capacity = 1;
			}
			chunkNode->spanCount = 1;
			chunkNode->chunk = chunk;
			chunkNode->chunkInfo = ChunkUtil::GetInfo(chunk);
			chunkNode->prev = node;
			node->next = chunkNode;
			vec->chunkCount++;
			vec->lastChunkNode = chunkNode;
			vec->size++;
			vec->capacity += capacity;
			azero = SpanUtil::Get(0, chunkNode->firstSpan);
			return azero;
		}*/

		template <typename T>
		static std::pair<T*,SpanEntity*> Pop(Pool<T>* pool) {
			std::pair<T*,SpanEntity*> azero;
			ContainerChunkNode<T>* node = pool->lastChunkNode;
			while (node != nullptr && azero.first == nullptr) {
				azero = ContainerChunkNodeUtil::PickZeroWithEntity(node, [&pool]() {
					pool->capacity += NUM_PRECREATED;
					pool->size++;
					});
				node = node->prev;
			}
			if (azero.first != nullptr) {
				return azero;
			}
			node = pool->lastChunkNode;
			Chunk* chunk = pool->chunkMgr->ImmediateUseNextChunk(0);
			ContainerChunkNode<T>* chunkNode = ChunkUtil::NewObjFrom<ContainerChunkNode<T>>(chunk);
			chunkNode->firstSpan = SpanUtil::Create(chunk, NUM_PRECREATED);
			int capacity = NUM_PRECREATED;
			if (!chunkNode->firstSpan) {
				chunkNode->firstSpan = SpanUtil::Create(chunk, 1);
				capacity = 1;
			}
			chunkNode->spanCount = 1;
			chunkNode->chunk = chunk;
			chunkNode->chunkInfo = ChunkUtil::GetInfo(chunk);
			chunkNode->prev = node;
			node->next = chunkNode;
			pool->chunkCount++;
			pool->lastChunkNode = chunkNode;
			pool->size++;
			pool->capacity += capacity;
			azero = SpanUtil::GetWithEntity(0, chunkNode->firstSpan);
			azero.second->isZero = true;
			return azero;
		}

	private:
	};
}