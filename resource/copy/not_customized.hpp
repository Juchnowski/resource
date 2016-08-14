#ifndef NOT_CUSTOMIZED_HPP
#define NOT_CUSTOMIZED_HPP

#include "../traits.hpp"

namespace kq::resource::copy
{

template<typename T, typename Resource>
struct not_customized
{
	using traits = traits::get_traits<T>;
	using type = typename traits::type;

	template<typename Cleanup>
	static auto
	copy_cleanup(Cleanup const& c) noexcept(noexcept(decltype(c)(c))) {
		static_assert(std::is_same<Cleanup, typename Resource::cleanup_p>::value);
		return c;
	}
	template<typename Storage>
	static auto
	copy_storage(Storage const& s) noexcept(noexcept(decltype(s)(s))) {
		static_assert(std::is_same<Storage, typename Resource::storage_p>::value);
		return s;
	}

	template<typename Cleanup>
	static auto
	move_cleanup(Cleanup&& c) noexcept(noexcept(decltype(c)(std::move(c)))) {
		static_assert(std::is_same<Cleanup, typename Resource::cleanup_p>::value);
		return c;
	}
	template<typename Storage>
	static auto
	move_storage(Storage&& s) noexcept(noexcept(decltype(s)(std::move(s)))) {
		static_assert(std::is_same<Storage, typename Resource::storage_p>::value);
		return std::move(s);
	}

	static void swap(Resource& l, Resource& r) noexcept(false) {
		using std::swap;

		using cleanup_p = typename Resource::cleanup_p;
		using storage_p = typename Resource::storage_p;
		using copy_p = typename Resource::copy_p;
		// Resource::copy may be a class derived from not_customized

		swap(static_cast<storage_p&>(l),static_cast<storage_p&>(r));
		swap(static_cast<cleanup_p&>(l),static_cast<cleanup_p&>(r));
		swap(static_cast<copy_p&>(l),static_cast<copy_p&>(r));
	}
};

} // kq::resource::copy

#endif // NOT_CUSTOMIZED_HPP
