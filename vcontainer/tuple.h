#pragma once
#include "../vmalloc/chunk.h"
namespace valkyr
{
	template <typename ...Ts>
	struct Tuple;

	template<typename T> 
	struct Tuple<T>{
		T value;
		enum {
			Size = sizeof(T)
		};
		Tuple(T t):value(t){}
	};

	template <typename Head,typename ...Rest>
	struct Tuple<Head, Rest...>
	{
		Head value;
		/*Tuple<Rest...> rest;
		enum {
			Size = Tuple<Head>::Size + Tuple<Rest...>::Size
		};*/

		Tuple(Head h,Rest... r):value(h)/*,rest(r...)*/
		{

		}
	};

	template <int Idx,typename ...Ts>
	struct _tuple_with_idx;

	template <int Idx>
	struct _tuple_with_idx<Idx, Tuple<>> {
		static_assert(0 <= Idx, "Index outside of tuple!");
	};




	class TupleUtil {
	public:
		/*template<typename Head, typename ...Rest>
		static inline Head GetFrom(Tuple<Head, Rest...>&& tuple) {
			return std::forward(tuple.value);
		}

		template<typename T>
		static inline T GetFrom(Tuple<T>&& tuple) {
			return std::forward(tuple.value);
		}*/
	};
}