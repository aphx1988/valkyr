#pragma once
#include <memory>

using std::shared_ptr;

namespace valkyr {
	constexpr size_t PADDING_SIZE = std::hardware_constructive_interference_size - 3*sizeof(void*)-8*sizeof(bool);

	template <class T>
	struct vhzptr {
		T* p_data;
		alignas(8) bool retired;
		std::atomic<vhzptr<T>*> next;
		std::atomic<vhzptr<T>*> prev;
		char paddings[PADDING_SIZE];
	};
}
