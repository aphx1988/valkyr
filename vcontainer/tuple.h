#pragma once
#include "../vmalloc/chunk.h"
namespace valkyr
{
	template <typename ...Ts>
	struct Tuple;

	template <> struct Tuple<>{};

	template <typename Head,typename ...Rest>
	struct Tuple<Head, Rest...>
	{
		Head value;
		Tuple<Rest...> rest;

		Tuple(Head h,Rest... r):value(h),Tuple<Rest...>(r)
		{

		}
	};
}