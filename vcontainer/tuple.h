#pragma once

#include "../vmalloc/chunk.h"

namespace valkyr {
	template <typename ...Ts>
	struct Tuple;

	template <typename Head, typename ...Rest>
	struct Tuple<Head, Rest...>:Tuple<Rest...> {
		Head head;
		Tuple(){}
		Tuple(Head h,Rest... r):head(h),Tuple<Rest...>(r...){}
		Tuple<Rest...>& tail() { return *this; }
;	};

	template <>
	struct Tuple<>{};

	template <typename Tuple>
	void getTuple(size_t i, Tuple& tuple, std::index_sequence<>)
	{
	}

	template <typename Tuple, size_t... N>
	void getTuple(size_t i, Tuple& tuple, std::index_sequence<N...>)
	{
		constexpr size_t I = index_sequence<N...>::size() - 1;
		if (i == I)
			cout << std::get<I>(tp) << endl;
		else
			visit3(i, tp, make_index_sequence<I>{});
	}

	template <typename Tuple>
	void get(size_t i, Tuple& tuple)
	{
		tuple(i, tp, std::make_index_sequence<std::tuple_size<Tuple>::value>{});
	}

}
