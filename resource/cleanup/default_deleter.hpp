#ifndef DEFAULT_DELETER_HPP
#define DEFAULT_DELETER_HPP

#include <type_traits>

#include "../traits.hpp"

#include "delete_deleter.hpp"

namespace kq::resource::cleanup
{

namespace detail
{

//using nullable = typename traits::nullable;
typedef typename traits::nullable nullable;

template<bool TypeAndHandleSame, bool HandleAndPointerSame, nullable IsNullable>
struct default_select;

template<typename T>
struct get_default_deleter
{
	using traits = traits::get_traits<T>;
	static constexpr bool type_and_handle_are_the_same = std::is_same<typename traits::type, typename traits::handle>::value;
	static constexpr bool handle_and_pointer_are_the_same = std::is_same<typename traits::handle, typename traits::pointer>::value;
	static constexpr nullable is_nullable = traits::is_nullable;

	using type = default_select<type_and_handle_are_the_same, handle_and_pointer_are_the_same, is_nullable>;
};

template<>
struct default_select<false, true, nullable::yes>
{
	template<typename T, typename Resource>
	using impl = delete_deleter<T,Resource>;
};

} // detail

template<typename T>
using default_deleter = detail::get_default_deleter<T>;

} // kq::resource::cleanup

#endif // DEFAULT_DELETER_HPP
