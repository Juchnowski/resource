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

template<typename> struct DEBUG_TEMPLATE;

#include "resource/resource.hpp"
#include "resource/storage/default_storage.hpp"
#include "resource/cleanup/function_deleter.hpp"
#include "resource/copy/not_customized.hpp"
#include "resource/copy/disable_copy_enable_move.hpp"
#include "resource/copy/disable_copy_and_move.hpp"

void test_fd();
void test_malloc();
void test_new();
void test_mutex_locker();

auto main() -> int
{
//	test_fd();
//	test_new();
//	test_malloc();
//	test_mutex_locker();
}

//#if 0

void please_close(int fd)
{
	cout << "please_close(" << fd << ")" << endl;
	close(fd);
}

void test_fd()
{
	BARK;
	using namespace kq::resource;
	using deleter = cleanup::function_deleter<void(int), &please_close>;

	using R = resource<
		traits::handle_is_type<int, traits::nullable::yes, int, -1>,
		deleter::impl,
		storage::default_storage,
		copy::disable_copy_enable_move_with_nullification
	>;

	R r1{open("/dev/random", O_RDONLY)};
	R r2;

	static_assert(sizeof(R) == sizeof(int));

	DBG(*r1); // 3 or something
	DBG(*r2); // -1

//	r2 = r1; // error
	r2 = std::move(r1);

	DBG(*r1); // -1
	DBG(*r2); // previous *r1

	// *r2 destructor calls please_close
}

template<typename T>
void please_delete(T* ptr)
{
	cout << "please_delete: " << ptr << endl;
	delete ptr;
}

void test_new()
{
	BARK;
	using namespace kq::resource;
	using deleter = cleanup::function_deleter<void(int*), &please_delete>;
	using R = resource<int, deleter::impl, storage::default_storage, copy::disable_copy_enable_move_with_nullification>;

	R r1{new int(42)};
	R r2; // equal to R r2{nullptr}

	static_assert(sizeof(R) == sizeof(int*));

	DBG(r1.get());
	DBG(r2.get()); // nullptr

//	r2 = r1; // error
	r2 = move(r1);

	DBG(r1.get()); // nullptr
	DBG(r2.get());

	// please_delete called only once, for the correct pointer
}


void please_free(void *ptr)
{
	cout << "please_free: " << ptr << endl;
	free(ptr);
}
void test_malloc()
{
	BARK;
	using namespace kq::resource;
	using deleter = cleanup::function_deleter<void(void*), &please_free>;
	using R = resource<void, deleter::impl, storage::default_storage, copy::disable_copy_enable_move_with_nullification>;

	R r1{malloc(1024)};
	R r2; // equal to R r2{nullptr}

	static_assert(sizeof(R) == sizeof(void*));

	DBG(r1.get());
	DBG(r2.get()); // nullptr

//	r2 = r1; // error
	r2 = move(r1);

	DBG(r1.get()); // nullptr
	DBG(r2.get());

	// please_free called only once, for the correct pointer
}

template<typename T, typename Resource>
struct MutexLocker
{
	void initialize()
	{
		auto& full_type = static_cast<Resource&>(*this);
		full_type->lock();
	}

	void clean()
	{
		auto& full_type = static_cast<Resource&>(*this);
		full_type->unlock();
	}
};

#include <boost/noncopyable.hpp>

struct Mutex : boost::noncopyable
{
	void lock()
	{
		BARK;
		DBG(this);
	}

	void unlock()
	{
		BARK;
		DBG(this);
	}
};

using nullable = kq::resource::traits::nullable;

struct MutexTraits
{
	using type = Mutex&;
	using handle = Mutex&;
	using pointer = Mutex*;

	static constexpr nullable is_nullable = nullable::no;

	static type deref(handle h) noexcept
	{
		return h;
	}

	static pointer ptr(handle h) noexcept
	{
		return &h;
	}
};

void test_mutex_locker()
{
	BARK;

	using namespace kq::resource;

	static_assert(traits::detail::is_trait_v<MutexTraits>);

	using R = resource<
		MutexTraits,
		MutexLocker,
		storage::default_storage,
		copy::disable_copy_and_move
	>;

	static_assert(sizeof(R) == sizeof(Mutex*));

	Mutex m;

	R r{m}; // lock

//	auto r2 = r; // error
//	auto r2 = std::move(r); // error

	DBG(&m);

	// unlock
}


//#endif



