#ifndef DELETE_DELETER_HPP
#define DELETE_DELETER_HPP

#include "../traits.hpp"

namespace kq::resource::cleanup
{

template<typename T, typename Resource>
struct delete_deleter
{
	using traits = traits::get_traits<T>;
	using type = typename traits::type;

	void clean(){
		using storage = typename Resource::storage;
		auto& full_type = static_cast<Resource&>(*this);
		delete full_type.storage::get();
		full_type.storage::nullify();
	}
};



} // kq::resource::cleanup

#endif // DELETE_DELETER_HPP
