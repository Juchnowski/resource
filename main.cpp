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

//template<int N>
//using Policy = template typename PolicyImpl<N>::Inner;

namespace kq::resource::traits
{

template<typename Trait>
concept bool Nullable = Trait::is_nullable == nullable::yes;

} // kq::resource::traits

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
		copy::disable_copy_enable_move_with_nullification
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
























