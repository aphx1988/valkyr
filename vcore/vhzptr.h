#pragma once
#include <memory>

using std::shared_ptr;

namespace valkyr {
	//constinitûЧ
	constexpr size_t PADDING_SIZE = std::hardware_constructive_interference_size - 3*sizeof(void*)-8*sizeof(bool);


	template <class T>
	struct vhzptr {
		T* p_data;
		alignas(8) bool retired;
		std::atomic<vhzptr<T>*> next;
		std::atomic<vhzptr<T>*> prev;
		char paddings[PADDING_SIZE];

		constexpr vhzptr() noexcept = default;
		constexpr vhzptr(nullptr_t) noexcept {}
		vhzptr(const vhzptr<T>& rhs) noexcept:p_data(rhs),retired(rhs.retired), next(rhs.next), prev(rhs.prev){}
		vhzptr<T>& operator=(const vhzptr<T>& rhs) noexcept {
			//after asign, this ptr will be removed from old ptr list, and add to the rhs'one
			//if this ptr is the only ptr is the old ptr list, wll reclaim the old memory
			if (this != &rhs) {
				T* old_data = p_data;
				p_data = rhs.p_data;
				retired = rhs.retired;
				if (!next && !prev) {
					delete old_data;
				}
				else if (this->next != nullptr && this->prev != nullptr) {
					vhzptr<T>* old_next = next;
					vhzptr<T>* old_prev = prev;
					next = &rhs;
					rhs.prev = this;
					old_next->prev = old_prev;
					old_prev->next = old_next;
				}
				else {
					if (next) {
						vhzptr<T>* old_next = next;
					}
					if (prev) {
						vhzptr<T>* old_prev = prev;
					}
					next = &rhs;
				}
			}
			return *this;
		}

	};


}
