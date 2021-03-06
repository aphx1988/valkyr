#pragma once
#include <memory>

namespace valkyr {
	template <typename T>
	using vptr = std::shared_ptr<T>;

#define vmake_ptr std::make_shared
}