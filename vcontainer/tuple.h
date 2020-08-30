#pragma once

#include "../vmalloc/chunk.h"

namespace valkyr {

	template <typename ...T>
	using Tuple = std::tuple<T...>;

	template <typename ...T>
	std::tuple<T...>* vmake_tuple_ptr(Chunk* chunk) {
		return ChunkUtil::NewObjFrom<Tuple<T...>>(chunk);
	}

	template <typename ...T>
	std::tuple<T...>* vmake_tuple_ptr(Chunk* chunk,T... args) {
		return ChunkUtil::NewObjFrom<Tuple<T...>,T...>(chunk,args...);
	}

	template <typename ...T>
	std::tuple<T...> vmake_tuple(Chunk* chunk) {
		return *(vmake_tuple_ptr<T...>(chunk));
	}

	template <typename ...T>
	std::tuple<T...> vmake_tuple(Chunk* chunk, T... args) {
		return *(vmake_tuple_ptr<T...>(chunk,args...));
	}

	template <size_t N,class Tuple>
	decltype(auto) vget(Tuple tuple) {
		return std::get<N>(tuple);
	}

	template <class T, class Tuple>
	decltype(auto) vget(Tuple tuple) {
		return std::get<T>(tuple);
	}

	template <size_t N, class Tuple>
	decltype(auto) vget(Tuple* tuple) {
		return std::get<N>(*tuple);
	}

	template <class T, class Tuple>
	decltype(auto) vget(Tuple* tuple) {
		return std::get<T>(*tuple);
	}

	template <size_t N, class T, class Tuple>
	T* vget_ptr(Tuple tuple) {
		return &std::get<N>(tuple);
	}

	template < size_t N, class T, class Tuple>
	T* vget_ptr(Tuple* tuple) {
		return &std::get<N>(*tuple);
	}

	template <class T, class Tuple>
	T* vget_ptr(Tuple tuple) {
		return &std::get<T>(tuple);
	}

	template <class T,class Tuple>
	T* vget_ptr(Tuple* tuple) {
		return &std::get<T>(*tuple);
	}

//	template <typename ...Ts>
//	struct Tuple;
//
//	//ref(Chinese) https://blog.csdn.net/qq_35553864/article/details/81876833
//	//inheritance uses less size than composition
//	template <typename Head, typename ...Rest>
//	struct Tuple<Head, Rest...>:Tuple<Rest...> {
//		Head head;
//		Tuple(){}
//		//Tuple(Head h,Rest... r):head(h),Tuple<Rest...>(r...){}
//		Tuple(Head&& h, Rest&&... r):head(std::forward<Head>(h)), Tuple<Rest...>(std::forward<Rest>(r)...) {}
//		Tuple<Rest...>& tail() { return *this; }
//;	};
//
//	template <>
//	struct Tuple<>{};
//
//	//ref(Chinese) https://zhuanlan.zhihu.com/p/143715615
//	//ref(Chinese) https://github.com/Humorly/wstd-tuple
//	template <size_t N,typename ...T>
//	struct _tuple_element;
//
//	template <size_t N, typename Head, typename ...Rest>
//	struct _tuple_element<N, Tuple<Head, Rest...>>
//		: public _tuple_element<N - 1, Tuple<Rest...>> {};
//
//	template <typename Head, typename ...Rest>
//	struct _tuple_element<0, Tuple<Head, Rest...>>
//	{
//		using value_type = Head;
//		using rest_type = Tuple<Head, Rest...>;
//	};
//
//	//ref https://en.cppreference.com/w/cpp/utility/tuple/tuple_size
//	template <typename T>
//	struct TupleCount;
//
//	template<typename...T>
//	struct TupleCount<Tuple<T...>>:public std::integral_constant<size_t,sizeof...(T)> {
//
//	};
//
//	template<typename T>
//	struct TupleCount<const T> :public std::integral_constant<size_t, TupleCount<T>::value> {
//
//	};
//
//	//inline variable needs c++17
//	template<typename T >
//	inline constexpr size_t TupleCountValue = TupleCount<T>::value;
//
//	
//
//	class TupleUtil {
//	public:
//		template <size_t N, typename ...T>
//		static inline decltype(auto) Get(Tuple<T...>& tuple)
//		{
//			using class_type = typename _tuple_element<N, Tuple<T...>>::rest_type;
//			return ((class_type&)tuple).head;
//		}
//
//		template <size_t N, typename ...T>
//		static inline decltype(auto) GetPtr(Tuple<T...>* tuple)
//		{
//			using class_type = typename _tuple_element<N, Tuple<T...>>::rest_type;
//			return &(((class_type*)tuple)->head);
//		}
//
//		template <typename ...T>
//		static inline Tuple<T...> Make(Chunk* chunk, T... args) {
//			return *(ChunkUtil::NewObjFrom<Tuple<T...>>(chunk, std::forward<T>(args)...));
//		}
//
//		template <typename ...T>
//		static inline Tuple<T...>* MakePtr(Chunk* chunk,T... args) {
//			return ChunkUtil::NewObjFrom<Tuple<T...>>(chunk,std::forward<T>(args)...);
//		}
//	};

}
