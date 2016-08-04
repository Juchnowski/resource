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
	typename,
	template <typename,typename> typename,
	template <typename,typename> typename,
	template <typename,typename> typename,
	typename>
struct resource;


template<
	typename Type,
	template <typename,typename> typename CleanupPolicy,
	template <typename,typename> typename StoragePolicy,
	template <typename,typename> typename CopyPolicy
>
struct resource_helper {
	using This = resource_helper<Type, CleanupPolicy, StoragePolicy, CopyPolicy>;

	using Cleanup = CleanupPolicy<Type,resource<Type,CleanupPolicy,StoragePolicy,CopyPolicy, This>>;
	using Copy = CopyPolicy<Type,resource<Type,CleanupPolicy,StoragePolicy,CopyPolicy, This>>;
	using Storage = StoragePolicy<Type,resource<Type,CleanupPolicy,StoragePolicy,CopyPolicy, This>>;
};

template<
	typename Type,
	template <typename,typename> typename CleanupPolicy,
	template <typename,typename> typename StoragePolicy,
	template <typename,typename> typename CopyPolicy,
	typename Helper = resource_helper<Type, CleanupPolicy, StoragePolicy, CopyPolicy>
>
struct resource:
		Helper::Storage,
		Helper::Cleanup,
		Helper::Copy
{
	using cleanup = typename Helper::Cleanup;
	using copy = typename Helper::Copy;
	using storage = typename Helper::Storage;

	using traits = traits::get_traits<Type>;
	using type = typename traits::type;

	template<typename T, typename Cleanup = cleanup, typename Copy = copy>
	explicit resource(T&& t, Cleanup&& cl = cleanup{}, Copy&& cp = copy{}):
		storage{std::forward<T>(t)},
		cleanup{std::forward<Cleanup>(cl)},
		copy{std::forward<Copy>(cp)}
	{}

	resource(resource const& other):
		storage(copy::copy_storage(static_cast<storage&>(other))),
		cleanup(copy::copy_cleanup(static_cast<cleanup&>(other))),
		copy(other)
	{}

	resource(resource&& other):
		storage(copy::move_storage(static_cast<storage&&>(*this))),
		storage(copy::move_cleanup(static_cast<cleanup&&>(*this))),
		storage(std::move(static_cast<copy>(*this)))
	{}

	resource& operator=(resource const& other) noexcept(false)
	{
		static_cast<storage&>(*this) = copy::copy_storage(static_cast<storage const&>(other));
		static_cast<cleanup&>(*this) = copy::copy_cleanup(static_cast<cleanup const&>(other));
		static_cast<copy&>(*this) = static_cast<copy const&>(other);
		return *this;
	}

	resource& operator=(resource&& other) noexcept(false)
	{
		static_cast<storage&>(*this) = copy::move_storage(static_cast<storage&&>(other));
		static_cast<cleanup&>(*this) = copy::move_cleanup(static_cast<cleanup&&>(other));
		static_cast<copy&>(*this) = static_cast<copy&&>(other);
		return *this;
	}

	~resource() noexcept(noexcept(std::declval<resource>().cleanup::clean()))
	{
		cleanup::clean();
	}
};

template<
	typename T,
	template <typename,typename> typename Cleanup,
	template <typename,typename> typename Storage,
	template <typename,typename> typename Copy
>
void swap(resource<T,Cleanup, Storage, Copy>& l, resource<T,Cleanup, Storage, Copy>& r){
	resource<T,Cleanup, Storage, Copy>::copy::swap(l, r);
}


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
