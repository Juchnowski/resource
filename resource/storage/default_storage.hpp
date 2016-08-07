#ifndef DEFAULT_STORAGE_HPP
#define DEFAULT_STORAGE_HPP

#include "../traits.hpp"

#include "automatic_storage.hpp"

namespace kq::resource::storage
{

namespace detail
{

//using nullable = typename traits::nullable;
typedef typename traits::nullable nullable;

template<bool TypeAndHandleSame, bool HandleAndPointerSame, nullable IsNullable>
struct default_select;

template<typename T>
struct get_default_storage
{
	using traits = traits::get_traits<T>;
	static constexpr bool type_and_handle_are_the_same = std::is_same<typename traits::type, typename traits::handle>::value;
	static constexpr bool handle_and_pointer_are_the_same = std::is_same<typename traits::handle, typename traits::pointer>::value;
	static constexpr nullable is_nullable = traits::is_nullable;

	using type = default_select<type_and_handle_are_the_same, handle_and_pointer_are_the_same, is_nullable>;
};

}

template<typename T, typename Resource>
using default_storage = automatic_storage<T,Resource>;

} // kq::resource::storage

#endif // DEFAULT_STORAGE_HPP
