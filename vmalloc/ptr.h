#pragma once

#include <memory>

namespace valkyr {
	template <typename T>
	using Ptr = std::shared_ptr<T>;

	template <typename T>
	using SharedPtr = Ptr<T>;

	template <typename T>
	using UniPtr = std::unique_ptr<T>;

	template <typename T>
	using WeakPtr = std::weak_ptr<T>;

	template <typename T>
	using vmake_ptr = std::make_shared<T>;

	template <typename T>
	using vmake_unique = std::make_unique<T>;
 }