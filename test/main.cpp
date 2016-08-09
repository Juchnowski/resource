#include "includes.hpp"

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

