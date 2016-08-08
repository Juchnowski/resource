#include "includes.hpp"

#include "resource/resource.hpp"
#include "resource/cleanup/function_deleter.hpp"

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
	using Res = resource<void, deleter::impl>;

	static_assert(
		std::is_same<
			Res,
			resource<
				void,
				deleter::impl,
				storage::automatic_storage,
				copy::disable_copy_enable_move_with_nullification
			>
		>::value
	);

	Res r1{malloc(1024)};
	Res r2; // equal to R r2{nullptr}

	static_assert(sizeof(Res) == sizeof(void*));

	DBG(r1.get());
	DBG(r2.get()); // nullptr

//	r2 = r1; // error
	r2 = move(r1);

	DBG(r1.get()); // nullptr
	DBG(r2.get());

	// please_free called only once, for the correct pointer
}
