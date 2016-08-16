#ifndef AUTOMATIC_STORAGE_HPP
#define AUTOMATIC_STORAGE_HPP

#include <type_traits>

#include "../traits.hpp"

namespace kq::resource::storage
{

template<typename T, typename Resource>
struct automatic_storage
{
	using traits = traits::get_traits<T>;
	using type = typename traits::type;
	using handle = typename traits::handle;

	automatic_storage(handle h) : data_(h) {}

	handle get() const noexcept {
		return data_;
	}

	decltype(auto) operator*() const {
		return traits::deref(data_);
	}

	decltype(auto) operator->() const {
		return traits::ptr(data_);
	}

	template<typename Index>
	decltype(auto) operator[](Index&& index) const {
		return traits::get_element(data_, std::forward<Index>(index));
	}

	bool is_valid() const noexcept {
		return typename detail::is_valid{}(traits{}, data_);
	}

	template<typename Traits = traits>
	auto nullify() -> std::enable_if_t<Traits::is_nullable, void>
	{
		data_ = traits::null;
	}

	template<typename Traits = traits>
	auto release() -> std::enable_if_t<Traits::is_nullable, handle>
	{
		auto tmp = data_;
		nullify();
		return tmp;
	}

private:
	handle data_;

	struct detail
	{
		struct is_valid
		{
			template<typename U, typename V, typename = std::enable_if_t<traits::is_nullable,void>>
			bool operator()(U&&, V&& handle) const noexcept {
				return handle != traits::null;
			}
			bool operator()(...) const noexcept {
				return true;
			}
		};
	};
};

} // kq::resource::storage

#endif // AUTOMATIC_STORAGE_HPP
