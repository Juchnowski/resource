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

//#include <fcntl.h>
//#include <unistd.h>

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

#include "resource/resource.hpp"

//template<int N>
//using Policy = template typename PolicyImpl<N>::Inner;

auto main() -> int
{
	using namespace kq::resource;
	using R = resource<int, PolicyImpl<0>::Inner, PolicyImpl<1>::Inner, PolicyImpl<2>::Inner>;
	R r{42};
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
























