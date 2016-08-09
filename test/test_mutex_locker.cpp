#include "includes.hpp"

#include <boost/noncopyable.hpp>

#include "resource/resource.hpp"

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


struct Mutex : boost::noncopyable
{
	void lock()
	{
		BARK;
		DBG(this);
	}

	void foo() const noexcept
	{
		BARK;
	}

	void unlock()
	{
		BARK;
		DBG(this);
	}
};

using nullable = kq::resource::traits::nullable;

void test_mutex_locker()
{
	BARK;

	using namespace kq::resource;

	static_assert(traits::detail::is_trait_v<traits::reference<Mutex>>);

	using Res = resource<traits::reference<Mutex>,MutexLocker>;

	static_assert(
		std::is_same<
			  Res,
			  resource<
				traits::reference<Mutex>,
				MutexLocker,
				storage::automatic_storage,
				copy::disable_copy_and_move
			>
		  >::value
	);

	static_assert(sizeof(Res) == sizeof(Mutex*));

	Mutex m;

	Res r{m}; // lock

	r->foo();

//	auto r2 = r; // error
//	auto r2 = std::move(r); // error

	DBG(&m);

	// unlock
}

