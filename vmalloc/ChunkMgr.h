#pragma once
#include "chunk.h"

namespace valkyr {
	const int MAX_USE_NEXT_CHUNK = 4;

	//all containers cross many chunks will use it
	class ChunkMgr {
	public:
		Chunk* m_currChunk;
		Chunk* m_firstChunk;
		Chunk* m_lastChunk;
		size_t m_chunkCount;
		int m_numUseNext;
		int pad[3] = { 0,0,0 };

		ChunkMgr() :m_currChunk(nullptr), m_firstChunk(nullptr), m_lastChunk(nullptr),
			m_chunkCount(0), m_numUseNext(0) {}

		static ChunkMgr* Create(size_t n,std::function<void(ChunkMgr*)> f) {
			Chunk* lastChunk = ChunkAllocator::Malloc();
			ChunkMgr* chunkMgr = ChunkUtil::NewObjFrom<ChunkMgr>(lastChunk);
			chunkMgr->m_firstChunk = lastChunk;
			chunkMgr->m_currChunk = lastChunk;
			for (int i = 1; i < n; i++) {
				Chunk* chunk = ChunkAllocator::Malloc();
				ChunkUtil::Connect(lastChunk, chunk);
				lastChunk = chunk;
			}
			chunkMgr->m_lastChunk = lastChunk;
			chunkMgr->m_chunkCount = n;
			chunkMgr->m_numUseNext = 0;
			//g_currChunk = chunkMgr->m_currChunk;
			if(f) f(chunkMgr);
			return chunkMgr;
		}

		inline void AddChunk() {
			Chunk* chunk = ChunkAllocator::Malloc();
			ChunkUtil::Connect(m_lastChunk, chunk);
			m_lastChunk = chunk;
			m_chunkCount++;
		}

		Chunk* UseNextChunk(std::function<void(Chunk*)> onChangeCurrChunk) {
			if (ChunkUtil::GetNext(m_currChunk) == nullptr) {
				AddChunk();
			}
			Chunk* next = ChunkUtil::GetNext(m_currChunk);
			m_numUseNext++;
			if (m_numUseNext >= MAX_USE_NEXT_CHUNK) {
				m_numUseNext = 0;
				m_currChunk = next;
				//g_currChunk = m_currChunk;
				if(onChangeCurrChunk) onChangeCurrChunk(m_currChunk);
			}
			return next;
		};

		void FreeExceptFirst() {
			Chunk* curr = m_lastChunk;
			while (curr != m_firstChunk) {
				Chunk* prev = ChunkUtil::GetPrev(curr);
				ChunkAllocator::Free(curr);
				curr = prev;
			}
		}

		void Destroy(std::function<void()> f) {
			Chunk* curr = m_lastChunk;
			while (curr != nullptr) {
				Chunk* prev = ChunkUtil::GetPrev(curr);
				ChunkAllocator::Free(curr);
				curr = prev;
			}
			//g_currChunk = nullptr; g_chunkMgr = nullptr;
			if(f) f();
		}
	};

	Chunk* g_currChunk = nullptr;
	ChunkMgr* g_chunkMgr = nullptr;
}