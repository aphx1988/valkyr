#pragma once
#include <vcruntime_string.h>
#include <xmemory>

namespace valkyr {
	const int MAX_CHUNK_SIZE = 16*1024*1024;
	const int INIT_CHUNK_COUNT = 4;

	struct ChunkInfo {
		alignas(4) bool isFull;
		int head; 
		size_t usedSize;
		size_t unUsedSize;

		ChunkInfo() {
			isFull = false;
			head = sizeof(ChunkInfo);
			usedSize = sizeof(ChunkInfo);
			unUsedSize = MAX_CHUNK_SIZE- sizeof(ChunkInfo);
		}
	};

	union Chunk {
		char buff[MAX_CHUNK_SIZE]; 
		Chunk* next; 
	};

	class ChunkAllocator {
	public:
		static inline Chunk* Malloc() {
			Chunk* chunk = (Chunk*)malloc(MAX_CHUNK_SIZE);
			memset(chunk->buff, 0, sizeof(chunk));
			ChunkInfo* info = new(chunk->buff) ChunkInfo();
			chunk->next = nullptr;
			return chunk;
		}

		static inline void Free(Chunk* chunk) {
			chunk->next = nullptr;
			free(chunk->buff);
		}

	};

	struct PoolDesc {
		Chunk* lastChunk;
		Chunk* firstChunk;
		size_t chunkCount;
		char pad[4];
	};

	class Pool {
	public:
		PoolDesc* mDesc;

		Pool() {
			Chunk* chunk = ChunkAllocator::Malloc();
			ChunkInfo* firstChunkInfo = GetInfo(chunk);
			mDesc = new(chunk->buff+ firstChunkInfo->head) PoolDesc();
			firstChunkInfo->head += sizeof(PoolDesc);
			mDesc->firstChunk = chunk;
			mDesc->lastChunk = chunk;
			mDesc->chunkCount = 1;
			for (int i = 1; i < INIT_CHUNK_COUNT; i++) {
				NewChunk();
			}
			mDesc->chunkCount = INIT_CHUNK_COUNT;

		}

		void NewChunk() {
			Chunk* chunk = ChunkAllocator::Malloc();
			if (mDesc->lastChunk != nullptr) {
				mDesc->lastChunk->next = chunk;
			}
			mDesc->lastChunk = chunk;
			++mDesc->chunkCount;
			/*
			if (mInfo->firstChunk == nullptr) {
				mInfo->firstChunk = mInfo->lastChunk;
			}
			*/
		}

		ChunkInfo* GetInfo(Chunk* chunk) {
			return (ChunkInfo*)chunk->buff;
		}

		ChunkInfo* GetLastChunkInfo() {
			return GetInfo(mDesc->lastChunk);
		}

		template <typename T>
		T* Spawn() {
			T* obj = new (mDesc->lastChunk->buff+mDesc->lastChunk->info->head)T();
			ChunkInfo* info = GetInfo(mLastChunk);
			info->usedSize += sizeof(T);
			if (info->usedSize >= MAX_CHUNK_SIZE) {
				info->usedSize = MAX_CHUNK_SIZE;
				info->unUsedSize = 0;
				info->head = MAX_CHUNK_SIZE;
				info->isFull = true;
			}
			else {
				info->head += sizeof(T);
				info->usedSize += sizeof(T);
				info->unUsedSize -= sizeof(T);
			}
			return obj;
		}

		void Remove(int num) {
			//first chunk has pool desc, will remove last
			if (num <1 ) {
				return;
			}
			if (num >= mDesc->chunkCount) {
				Clear();
				return;
			}
			for (int i = 0; i < num; ++i) {
				ChunkAllocator::Free(mDesc->lastChunk);
				mDesc->lastChunk = mDesc->lastChunk->next;
				mDesc->chunkCount--;
			}
		}

		void Clear() {
			Chunk* next = nullptr;
			Chunk* curr = mDesc->firstChunk->next;
			//ChunkAllocator::Free(mFirstChunk);
			for (int i = 1; i < mDesc->chunkCount; i++) {
				next = curr->next;
				ChunkAllocator::Free(curr);
				curr = next;
			}
			mDesc->lastChunk = nullptr;
			mDesc->chunkCount = 0;
			ChunkAllocator::Free(mDesc->firstChunk);
			mDesc->firstChunk = nullptr;
		}

		~Pool() {
			Clear();
		}

	};

};