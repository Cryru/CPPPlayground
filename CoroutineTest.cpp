#include "CoroutineTest.h"
#include <coroutine>
#include <iostream>

const CoroutineWaiter CoroutineWaiter::WaitOneLoop = CoroutineWaiter();

static Coroutine loopRoutine()
{
	for (size_t i = 0; i < 10; i++)
	{
		co_yield CoroutineWaiter::WaitOneLoop;
		std::cout << i << std::endl;
	}
}

static CoroutineBase<CoroutineWaiter> nestedRoutine()
{
	co_yield loopRoutine();
	std::cout << "ez" << std::endl;
}

static CoroutineBase<CoroutineWaiter> nestedRoutineWait()
{
	co_yield CoroutineWaiter::WaitOneLoop;
	co_yield CoroutineWaiter::WaitOneLoop;
	co_yield loopRoutine();
}

static CoroutineManager Manager{};
void TestCoroutine(void)
{
	Manager.AddRoutine(nestedRoutine());
	Manager.AddRoutine(nestedRoutine());
	Manager.AddRoutine(nestedRoutineWait());
	Manager.AddRoutine(loopRoutine());
	while (true)
	{
		Manager.Update();
	}
}