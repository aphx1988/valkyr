#pragma once

#include "span.h"
//#include "vec.h"
#include "../vmalloc/ChunkMgr.h"

namespace valkyr {

	template <class T>
	void vfor_each(Span<T>* container,std::function<void(T*,int)> f) {
		for (int i = 0; i < container->count; ++i) {
			f(SpanUtil::Get(i, container), i);
		}
	}

	void vfor_each(ChunkMgr* container, std::function<void(Chunk*, int)> f) {
		Chunk* curr = container->m_firstChunk;
		int i = 0;
		while (curr != nullptr) {
			f(curr,i);
			Chunk* next = ChunkUtil::GetNext(curr);
			curr = next;
			i++;
		}
	}
}