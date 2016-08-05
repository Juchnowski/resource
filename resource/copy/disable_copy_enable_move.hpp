#ifndef DISABLE_COPY_ENABLE_MOVE_HPP
#define DISABLE_COPY_ENABLE_MOVE_HPP

#include <boost/scope_exit.hpp>

#include "../traits.hpp"
#include "not_customized.hpp"

namespace kq::resource::copy
{


template<typename T, typename Resource>
struct disable_copy_enable_move : not_customized<T, Resource>
{
	template<typename U>
	static auto copy_cleanup(U const& c) = delete;
	template<typename U>
	static auto copy_storage(U const& s) = delete;

	disable_copy_enable_move() = default;

	disable_copy_enable_move(disable_copy_enable_move const&) = delete;
	disable_copy_enable_move(disable_copy_enable_move&&) = default;

	disable_copy_enable_move& operator=(disable_copy_enable_move const&) = delete;
	disable_copy_enable_move& operator=(disable_copy_enable_move&&) = default;
};


template<typename T, typename Resource>
struct disable_copy_enable_move_with_nullification : disable_copy_enable_move<T, Resource>
{
	using parent = disable_copy_enable_move<T, Resource>;
	using traits = typename parent::traits;

	static_assert(traits::is_nullable);

	template<typename U>
	static auto move_storage(U&& c) noexcept(noexcept(parent::move_storage(std::forward<U>(c)))) {
		BOOST_SCOPE_EXIT_TPL(&c){
			c.nullify();
		} BOOST_SCOPE_EXIT_END
		return parent::move_storage(std::forward<U>(c));
	}
};

} // kq::resource::copy

#endif // DISABLE_COPY_ENABLE_MOVE_HPP
