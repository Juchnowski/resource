#ifndef DELETE_DELETER_HPP
#define DELETE_DELETER_HPP

#include "../traits.hpp"

namespace kq::resource::cleanup
{

namespace detail
{
template<bool IsArray = false>
struct delete_caller
{
	template<typename T>
	static void call(T* ptr){
		delete ptr;
	}
};

template<>
struct delete_caller<true>
{
	template<typename T>
	static void call(T* ptr){
		delete[] ptr;
	}
};
}

template<typename T, typename Resource>
struct delete_deleter
{
	using traits = traits::get_traits<T>;
	using type = typename traits::type;

	void clean(){
		using storage = typename Resource::storage;
		auto& full_type = static_cast<Resource&>(*this);
		detail::delete_caller<std::is_array<T>::value>::call(full_type.storage::get());
		full_type.storage::nullify();
	}
};



} // kq::resource::cleanup

#endif // DELETE_DELETER_HPP
