#pragma once
#ifdef WIN32
#include <vcruntime_string.h>
#endif
#include <xmemory>

namespace valkyr {
	const unsigned int CHUNK_SIZE = 16 * 1024 * 1024;
	const unsigned int POOL_CHUNKS = 4;

	struct ChunkInfo {
		alignas(4) bool isFull;
		int head;
		size_t usedSize;
		size_t unUsedSize;

		ChunkInfo() {
			isFull = false;
			head = sizeof(ChunkInfo);
			usedSize = sizeof(ChunkInfo);
			unUsedSize = CHUNK_SIZE - sizeof(ChunkInfo);
		}
	};

	struct Chunk {
		char buff[CHUNK_SIZE];
		//Chunk* next;
	};

	class ChunkAllocator {
	public:
		static inline Chunk* Malloc() {
			Chunk* chunk = (Chunk*)malloc(CHUNK_SIZE);
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

	class ChunkUtil {
	public:
		static inline ChunkInfo* GetInfo(Chunk* chunk) {
			return (ChunkInfo*)chunk->buff;
		}

		static inline Chunk* GetNext(Chunk* chunk) {
			return (Chunk*)chunk->buff+sizeof(ChunkInfo);
		}

		template <typename T>
		static inline T* NewObjFrom(Chunk* chunk) {
			ChunkInfo* info = GetInfo(chunk);
			if (info->unUsedSize < sizeof(T)) {
				return nullptr;
			}
			T* obj = new (chunk->buff + info->head)T();
			info->usedSize += sizeof(T);
			if (info->usedSize >= CHUNK_SIZE) {
				info->usedSize = CHUNK_SIZE;
				info->unUsedSize = 0;
				info->head = CHUNK_SIZE;
				info->isFull = true;
			}
			else {
				info->head += sizeof(T);
				info->usedSize += sizeof(T);
				info->unUsedSize -= sizeof(T);
			}
			return obj;
		}

		template <typename T>
		static inline T* GetFrom(int idx, Chunk* chunk) {
			return (T*)(chunk->buff + idx);
		}
	};

	struct Pool
	{
		Chunk* lastChunk;
		Chunk* firstChunk;
		size_t chunkCount;
		int pad;
	};

	class PoolUtil {
	public:
		static inline Pool* CreatePool() {
			Chunk* chunks[POOL_CHUNKS];
			for (int i = 0; i < POOL_CHUNKS; ++i) {
				chunks[i] = ChunkAllocator::Malloc();
				if (i > 0) {
					chunks[i - 1]->next = chunks[i];
				}
			}
			Pool* pool = ChunkUtil::NewObjFrom<Pool>(chunks[0]);
			pool->chunkCount = POOL_CHUNKS;
			pool->firstChunk = chunks[0];
			pool->lastChunk = chunks[POOL_CHUNKS-1];
			return pool;
		}

		template <typename T>
		static inline T* Spawn() {
		}

		static inline void Clear(Pool* pool) {
			if (pool->chunkCount == 0) return;
			if (pool->firstChunk == nullptr) return;
			if (pool->chunkCount ==1) {
				ChunkAllocator::Free(pool->firstChunk);
				return;
			}
			Chunk* curr = pool->firstChunk->next;
			for (int i = 1; i < pool->chunkCount; ++i) {
				Chunk* next = curr->next;
				if (next == nullptr) return;
				ChunkAllocator::Free(curr);
				curr = next;
			}
		}
	};

	//class PoolUtil {
	//public:
	//	PoolDesc* mDesc=nullptr;

	//	PoolUtil() {
	//		Chunk* chunk = ChunkAllocator::Malloc();
	//		ChunkInfo* firstChunkInfo = GetInfo(chunk);
	//		mDesc = new(chunk->buff+ firstChunkInfo->head) PoolDesc();
	//		firstChunkInfo->head += sizeof(PoolDesc);
	//		mDesc->firstChunk = chunk;
	//		mDesc->lastChunk = chunk;
	//		mDesc->chunkCount = 1;
	//		for (int i = 1; i < INIT_CHUNK_COUNT; i++) {
	//			NewChunk();
	//		}
	//		mDesc->chunkCount = INIT_CHUNK_COUNT;

	//	}

	//	void NewChunk() {
	//		Chunk* chunk = ChunkAllocator::Malloc();
	//		if (mDesc->lastChunk != nullptr) {
	//			mDesc->lastChunk->next = chunk;
	//		}
	//		mDesc->lastChunk = chunk;
	//		++mDesc->chunkCount;
	//		/*
	//		if (mInfo->firstChunk == nullptr) {
	//			mInfo->firstChunk = mInfo->lastChunk;
	//		}
	//		*/
	//	}

	//	ChunkInfo* GetInfo(Chunk* chunk) {
	//		return (ChunkInfo*)chunk->buff;
	//	}

	//	ChunkInfo* GetLastChunkInfo() {
	//		return GetInfo(mDesc->lastChunk);
	//	}

	//	template <typename T>
	//	T* Spawn() {
	//		ChunkInfo* info = GetLastChunkInfo();
	//		if (info->unUsedSize < sizeof(T)) {
	//			return nullptr;
	//		}
	//		T* obj = new (mDesc->lastChunk->buff + info->head)T();
	//		info->usedSize += sizeof(T);
	//		if (info->usedSize >= CHUNK_SIZE) {
	//			info->usedSize = CHUNK_SIZE;
	//			info->unUsedSize = 0;
	//			info->head = CHUNK_SIZE;
	//			info->isFull = true;
	//		}
	//		else {
	//			info->head += sizeof(T);
	//			info->usedSize += sizeof(T);
	//			info->unUsedSize -= sizeof(T);
	//		}
	//		return obj;
	//	}

	//	void Remove(int num) {
	//		//first chunk has pool desc, will remove last
	//		if (num <1 ) {
	//			return;
	//		}
	//		if (num >= mDesc->chunkCount) {
	//			Clear();
	//			return;
	//		}
	//		for (int i = 0; i < num; ++i) {
	//			ChunkAllocator::Free(mDesc->lastChunk);
	//			mDesc->lastChunk = mDesc->lastChunk->next;
	//			mDesc->chunkCount--;
	//		}
	//	}

	//	void Clear() {
	//		Chunk* next = nullptr;
	//		Chunk* curr = mDesc->firstChunk->next;
	//		//ChunkAllocator::Free(mFirstChunk);
	//		for (int i = 1; i < mDesc->chunkCount; i++) {
	//			next = curr->next;
	//			ChunkAllocator::Free(curr);
	//			curr = next;
	//		}
	//		mDesc->lastChunk = nullptr;
	//		mDesc->chunkCount = 0;
	//		ChunkAllocator::Free(mDesc->firstChunk);
	//		mDesc->firstChunk = nullptr;
	//	}

	//	~PoolUtil() {
	//		Clear();
	//	}

	//};
};