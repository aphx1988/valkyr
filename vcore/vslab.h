#pragma once
#include <cstdint>
#include <ranges>
#include "bitop.h"

const uint16_t CACHE_LINE_SIZE = 128;

template <class T>
struct SlabItem;

template <class T>
struct Slab
{
	char* buf;
	uint16_t usage_bits;
	uint16_t cap;
	uint8_t color_offset;
	SlabItem<T>* start_item;
	SlabItem<T>* free_item;

	SlabItem<T>* pop() {
		if (this->free_item == nullptr) {
			return nullptr;
		}
		reclaim(this->free_item);
		return this->free_item;
	}

	void reclaim(SlabItem<T>* slab_item) {
		auto next_usage = reverse_mask(this->usage_bits, 1 << slab_item->offset);
		auto next_off = tail_one(next_usage);
		if (next_off == std::nullopt) {
			return;
		}
		this->free_item = this->start_item + next_off.value();
		this->usage_bits = next_usage;
	}

	SlabItem<T>* get(uint16_t i) {
		return this->start_item + i * sizeof(SlabItem<T>);
	}

	SlabItem<T>* operator[](int i) {
		return this->start_item + i*sizeof(SlabItem<T>);
	}
};

template <class T>
struct SlabItem {
	Slab<T>* slab;
	uint16_t offset;
	T data;

	SlabItem<T>() {}
};

template<class T>
struct Cache {
	template<class ...Args>
	Slab<T>* create(char* buf, uint16_t cap, uint8_t col_off,Args... args) {
		auto data_start = buf + col_off;
		Slab<T>* slab = new (data_start)Slab<T>();
		slab->cap = cap;
		slab->buf = buf;
		slab->usage_bits = 0;
		slab->color_offset = col_off;
		auto slab_start = data_start + sizeof(Slab<T>);
		for (auto i = 0; i < cap; i++) {
			auto item_start = slab_start + sizeof(SlabItem<T>)*i;
			SlabItem<T>* item = new(item_start)SlabItem<T>();
			item->slab = slab;
			item->offset = i;
			item->data = T(std::forward<Args>(args)...);
			if (i == 0) slab->start_item = item;
			else if (i == cap - 1) slab->free_item = item;
		}
		return slab;
	}
};
