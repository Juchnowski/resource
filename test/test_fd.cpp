#include "includes.hpp"

#include <fcntl.h>
#include <unistd.h>

#include "../resource/resource.hpp"
#include "../resource/cleanup/function_deleter.hpp"

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

	using Res = resource<
		traits::handle_is_type<int, traits::nullable::yes, int, -1>,
		deleter::impl
	>;

	static_assert(
		std::is_same<
			Res,
			resource<
				traits::handle_is_type<int, traits::nullable::yes, int, -1>,
				deleter::impl,
				storage::automatic_storage,
				copy::disable_copy_enable_move_with_nullification
			>
		>::value
	);

	Res r1{open("/dev/random", O_RDONLY)};
	Res r2;

	static_assert(sizeof(Res) == sizeof(int));

	DBG(*r1); // 3 or something
	DBG(*r2); // -1

//	r2 = r1; // error
	r2 = std::move(r1);

	DBG(*r1); // -1
	DBG(*r2); // previous *r1

	// *r2 destructor calls please_close
}

