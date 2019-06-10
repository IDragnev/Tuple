#pragma once

#include "TypeFunctionsAndPredicates.h"
#include "TypeList.h"
#include "ValueList.h"

namespace IDragnev::Meta
{
	template <typename List, std::size_t N>
	struct ListRefT : ListRefT<Tail<List>, N - 1> { };

	template <typename List>
	struct ListRefT<List, 0> : HeadT<List> { };

	template <typename List, std::size_t N>
	using ListRef = typename ListRefT<List, N>::type;

	template <typename Lhs, typename Rhs, bool = isEmpty<Rhs>>
	struct ConcatT
	{
		using type = Lhs;
	};

	template <typename Lhs, typename Rhs>
	struct ConcatT<Lhs, Rhs, false> : ConcatT<InsertBack<Lhs, Head<Rhs>>, Tail<Rhs>> { };

	template <typename Lhs, typename Rhs>
	using Concat = typename ConcatT<Lhs, Rhs>::type;

	template <typename List, bool = isEmpty<List>>
	struct ReverseT
	{
	private:
		using ReversedTail = typename ReverseT<Tail<List>>::type;
		using Head = Head<List>;

	public:
		using type = InsertBack<ReversedTail, Head>;
	};

	template<typename List>
	struct ReverseT<List, true>
	{
		using type = List;
	};

	template <typename List>
	using Reverse = typename ReverseT<List>::type;

	template <template <typename, typename> typename F,
		      typename L1,
		      typename L2>
	struct ZipT;

	template <template <typename, typename> typename F,
		      typename... Ts,
		      typename... Fs>
	struct ZipT<F, TypeList<Ts...>, TypeList<Fs...>>
	{
		using type = TypeList<typename F<Ts, Fs>::type...>;
	};

	template <template <typename, typename> typename F,
		      typename L1,
		      typename L2>
	using Zip = typename ZipT<F, L1, L2>::type;

	template <template <typename> typename F,
		      typename List>
	struct MapT;

	template <template <typename> typename F,
		      typename... Vals>
	struct MapT<F, TypeList<Vals...>>
	{
		using type = TypeList<typename F<Vals>::type...>;
	};

	template <template <typename> typename F,
		      typename List>
	using Map = typename MapT<F, List>::type;

	template <template <typename Res, typename Current> typename Op,
		      typename Initial,
		      typename List,
	          bool = isEmpty<List>>
	struct FoldLeftT : FoldLeftT<Op, typename Op<Initial, Head<List>>::type, Tail<List>> { };

	template <template <typename Res, typename Current> typename Op,
		      typename Initial,
	          typename List>
	struct FoldLeftT<Op, Initial, List, true>
	{
		using type = Initial;
	};

	template <template <typename Res, typename Current> typename Op,
		      typename Initial,
		      typename List>
	using FoldLeft = typename FoldLeftT<Op, Initial, List>::type;

	template <typename List>
	struct LargestTypeT : FoldLeftT<LargerT, Head<List>, Tail<List>> { };

	template<>
	struct LargestTypeT<TypeList<>> { };

	template <typename List>
	using LargestType = typename LargestTypeT<List>::type;

	template <typename L>
	using AccReverse = FoldLeft<InsertFrontT, TypeList<>, L>;

	template <typename Lhs, typename Rhs>
	using AccConcat = FoldLeft<InsertBackT, Lhs, Rhs>;

	template <template <typename> typename Predicate,
		      typename List,
	          bool = isEmpty<List>>
	struct FilterT
	{
	private:
		using H = Head<List>;
		using FilteredTail = typename FilterT<Predicate, Tail<List>>::type;
		using Result = std::conditional_t<Predicate<H>::value,
			                              InsertFrontT<FilteredTail, H>,
			                              IdentityT<FilteredTail>>;
	public:
		using type = typename Result::type;
	};

	template <template <typename> typename Predicate,
	          typename List>
	struct FilterT<Predicate, List, true>
	{
		using type = List;
	};

	template <template <typename> typename Predicate,
		      typename List>
	using Filter = typename FilterT<Predicate, List>::type;

	template <unsigned N, typename List>
	struct DropT : DropT<N - 1, Tail<List>> { };

	template <typename List>
	struct DropT<0, List>
	{
		using type = List;
	};

	template <unsigned N, typename List>
	using Drop = typename DropT<N, List>::type;

	template <unsigned N, typename List>
	struct TakeT
	{
		using type = InsertFront<typename TakeT<N - 1, Tail<List>>::type, 
			                     Head<List>>;
	};

	template <typename List>
	struct TakeT<0, List>
	{
		using type = EmptyList<List>;
	};

	template <unsigned N, typename List>
	using Take = typename TakeT<N, List>::type;

	template <unsigned N, typename Result = ValueList<unsigned>>
	struct MakeIndexListT : MakeIndexListT<N - 1, InsertFront<Result, CTValue<unsigned, N - 1>>> { };

	template <typename Result>
	struct MakeIndexListT<0, Result>
	{
		using type = Result;
	};

	template <unsigned N>
	using MakeIndexList = typename MakeIndexListT<N>::type;

	template <auto Value,
		      unsigned N,
		      typename Result = ValueList<decltype(Value)>
	> struct ReplicateValueT : 
		ReplicateValueT<Value, N - 1, InsertFront<Result, CTValue<decltype(Value), Value>>> { };

	template <auto Value, typename Result>
	struct ReplicateValueT<Value, 0, Result>
	{
		using type = Result;
	};

	template <auto V, unsigned N>
	using ReplicateValue = typename ReplicateValueT<V, N>::type;

	template <typename T,
		      typename List,
		      template <typename U, typename V> typename Compare,
		      bool = isEmpty<List>
	> struct InsertInSortedT;

	template <typename T,
		      typename List,
		      template <typename U, typename V> typename Compare>
	using InsertInSorted = typename InsertInSortedT<T, List, Compare>::type;

	template <typename List,
		      template <typename U, typename V> typename Compare,
		      bool = isEmpty<List>
	> struct InsertionSortT;

	template <typename List,
		      template <typename U, typename V> typename Compare> 
	using InsertionSort = typename InsertionSortT<List, Compare>::type;

	template <typename List,
		      template <typename U, typename V> typename Compare
    > struct InsertionSortT<List, Compare, true>
	{
		using type = List;
	};

	template <typename List,
		      template <typename U, typename V> typename Compare
	> struct InsertionSortT<List, Compare, false> : 
		InsertInSortedT<Head<List>, InsertionSort<Tail<List>, Compare>, Compare> { };

	template <typename T,
		      typename List,
		      template <typename U, typename V> typename Compare
	> struct InsertInSortedT<T, List, Compare, true> : InsertFrontT<List, T> { };

	template <typename T,
		      typename List,
		      template <typename U, typename V> typename Compare
	> struct InsertInSortedT<T, List, Compare, false>
	{
	private:
		using NewTailT = std::conditional_t<Compare<T, Head<List>>::value,
			                                IdentityT<List>,
			                                InsertInSortedT<T, Tail<List>, Compare>>;
		using NewTail = typename NewTailT::type;
		using NewHead = std::conditional_t<Compare<T, Head<List>>::value,
			                               T,
			                               Head<List>>;
	public:
		using type = InsertFront<NewTail, NewHead>;
	};

	template <typename List,
		      template <typename U, typename V> typename CompareFn
	> struct MakeIndexedCompareT
	{
		template <typename T, typename F> struct Compare;

		template <unsigned I, unsigned J>
		struct Compare<CTValue<unsigned, I>, CTValue<unsigned, J>> : 
			CompareFn<ListRef<List, I>, ListRef<List, J>> { };
	};

	template <template <typename T> typename Predicate,
		      typename List,
		      bool = isEmpty<List>
	> struct AllOfT;

	template <template <typename T> typename Predicate,
		      typename List>
	inline constexpr bool allOf = AllOfT<Predicate, List>::value;

	template <template <typename T> typename Predicate,
		      typename List>
	struct AllOfT<Predicate, List, true> : std::true_type { };

	template <template <typename T> typename Predicate,
		      typename List> 
	struct AllOfT<Predicate, List, false> : 
		std::bool_constant<Predicate<Head<List>>::value && allOf<Predicate, Tail<List>>> { };

	template <template <typename T> typename Predicate,
		      typename List
	> struct NoneOfT : AllOfT<Inverse<Predicate>::template Result, List> { };

	template <template <typename T> typename Predicate,
		      typename List>
	inline constexpr bool noneOf = NoneOfT<Predicate, List>::value;
}
