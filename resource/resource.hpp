#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include <type_traits>

namespace kq::resource
{

namespace traits
{

template<typename T>
struct default_traits
{
	using type = T;
	using handle = type*;
	static constexpr bool is_nullable = true;
	static constexpr handle null = nullptr;

	static type deref(handle h) {
		return *h;
	}
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
	template <typename,typename> typename CleanupPolicy,
	template <typename,typename> typename StoragePolicy,
	template <typename,typename> typename CopyPolicy
>
struct resource:
		StoragePolicy<Type,resource<Type,CleanupPolicy,StoragePolicy,CopyPolicy>>,
		CleanupPolicy<Type,resource<Type,CleanupPolicy,StoragePolicy,CopyPolicy>>,
		CopyPolicy<Type,resource<Type,CleanupPolicy,StoragePolicy,CopyPolicy>>
{
	using cleanup = CleanupPolicy<Type,resource<Type,CleanupPolicy,StoragePolicy,CopyPolicy>>;
	using copy = CopyPolicy<Type,resource<Type,CleanupPolicy,StoragePolicy,CopyPolicy>>;
	using storage = StoragePolicy<Type,resource<Type,CleanupPolicy,StoragePolicy,CopyPolicy>>;
	using traits = traits::get_traits<Type>;
	using type = typename traits::type;

	template<typename T, typename Cleanup = cleanup, typename Copy = copy>
	explicit resource(T&& t, Cleanup&& cl = cleanup{}, Copy&& cp = copy{}):
		storage{std::forward<T>(t)},
		cleanup{std::forward<Cleanup>(cl)},
		copy{std::forward<Copy>(cp)}
	{}

	~resource() {
		cleanup::clean();
	}
};

template<int N, typename T, typename U> struct PolicyN{};

template<int N>
struct PolicyImpl{
	template<typename T, typename U>
	struct Inner{
		using type = PolicyN<N,T,U>;
	};
};

} // namespace kq::resource

#endif // RESOURCE_HPP
