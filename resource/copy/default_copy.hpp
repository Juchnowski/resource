#ifndef DEFAULT_COPY_HPP
#define DEFAULT_COPY_HPP

#include "disable_copy_and_move.hpp"

namespace kq::resource::copy
{

namespace detail
{

//using nullable = typename traits::nullable;
typedef typename traits::nullable nullable;

template<bool TypeAndHandleSame, bool HandleAndPointerSame, nullable IsNullable>
struct default_select;

template<typename T>
struct get_default_copy
{
	using traits = traits::get_traits<T>;
	static constexpr bool type_and_handle_are_the_same = std::is_same<typename traits::type, typename traits::handle>::value;
	static constexpr bool handle_and_pointer_are_the_same = std::is_same<typename traits::handle, typename traits::pointer>::value;
	static constexpr nullable is_nullable = traits::is_nullable;

	using type = default_select<type_and_handle_are_the_same, handle_and_pointer_are_the_same, is_nullable>;
};

template<bool TypeAndHandleSame, bool HandleAndPointerSame, nullable IsNullable>
struct default_select
{
	template<typename T, typename Resource>
	using impl = disable_copy_and_move<T,Resource>;
};

}

template<typename T>
using default_copy = detail::get_default_copy<T>;

} // kq::resource::copy

#endif // DEFAULT_COPY_HPP
