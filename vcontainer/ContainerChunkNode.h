#pragma once

//#include "span.h"
#include "algorithm.h"

namespace valkyr {
	const int NUM_PRECREATED = 8;
	//store in each chunk used by a container, providing info for the chunk's elements 
	//and for fast visiting elements in a specific chunk
	//a proxy of a container in each chunk
	template <typename T>
	struct ContainerChunkNode {
		ContainerChunkNode* prev;
		ContainerChunkNode* next;
		ChunkInfo* chunkInfo;
		Chunk* chunk;
		Span<T>* firstSpan;
		unsigned int spanCount;
		int pad[2] = {0,0};

		ContainerChunkNode():prev(nullptr),next(nullptr),chunkInfo(nullptr),
			chunk(nullptr),spanCount(0),firstSpan(nullptr){}
	};

	class ContainerChunkNodeUtil {
	public:
		template <typename T>
		static Span<T>* NewSpan(ContainerChunkNode<T>* node) {
			Span<T>* span = SpanUtil::Create(node->chunk, NUM_PRECREATED);
			if(span!=nullptr)
				node->spanCount++;
			return span;
		}

		//check if node's chunk has zero T,or create one with a new span
		template <typename T>
		static T* PickZero(ContainerChunkNode<T>* node,std::function<void()> onNewSpan) {
			Span<T>* span = node->firstSpan;
			T* azero = nullptr;
			while (span != nullptr && azero == nullptr) {
				azero = vpick<Span<T>, T>(span,
					[&](auto* curr) { return SpanUtil::PickZero(curr); });
				span = span->next;
			}
			if(azero!=nullptr)
				return azero;
			span = NewSpan(node);
			if (span != nullptr) {
				azero =  SpanUtil::Get(0, span);
				if (onNewSpan) onNewSpan();
			}
			return azero;
		}

		template <typename T>
		static std::pair<T*,SpanEntity<T>*> PickZeroWithEntity(ContainerChunkNode<T>* node,std::function<void()> onNewSpan) {
			Span<T>* span = node->lastChunkNode->firstSpan;
			auto azero = nullptr;
			while (span != nullptr && azero == nullptr) {
				azero = vpick_with_entity<Span<T>, T>(span,
					[&](auto curr) { return SpanUtil::PickZeroWithEntity(curr); });
				span = span->next;
			}
			if (azero != nullptr) {
				return azero;
			}
			span = NewSpan(node);
			if (span != nullptr) {
				azero = SpanUtil::GetWithEntity(0, span);
				if (onNewSpan) onNewSpan();
			}
			return azero;
		}
	};
}
