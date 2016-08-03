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


template<typename> struct DEBUG_TEMPLATE;

#include "resource/resource.hpp"

//template<int N>
//using Policy = template typename PolicyImpl<N>::Inner;

namespace kq::resource::storage
{

template<typename T, typename Resource>
struct automatic_storage
{
	using traits = traits::get_traits<T>;
	using type = typename traits::type;
	using handle = typename traits::handle;

	automatic_storage(handle h) : data_(h) {}

	handle get() const noexcept {
		return data_;
	}

	decltype(auto) operator*() const {
		return traits::deref(data_);
	}

	struct is_valid_detail
	{
		template<typename Handle>
		std::enable_if_t<traits::is_nullable, bool> operator()(Handle&& h) const noexcept {
			return h == traits::null;
		}
		bool operator()(...) const noexcept {
			return true;
		}
	};

	bool is_valid() const noexcept {
		return is_valid_detail{}(data_);
	}

	template<typename ImplDetail = std::enable_if_t<traits::is_nullable, void>>
	void nullify(ImplDetail* = nullptr) noexcept {
		data_ = traits::null;
	}

//	aligned_storage

private:
	handle data_;
};


template<typename T, typename Resource>
using default_storage = automatic_storage<T,Resource>;

} // kq::resource::storage

namespace kq::resource::cleanup
{

template<typename T, typename Resource>
struct default_deleter
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

template<typename Func, Func* Function, bool RequireNullable = false>
struct function_deleter
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
		auto operator()(StoragePolicy* s) const noexcept(noexcept(s->nullify())) -> decltype(s->nullify()){
			return s->nullify();
		}
	};

	template<typename T, typename Resource>
	struct impl
	{
		void clean(){
			using storage = typename Resource::storage;
			auto& full_type = static_cast<Resource&>(*this);
			Function(full_type.storage::get());

			using nullifier = std::conditional_t<RequireNullable, ensure_nullify, maybe_nullify>;

			nullifier{}(static_cast<storage*>(&full_type));
		}
	};
};

} // kq::resource::cleanup

namespace kq::resource::copy
{

template<typename T, typename Resource>
struct default_copy
{
	using traits = traits::get_traits<T>;
	using type = typename traits::type;

//	void operator()(typename traits::handle ptr){
//		using storage = typename Resource::storage;
//	}
};

} // kq::resource::copy

template<typename T>
void please_delete(T* ptr)
{
	DBG(ptr);
	delete ptr;
}

auto main() -> int
{
	using namespace kq::resource;
	using deleter = cleanup::function_deleter<void(int*), &please_delete>;
	using R = resource<int, deleter::impl, storage::default_storage, PolicyImpl<2>::Inner>;
	R r{new int(42)};



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
























