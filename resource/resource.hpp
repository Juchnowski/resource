#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include <type_traits>

namespace kq
{

namespace resource
{

namespace traits
{

template<typename T>
struct default_traits
{
	using type = T;
	using handle = type*;
};

namespace detail
{
template<typename T>
struct is_trait
{
	template<typename U>
	static std::true_type test(int, typename U::type* = nullptr);

	template<typename>
	static std::false_type test(...);

	using type = decltype(test<T>(0));
	static constexpr bool value = std::is_same<type, std::true_type>::value;
};

template<typename T>
constexpr bool is_trait_v = is_trait<T>::value;

template<typename T, bool = is_trait_v<T>>
struct get_traits
{
	using type = default_traits<T>;
};

template<typename T>
struct get_traits<T, true>
{
	using type = T;
};

} // detail

template<typename T>
using get_traits = typename detail::get_traits<T>::type;

} // traits

template<
	typename Type,
	template <typename> typename CleanupPolicy,
	template <typename> typename StoragePolicy,
	template <typename> typename CopyPolicy
>
struct resource:
		CleanupPolicy<resource<Type,CleanupPolicy,CopyPolicy,StoragePolicy>>,
		StoragePolicy<resource<Type,CleanupPolicy,CopyPolicy,StoragePolicy>>,
		CopyPolicy<resource<Type,CleanupPolicy,CopyPolicy,StoragePolicy>>
{
	using cleanup = CleanupPolicy<resource<Type,CleanupPolicy,CopyPolicy,StoragePolicy>>;
	using copy = CopyPolicy<resource<Type,CleanupPolicy,CopyPolicy,StoragePolicy>>;
	using storage = StoragePolicy<resource<Type,CleanupPolicy,CopyPolicy,StoragePolicy>>;
	using traits = traits::get_traits<Type>;
	using type = typename traits::type;

	template<typename T, typename Cleanup = cleanup, typename Storage = storage, typename Copy = copy>
	explicit resource(T&& t, Cleanup&& cl = cleanup{}, Storage&& s = storage{}, Copy&& cp = copy{}):
		cleanup{std::forward<Cleanup>(cl)},
		storage{std::forward<Storage>(s)},
		copy{std::forward<Copy>(cp)}
	{}
};

template<int N, typename T> struct PolicyN{};

template<int N>
struct PolicyImpl{
	template<typename T>
	struct Inner{
		using type = PolicyN<N,T>;
	};
};

} // namespace resource

} // namespace kq

#endif // RESOURCE_HPP
