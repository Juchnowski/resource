#ifndef TRAITS_HPP
#define TRAITS_HPP

#include <type_traits>

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
	using pointer = type*;
	static constexpr nullable is_nullable = nullable::yes;
	static constexpr handle null = nullptr;

	static type deref(handle h) noexcept(noexcept(*h))
	{
		return *h;
	}

	static pointer ptr(handle h) noexcept
	{
		return h;
	}
};

template<typename T>
struct default_traits<T[]>
{
	using element_type = T;
	using type = element_type[];
	using handle = element_type*;
	using pointer = element_type*;
	static constexpr nullable is_nullable = nullable::yes;
	static constexpr handle null = nullptr;

	template<typename Index>
	static decltype(auto) get_element(handle h, Index&& index)
	{
		return h[index];
	}

	static pointer ptr(handle h) noexcept
	{
		return h;
	}
};

template<typename T, nullable Nullable = nullable::no, typename NullType = void*, NullType Null = nullptr>
struct handle_is_type
{
	using type = T;
	using handle = type;
	using pointer = type*;
	static constexpr nullable is_nullable = Nullable;

	static type deref(handle h) noexcept(noexcept(handle(h)))
	{
		return h;
	}

	static pointer ptr(handle& h) noexcept
	{
		return &h;
	}

	static
	std::add_pointer_t<std::add_const_t<std::remove_pointer_t<pointer>>>
	ptr(handle const& h) noexcept
	{
		return &h;
	}

};

template<typename T, T Null>
struct handle_is_type<T, nullable::yes, T, Null> : handle_is_type<T,nullable::no>
{
	static constexpr nullable is_nullable = nullable::yes;
	static constexpr typename handle_is_type::handle null = Null;
};

template<typename T>
struct reference
{
	using type = T&;
	using handle = T&;
	using pointer = T*;

	static constexpr nullable is_nullable = nullable::no;

	static type deref(handle h) noexcept
	{
		return h;
	}

	static pointer ptr(handle h) noexcept
	{
		return &h;
	}
};

namespace detail
{

template<typename T, typename = std::void_t<>>
struct is_trait : std::false_type {};

template<typename T>
struct is_trait<T, std::void_t<typename T::type>> : std::true_type {};

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
