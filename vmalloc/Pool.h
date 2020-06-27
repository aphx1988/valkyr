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

	class Pool {
		Chunk* mLastChunk;
		Chunk* mFirstChunk;
		size_t mChunkSize;

		Pool() {
			mChunkSize = 0;
			mLastChunk = nullptr;
			mFirstChunk = nullptr;
			for (int i = 0; i < INIT_CHUNK_COUNT; i++) {
				NewChunk();
			}
			
		}

		void NewChunk() {
			Chunk* chunk = ChunkAllocator::Malloc();
			if (mLastChunk != nullptr) {
				mLastChunk->next = chunk;
			}
			mLastChunk = chunk;
			++mChunkSize;
			if (mFirstChunk == nullptr) {
				mFirstChunk = mLastChunk;
			}
		}

		ChunkInfo* GetInfo(Chunk* chunk) {
			return (ChunkInfo*)chunk->buff;
		}

		template <typename T>
		T* Spawn() {
			T* obj = new (mLastChunk->buff+mLastChunk->head)T();
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
			if (num >= mChunkSize) 
				Clear();
			Chunk* curr = mFirstChunk;
			for (int i = 0; i < num; i++) {
				ChunkAllocator::Free(curr);
				curr = curr->next;
			}
			mFirstChunk = curr;
		}

		void Clear() {
			Chunk* next = nullptr;
			Chunk* curr = mFirstChunk->next;
			ChunkAllocator::Free(mFirstChunk);
			for (int i = 1; i < mChunkSize; i++) {
				next = curr->next;
				ChunkAllocator::Free(curr);
				curr = next;
			}
			mLastChunk = nullptr;
			mFirstChunk = nullptr;
			mChunkSize = 0;
		}

		~Pool() {
			Clear();
		}

	};

};