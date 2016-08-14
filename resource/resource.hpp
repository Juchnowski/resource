#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include <type_traits>

#include "traits.hpp"

#include "cleanup/default_deleter.hpp"
#include "copy/default_copy.hpp"
#include "storage/default_storage.hpp"

namespace kq::resource
{

template<
	typename,
	template <typename,typename> typename,
	template <typename,typename> typename,
	template <typename,typename> typename,
	typename
>
struct resource;

namespace detail
{
template<
	typename,
	template <typename,typename> typename,
	template <typename,typename> typename,
	template <typename,typename> typename
>
struct resource_helper;
}

template<
	typename Type,
	template <typename,typename> typename CleanupPolicy = cleanup::default_deleter<Type>::type::template impl,
	template <typename,typename> typename StoragePolicy = storage::default_storage<Type>::type::template impl,
	template <typename,typename> typename CopyPolicy = copy::default_copy<Type>::type::template impl,
	typename Helper = detail::resource_helper<Type, CleanupPolicy, StoragePolicy, CopyPolicy>
>
class resource:
		Helper::Storage,
		Helper::Cleanup,
		Helper::Copy
{
public:
	using cleanup_p = typename Helper::Cleanup;
	using copy_p = typename Helper::Copy;
	using storage_p = typename Helper::Storage;
	friend cleanup_p;
	friend copy_p;
	friend storage_p;

	using traits = traits::get_traits<Type>;
	using type = typename traits::type;

	template<typename Traits = traits, typename = std::enable_if_t<Traits::is_nullable,void>>
	explicit resource() : storage_p{traits::null} {}

	template<
		typename First,
		typename... Params,
		typename BareFirst = std::remove_cv_t<std::remove_reference_t<First>>,
		typename = std::enable_if_t<
				!std::is_base_of<std::true_type,BareFirst>::value &&
				!std::is_base_of<std::false_type,BareFirst>::value,
			void
		>
	>
	explicit resource(First&& first, Params&&... params):
		resource(
			std::is_same<std::remove_cv_t<std::remove_reference_t<First>>, resource>{},
			std::forward<First>(first),
			std::forward<Params>(params)...
		)
	{
	}

	// the template above doesn't cover this, because copy constructor gets
	// implicitly explicitly deleted (implicitly declared by the compiler as
	// deleted)
	explicit resource(resource const& other):
		resource(std::true_type{}, other)
	{
	}

	resource& operator=(resource const& other) & noexcept(false)
	{
		static_cast<storage_p&>(*this) = copy_p::copy_storage(static_cast<storage_p const&>(other));
		static_cast<cleanup_p&>(*this) = copy_p::copy_cleanup(static_cast<cleanup_p const&>(other));
		static_cast<copy_p&>(*this) = static_cast<copy_p const&>(other);
		return *this;
	}

	resource& operator=(resource&& other) & noexcept(false)
	{
		static_cast<storage_p&>(*this) = copy_p::move_storage(static_cast<storage_p&&>(other));
		static_cast<cleanup_p&>(*this) = copy_p::move_cleanup(static_cast<cleanup_p&&>(other));
		static_cast<copy_p&>(*this) = static_cast<copy_p&&>(other);
		return *this;
	}

	~resource() noexcept(noexcept(resource::call_clean(std::declval<resource>())))
	{
		call_clean(this);
	}

	using storage_p::operator*;
	using storage_p::get;
	using storage_p::operator->;
	using storage_p::is_valid;
	using storage_p::nullify;

private:

	template<typename T, typename Cleanup = cleanup_p, typename Copy = copy_p>
	resource(std::false_type, T&& t, Cleanup&& cl = Cleanup{}, Copy&& cp = Copy{}):
		storage_p{std::forward<T>(t)},
		cleanup_p{std::forward<Cleanup>(cl)},
		copy_p{std::forward<Copy>(cp)}
	{
		call_initialize(this);
	}

	resource(std::true_type, resource const& other):
		storage_p(copy_p::copy_storage(static_cast<storage_p const&>(other))),
		cleanup_p(copy_p::copy_cleanup(static_cast<cleanup_p const&>(other))),
		copy_p(other)
	{
	}

	resource(std::true_type, resource&& other):
		storage_p(copy_p::move_storage(static_cast<storage_p&&>(other))),
		cleanup_p(copy_p::move_cleanup(static_cast<cleanup_p&&>(other))),
		copy_p(static_cast<copy_p&&>(other))
	{
	}

	template<typename T>
	static auto call_initialize(T* t = nullptr) -> decltype(t->cleanup_p::initialize())
	{
		return t->cleanup_p::initialize();
	}

	static void call_initialize(...) noexcept {}

	template<typename T>
	static auto call_clean(T* t = nullptr) -> decltype(t->cleanup_p::cleanup())
	{
		return t->cleanup_p::cleanup();
	}

	static void call_clean(...) noexcept {}

};

template<
	typename T,
	template <typename,typename> typename Cleanup,
	template <typename,typename> typename Storage,
	template <typename,typename> typename Copy
>
void swap(resource<T,Cleanup, Storage, Copy>& l, resource<T,Cleanup, Storage, Copy>& r){
	resource<T,Cleanup, Storage, Copy>::copy::swap(l, r);
}

namespace detail
{
template<
	typename Type,
	template <typename,typename> typename CleanupPolicy,
	template <typename,typename> typename StoragePolicy,
	template <typename,typename> typename CopyPolicy
>
struct resource_helper {
	using This = resource_helper<Type, CleanupPolicy, StoragePolicy, CopyPolicy>;

	using Cleanup = CleanupPolicy<Type,resource<Type,CleanupPolicy,StoragePolicy,CopyPolicy, This>>;
	using Copy = CopyPolicy<Type,resource<Type,CleanupPolicy,StoragePolicy,CopyPolicy, This>>;
	using Storage = StoragePolicy<Type,resource<Type,CleanupPolicy,StoragePolicy,CopyPolicy, This>>;
};
}

} // namespace kq::resource

#endif // RESOURCE_HPP
