#ifndef DEFAULT_STORAGE_HPP
#define DEFAULT_STORAGE_HPP

#include "automatic_storage.hpp"

namespace kq::resource::storage
{

template<typename T, typename Resource>
using default_storage = automatic_storage<T,Resource>;

} // kq::resource::storage

#endif // DEFAULT_STORAGE_HPP
