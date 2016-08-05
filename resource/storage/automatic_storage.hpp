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

	bool is_valid() const noexcept {
		return typename detail::is_valid{}(traits{}, data_);
	}

	template<typename ImplDetail = std::enable_if_t<traits::is_nullable, void>>
	void nullify(ImplDetail* = nullptr) noexcept {
		data_ = traits::null;
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
