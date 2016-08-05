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

//	using cleanup = typename Resource::cleanup;
//	using storage = typename Resource::storage;

	template<typename Cleanup>
	static auto
	copy_cleanup(Cleanup const& c) noexcept(noexcept(decltype(c)(c))) {
		static_assert(std::is_same<Cleanup, typename Resource::cleanup>::value);
		return c;
	}
	template<typename Storage>
	static auto
	copy_storage(Storage const& s) noexcept(noexcept(decltype(s)(s))) {
		static_assert(std::is_same<Storage, typename Resource::storage>::value);
		return s;
	}

	template<typename Cleanup>
	static auto
	move_cleanup(Cleanup&& c) noexcept(noexcept(decltype(c)(std::move(c)))) {
		static_assert(std::is_same<Cleanup, typename Resource::cleanup>::value);
		return c;
	}
	template<typename Storage>
	static auto
	move_storage(Storage&& s) noexcept(noexcept(decltype(s)(std::move(s)))) {
		static_assert(std::is_same<Storage, typename Resource::storage>::value);
		return std::move(s);
	}

	static void swap(Resource& l, Resource& r) noexcept(false) {
		using std::swap;

		using cleanup = typename Resource::cleanup;
		using storage = typename Resource::storage;
		using copy = typename Resource::copy;
		// Resource::copy may be a class derived from default_copy

		swap(static_cast<storage&>(l),static_cast<storage&>(r));
		swap(static_cast<cleanup&>(l),static_cast<cleanup&>(r));
		swap(static_cast<copy&>(l),static_cast<copy&>(r));
	}
};

} // kq::resource::copy

#endif // NOT_CUSTOMIZED_HPP
