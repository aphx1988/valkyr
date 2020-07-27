#pragma once

#include "../vmalloc/chunk.h"
#include <functional>

namespace valkyr {
	template <typename ...Ts>
	struct Tuple;

	//ref(Chinese) https://blog.csdn.net/qq_35553864/article/details/81876833
	//inheritance uses less size than composition
	template <typename Head, typename ...Rest>
	struct Tuple<Head, Rest...>:Tuple<Rest...> {
		Head head;
		Tuple(){}
		//Tuple(Head h,Rest... r):head(h),Tuple<Rest...>(r...){}
		Tuple(Head&& h, Rest&&... r):head(std::forward<Head>(h)), Tuple<Rest...>(std::forward<Rest>(r)...) {}
		Tuple<Rest...>& tail() { return *this; }
;	};

	template <>
	struct Tuple<>{};

	//ref(Chinese) https://zhuanlan.zhihu.com/p/143715615
	//ref(Chinese) https://github.com/Humorly/wstd-tuple

	template <size_t N,typename ...T>
	struct _tuple_element;

	template <size_t N, typename Head, typename ...Rest>
	struct _tuple_element<N, Tuple<Head, Rest...>>
		: public _tuple_element<N - 1, Tuple<Rest...>> {};

	template <typename Head, typename ...Rest>
	struct _tuple_element<0, Tuple<Head, Rest...>>
	{
		using value_type = Head;
		using rest_type = Tuple<Head, Rest...>;
	};

	//ref https://en.cppreference.com/w/cpp/utility/tuple/tuple_size
	template <typename T>
	struct TupleCount;

	template<typename...T>
	struct TupleCount<Tuple<T...>>:public std::integral_constant<size_t,sizeof...(T)> {

	};

	template<typename T>
	struct TupleCount<const T> :public std::integral_constant<size_t, TupleCount<T>::value> {

	};

	//inline variable needs c++17
	template<typename T >
	inline constexpr size_t TupleCountValue = TupleCount<T>::value;

	

	class TupleUtil {
	public:
		template <size_t N, typename ...T>
		static inline decltype(auto) Get(Tuple<T...>& tuple)
		{
			using class_type = typename _tuple_element<N, Tuple<T...>>::rest_type;
			return ((class_type&)tuple).head;
		}

		template <typename ...T>
		static inline Tuple<T...>* Make(Chunk* chunk,T&&... args) {
			return ChunkUtil::NewObjFrom<Tuple<T...>>(chunk,std::forward<T>(args)...);
		}

		/*template <typename ...T, typename F, size_t N>
		static inline void ForEach(Tuple<T...>& tuple, F f) {
			f(T, N);
			ForEach(tuple, N - 1);
		}*/

		/*template <typename ...T, typename F, size_t N>
		static inline void ForEach(Tuple<T...>& tuple, std::function<void(Tuple<T...>& tuple,size_t)> f) {
			f(tuple), N);
			ForEach(tuple, N - 1);
		}

		template <typename ...T>
		static inline void ForEach<T..., 0>(Tuple<T...>& tuple, std::function<void(Tuple<T...>& tuple, size_t)> f) {
			f(tuple, 0);
		}*/

		/*template <typename ...T,typename F,size_t N>
		static inline void ForEach(Tuple<T...>& tuple,std::function<void(decltype(Get<N>(tuple)),size_t)> f) {
			f(Get<N>(tuple), N);
			ForEach(tuple, N - 1);
		}

		template <typename ...T>
		static inline void ForEach<T..., 0>(Tuple<T...>& tuple, std::function<void(decltype(Get<0>(tuple)), size_t)> f) {
			f(Get<0>(tuple), 0);
		}*/

		/*template <typename F,typename ...T, typename std::enable_if_t<std::is_function_v<F>()> = 0>
		static inline void ForEach(Tuple<T...>& tuple,F f) {
			for (int i = 0; i < TupleCountValue<T...>; i++) {
				f(Get<i>(tuple),i);
			}
		}*/
		/*template <typename ...T>
		static inline void ForEach(Tuple<T...>& tuple, std::function<void(Tuple<T...>&, int)> f) {
			for (int i = 0; i < TupleCountValue<T...>; i++) {
				f(tuple,i);
			}
		}*/
		/*template <typename ...T>
		static inline void ForEach(Tuple<T...>& tuple, std::function<void(T, int)>... f) {
			f(T,)
		}*/
	};

}
