#ifndef FUNCTION_DELETER_HPP
#define FUNCTION_DELETER_HPP

#include "../traits.hpp"

namespace kq::resource::cleanup
{

template<typename Func, Func* Function, bool RequireNullable = false>
struct function_deleter
{
	struct detail
	{
		struct maybe_nullify{
			template<typename StoragePolicy>
			auto operator()(StoragePolicy* s) const noexcept(noexcept(s->nullify())) -> decltype(s->nullify()){
				return s->nullify();
			}
			void operator()(...) const noexcept {}
		};

		struct ensure_nullify{
			template<typename StoragePolicy>
			auto operator()(StoragePolicy* s) const noexcept(noexcept(s->nullify())) {
				return s->nullify();
			}
		};
	};


	template<typename T, typename Resource>
	struct impl
	{
		using traits = traits::get_traits<T>;

		void cleanup() noexcept(noexcept(Function(std::declval<typename traits::handle>()))){
			using storage_p = typename Resource::storage_p;
			auto& full_type = static_cast<Resource&>(*this);
			if(full_type.storage_p::is_valid()){
				Function(full_type.storage_p::get());
			}

			using nullifier = std::conditional_t<
				RequireNullable,
				typename detail::ensure_nullify,
				typename detail::maybe_nullify
			>;

			nullifier{}(static_cast<storage_p*>(&full_type));
		}
	};
};

} // kq::resource::cleanup

#endif // FUNCTION_DELETER_HPP
