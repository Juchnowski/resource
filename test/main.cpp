#include "includes.hpp"

#include "../resource/resource.hpp"
#include "../resource/storage/default_storage.hpp"
#include "../resource/cleanup/function_deleter.hpp"
#include "../resource/copy/not_customized.hpp"
#include "../resource/copy/disable_copy_enable_move.hpp"
#include "../resource/copy/disable_copy_and_move.hpp"

void test_fd();
void test_malloc();
void test_new();
void test_mutex_locker();

auto main() -> int
{
	test_fd();
	test_new();
	test_malloc();
	test_mutex_locker();
}

//#if 0



//#endif



