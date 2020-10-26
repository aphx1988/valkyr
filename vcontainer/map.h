#pragma once
#include <unordered_map>

namespace valkyr {
	template <typename K,typename V>
	using Map = std::unordered_map<K,V>;
}
