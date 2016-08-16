#include "includes.hpp"

#include "resource/resource.hpp"
#include "resource/cleanup/function_deleter.hpp"

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
	using Res = resource<int, deleter::impl>;

	static_assert(std::is_same<resource<int>,resource<int, cleanup::delete_deleter, storage::automatic_storage, copy::disable_copy_enable_move_with_nullification>>::value);
	static_assert(std::is_same<Res,resource<int, deleter::impl, storage::automatic_storage, copy::disable_copy_enable_move_with_nullification>>::value);

	Res r1{new int(42)};
	Res r2; // equal to R r2{nullptr}

	static_assert(sizeof(Res) == sizeof(int*));

	DBG(r1.get());
	DBG(r2.get()); // nullptr

//	r2 = r1; // error
	r2 = move(r1);

	DBG(r2.release());

	DBG(r1.get()); // nullptr
	DBG(r2.get());

	// please_delete called only once, for the correct pointer
}

