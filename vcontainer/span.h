#pragma once

#include <functional>
#include "tuple.h"

namespace valkyr{
	//a continuous or linear memory store a type in a chunk
	//can be multiple in a single chunk, so has prev and next for connection
	//all elements are fixed, can not add more or remove

	int g_EntityId = -1;

	struct SpanEntity {
		unsigned int id;
		unsigned int version;
	};

	template <typename T>
	struct Span {
		unsigned int firstHead;
		unsigned int count;
		Chunk* chunk;
		Span<T>* next;
		Span<T>* prev;
		int pad[3] = { 0,0,0 };

		Span() : firstHead(0), count(0), chunk(nullptr), next(nullptr), prev(nullptr)
		{
		}

		Span(Chunk* chunk) :
			count(0), chunk(chunk), next(nullptr), prev(nullptr) {
			firstHead = ChunkUtil::GetInfo(chunk)->head;
		}
	};

	

	class SpanUtil {
	public:

		template <typename ...T>
		static inline Span<Tuple<T...>>* Create(Chunk* chunk,int num) {
			if (!CanCreate<T...>(chunk,num)) return nullptr;
			Span<Tuple<T...>>* span = ChunkUtil::NewObjFrom<Span<Tuple<T...>>>(chunk);
			span->chunk = chunk;
			//then all types
			//in memory:span-types...-types...
			span->firstHead = ChunkUtil::GetInfo(chunk)->head;
			for (int i = 0; i < num; i++) {
				vmake_tuple<T...>(chunk);
				SpanEntity* entity = ChunkUtil::NewObjFrom<SpanEntity>(chunk);
				g_EntityId++;
				entity->id = g_EntityId;
				entity->version = 0;
			}
			span->count = num;
			return span;
		}

		template <typename ...T>
		static inline Span<Tuple<T...>>* Create(Chunk* chunk, int num, T... prototype) {
			if (!CanCreate<T...>(chunk, num)) return nullptr;
			Span<Tuple<T...>>* span = ChunkUtil::NewObjFrom<Span<Tuple<T...>>>(chunk);
			span->chunk = chunk;
			span->firstHead = ChunkUtil::GetInfo(chunk)->head;
			//then all types
			//in memory:span-types...-types...
			for (int i = 0; i < num; i++) {
				vmake_tuple<T...>(chunk,prototype...);
				SpanEntity* entity = ChunkUtil::NewObjFrom<SpanEntity>(chunk);
				g_EntityId++;
				entity->id = g_EntityId;
				entity->version = 0;
			}
			span->count = num;
			return span;
		}

		template <typename T, typename ...Args>
		static inline Span<T>* Create(Chunk* chunk, int num, Args... args) {
			if (!CanCreate<T>(chunk,num)) return nullptr;
			Span<T>* span = ChunkUtil::NewObjFrom<Span<T>>(chunk);
			span->chunk = chunk;
			span->firstHead = ChunkUtil::GetInfo(chunk)->head;
			//then all types
			//in memory:span-types...-types...
			for (int i = 0; i < num; i++) {
				ChunkUtil::NewObjFrom<T>(chunk,args...);
				SpanEntity* entity = ChunkUtil::NewObjFrom<SpanEntity>(chunk);
				g_EntityId++;
				entity->id = g_EntityId;
				entity->version = 0;
			}
			span->count = num;
			return span;
		}

		template <typename T>
		static inline Span<T>* Create(Chunk* chunk, int num) {
			if (!CanCreate<T>(chunk, num)) return nullptr;
			Span<T>* span = ChunkUtil::NewObjFrom<Span<T>>(chunk);
			span->chunk = chunk;
			span->firstHead = ChunkUtil::GetInfo(chunk)->head;
			//then all types
			//in memory:span-types...-types...
			for (int i = 0; i < num; i++) {
				ChunkUtil::NewObjFrom<T>(chunk);
				SpanEntity* entity = ChunkUtil::NewObjFrom<SpanEntity>(chunk);
				g_EntityId++;
				entity->id = g_EntityId;
				entity->version = 0;
			}
			span->count = num;
			return span;
		}

		template <typename ...T>
		static inline bool CanCreate(Chunk* chunk, int num) {
			size_t elementSize = sizeof(Tuple<T...>)+sizeof(SpanEntity);
			size_t spanSize = sizeof(Span<Tuple<T...>>);
			ChunkInfo* info = ChunkUtil::GetInfo(chunk);
			return CHUNK_SIZE - info->usedSize >= elementSize * num + spanSize;
		}

		template <typename T>
		static inline bool CanCreate(Chunk* chunk,int num) {
			size_t elementSize = sizeof(T)+sizeof(SpanEntity);
			size_t spanSize = sizeof(Span<Tuple<T...>>);
			ChunkInfo* info = ChunkUtil::GetInfo(chunk);
			return CHUNK_SIZE - info->usedSize >= elementSize*num + spanSize;
		}

		template <typename T>
		static inline T* Get(unsigned int idx, Span<T>* span) {
			size_t tsize = sizeof(T) + sizeof(SpanEntity);
			return ChunkUtil::Get<T>(span->firstHead + idx * tsize, span->chunk);
		}

		template <typename T>
		static inline SpanEntity* GetEntity(unsigned int idx, Span<T>* span) {
			size_t tsize = sizeof(T) + sizeof(SpanEntity);
			return ChunkUtil::Get<SpanEntity>(span->firstHead + idx * tsize + sizeof(T), span->chunk);
		}

		template <typename T>
		static inline void Connect(Span<T>* span, Span<T>* next) {
			if (span->chunk == next->chunk) {
				span->next = next;
				next->prev = span;
			}
		}

		/*template <typename T>
		static inline void ForEach(Span<T>* span, std::function<void(T*,int)> func) {
			for (int i = 0; i < span->count; ++i) {
				func(Get(i,span),i);
			}
		}*/

		template <typename T>
		static inline void Zero(Span<T>* span, int idx) {
			size_t tsize = sizeof(T) + sizeof(SpanEntity);
			ChunkUtil::Zero(span->chunk, span->firstHead + idx * tsize,sizeof(T));
			GetEntity(idx, span)->version++;
		}

		template <typename T>
		static inline void ZeroRange(Span<T>* span, int startIdx, size_t n) {
			//ChunkUtil::Zero(span->chunk, span->firstHead + startIdx * sizeof(T), (n > span->count ? span->count : n) * sizeof(T));
			//zero data but not span entity
			for (int i = 0; i < n; i++) {
				SpanUtil::Zero(span, startIdx+i);
			}
		}

		template <typename T>
		static inline void ZeroAll(Span<T>* span) {
			SpanUtil::ZeroRange(span, 0, span->count);
		}
	};
}
