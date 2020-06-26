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
			head = 0;
			usedSize = 0;
			unUsedSize = MAX_CHUNK_SIZE;
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
		Chunk* mCurrChunk;
		Chunk* mFirstChunk;
		size_t mChunkSize;

		Pool() {
			mChunkSize = 0;
			mCurrChunk = nullptr;
			mFirstChunk = nullptr;
			for (int i = 0; i < INIT_CHUNK_COUNT; i++) {
				NewChunk();
			}
			
		}

		void NewChunk() {
			Chunk* chunk = ChunkAllocator::Malloc();
			if (mCurrChunk != nullptr) {
				mCurrChunk->next = chunk;
			}
			mCurrChunk = chunk;
			++mChunkSize;
			if (mFirstChunk == nullptr) {
				mFirstChunk = mCurrChunk;
			}
		}

		ChunkInfo* GetInfo(Chunk* chunk) {
			return (ChunkInfo*)chunk->buff;
		}

		template <typename T>
		T* Spawn() {
			
			T* obj = new (mCurrChunk->buff+mCurrChunk->head)T();
			mCurrChunk->info.usedSize += sizeof(T);

			mCurrChunk->head += sizeof(T);
		}

		~Pool() {
			Chunk* next = nullptr;
			mCurrChunk = mFirstChunk->next;
			ChunkAllocator::Free(mFirstChunk);
			for (int i = 1; i < mChunkSize; i++) {
				next = mCurrChunk->next;
				ChunkAllocator::Free(mCurrChunk);
				mCurrChunk = next;
			}
		}

	};

};