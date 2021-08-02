#include "vslab.h"

template<class T>
bool valkyr::slab::Cache<T>::alloc_slab(size_t n)
{
	assert(this->base == nullptr);
	assert((sizeof(T))*n<=this->capacity)
	for (int i = 0; i < n; i++) {

	}
}


template<class T>
void valkyr::slab::Cache<T>::clear()
{
}

template<class T>
T* valkyr::slab::Cache<T>::req()
{
	return nullptr;
}

template<class T>
void valkyr::slab::Cache<T>::free(T*)
{
}

