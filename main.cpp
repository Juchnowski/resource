#include <cassert>
#include <climits>
#include <csetjmp>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <deque>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <random>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <thread>
#include <type_traits>
#include <typeinfo>
#include <tuple>
#include <unordered_set>

using namespace std;

constexpr int Width = 60;
#define DBG(x) { cout << left << setw(Width) << #x << boolalpha << (x) << endl; }
#define DBG_CONT(x) { cout << left << setw(Width) << #x; for(auto const& _name : (x)) \
	cout << boolalpha << _name << " "; cout << endl; }

#define BARK cout << __PRETTY_FUNCTION__ << endl;

#include <fcntl.h>
#include <unistd.h>

template<typename Func, Func* Function>
struct FunctionDeleter
{
	template<typename T>
	auto operator()(T&& param) const noexcept(noexcept(Function))
	{
		return Function(std::forward<T>(param));
	}
};

#define MAKE_FUNCTION_DELETER(f) \
	FunctionDeleter<decltype(f), f>

void del(void* ptr){
	DBG(ptr);
	free(ptr);
}

#include <boost/scope_exit.hpp>

template<typename> struct DEBUG_TEMPLATE;

#include "resource/resource.hpp"
#include "resource/storage/default_storage.hpp"
#include "resource/cleanup/function_deleter.hpp"

//template<int N>
//using Policy = template typename PolicyImpl<N>::Inner;

namespace kq::resource::traits
{

template<typename Trait>
concept bool Nullable = Trait::is_nullable == nullable::yes;

} // kq::resource::traits

namespace kq::resource::copy
{

template<typename T, typename Resource>
struct default_copy
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

template<typename T, typename Resource>
struct no_copy_yes_move : default_copy<T, Resource>
{
	template<typename U>
	static auto copy_cleanup(U const& c) = delete;
	template<typename U>
	static auto copy_storage(U const& s) = delete;

	no_copy_yes_move() = default;

	no_copy_yes_move(no_copy_yes_move const&) = delete;
	no_copy_yes_move(no_copy_yes_move&&) = default;

	no_copy_yes_move& operator=(no_copy_yes_move const&) = delete;
	no_copy_yes_move& operator=(no_copy_yes_move&&) = default;
};


template<typename T, typename Resource>
struct no_copy_yes_move_and_nullify : no_copy_yes_move<T, Resource>
{
	using parent = default_copy<T, Resource>;
	using traits = typename parent::traits;

	static_assert(traits::is_nullable);

	template<typename U>
	static auto move_storage(U&& c) noexcept(noexcept(parent::move_storage(std::forward<U>(c)))) {
		BOOST_SCOPE_EXIT_TPL(&c){
			c.nullify();
		} BOOST_SCOPE_EXIT_END
		return parent::move_storage(std::forward<U>(c));
	}
};

} // kq::resource::copy

template<typename T>
void please_delete(T* ptr)
{
	cout << "please_delete: " << ptr << endl;
	delete ptr;
}

void please_close(int fd)
{
	cout << "please_close(" << fd << ")" << endl;
	close(fd);
}

auto main() -> int
{
	using namespace kq::resource;
	using deleter = cleanup::function_deleter<void(int), &please_close>;

	using R = resource<
		traits::handle_is_type<int, traits::nullable::yes, int, -1>,
		deleter::impl,
		storage::default_storage,
		copy::no_copy_yes_move_and_nullify
	>;

	R r1{open("/dev/random", O_RDONLY)};
	R r2;

	DBG(*r1);
	DBG(*r2);

//	r2 = r1; // error
	r2 = std::move(r1);

	DBG(*r1);
	DBG(*r2);


//	using deleter = cleanup::function_deleter<void(int*), &please_delete>;
//	using R = resource<int, deleter::impl, storage::default_storage, copy::no_copy_yes_move_and_nullify>;
//	R r1{new int(42)};

//	R r2{nullptr};

//	DBG(sizeof(r1));
//	DBG(sizeof(void*));

//	DBG(r1.get());
//	DBG(r2.get());

//	r2 = move(r1);

//	DBG(r1.get());
//	DBG(r2.get());
}

//auto main() -> int
//{
////	auto n = open("/dev/random", O_RDONLY);

////	close(n);

//	using ptr_t = unique_ptr<void, decltype(&free)>;
////	using ptr_del_t = unique_ptr<void, FunctionDeleter<void(void*),&::del>>;
//	using ptr_del_t = unique_ptr<void, MAKE_FUNCTION_DELETER(del)>;

//	ptr_t ptr{malloc(128), &free};
//	ptr_del_t ptr_del{malloc(128)};

//	DBG(ptr_del.get());

//	DBG(sizeof(ptr_t));
//	DBG(sizeof(ptr_del_t));
//	DBG(sizeof(void*));
//}
























