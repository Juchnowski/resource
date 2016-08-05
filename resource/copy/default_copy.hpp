#ifndef DEFAULT_COPY_HPP
#define DEFAULT_COPY_HPP

#include "disable_copy_and_move.hpp"

namespace kq::resource::copy
{

template<typename T, typename Resource>
using default_copy = disable_copy_and_move<T,Resource>;

} // kq::resource::copy

#endif // DEFAULT_COPY_HPP
