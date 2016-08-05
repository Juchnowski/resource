#ifndef TRAITS_HPP
#define TRAITS_HPP

namespace kq::resource::traits
{

enum nullable
{
	no,
	yes
};

template<typename T>
struct default_traits
{
	using type = T;
	using handle = type*;
	static constexpr nullable is_nullable = nullable::yes;
	static constexpr handle null = nullptr;

	static type deref(handle h) noexcept(noexcept(*h))
	{
		return *h;
	}
};

template<typename T, nullable Nullable = nullable::no, typename NullType = void*, NullType Null = nullptr>
struct handle_is_type
{
	using type = T;
	using handle = type;
	static constexpr nullable is_nullable = Nullable;

	static type deref(handle h) noexcept(noexcept(handle(h)))
	{
		return h;
	}
};

template<typename T, T Null>
struct handle_is_type<T, nullable::yes, T, Null> : handle_is_type<T,nullable::no>
{
	static constexpr nullable is_nullable = nullable::yes;
	static constexpr typename handle_is_type::handle null = Null;
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

} // kq::resource::traits

#endif // TRAITS_HPP
