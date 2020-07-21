#pragma once
#include <functional>
#include "../vmalloc/chunk.h"

namespace valkyr {
	template <typename ...Ts>
	struct MultiTypeSpan;

	template <typename Head, typename ...Rest>
	struct MultiTypeSpan<Head, Rest...> {
		unsigned int startIdx;
		unsigned int count;
		Chunk* chunk;
		MultiTypeSpan<Head, Rest...>* next;
		MultiTypeSpan<Head, Rest...>* prev;
		/*Head* head;
		MultiTypeSpan<Rest...>* rest;*/
		int pad[2];
		enum { 
			ElementSize = MultiTypeSpan<Head>::ElementSize+ MultiTypeSpan<Rest...>::ElementSize
		};

		MultiTypeSpan() : startIdx(0), count(0), chunk(nullptr), next(nullptr), prev(nullptr)
		{
		}

		MultiTypeSpan(Chunk* chunk) :
			count(0), chunk(chunk), next(nullptr), prev(nullptr) {
			startIdx = ChunkUtil::GetInfo(chunk)->head;
		}
	};

	template<typename T>
	struct MultiTypeSpan<T> {
		enum {
			ElementSize = sizeof(T)
		};
	};
}


