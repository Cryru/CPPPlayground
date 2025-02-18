#pragma once

#include <coroutine>
#include <iostream>
#include <vector>

void TestCoroutine(void);

// resources
// https://en.cppreference.com/w/cpp/language/coroutines
template<typename T>
class CoroutineBase : std::coroutine_traits<T> {
public:
	struct promise_type; // The object used to manipulate the routine from the inside. (contains get_return_object and the other funcs)
	using handle_type = std::coroutine_handle<promise_type>; // The handle used to manipulate the coroutine from the outside.

	CoroutineBase(handle_type h) : _routineHandle(h) {
		std::cout << "coroutine base created" << std::endl;

		// eagerly run when created
		if (_routineHandle)
			_routineHandle.resume();
	}

	virtual ~CoroutineBase() {
		std::cout << "coroutine base destroyed" << std::endl;

		if (_routineHandle)
			_routineHandle.destroy();
	}

	// Copying routines is kinda weird, better not deal with it rn
	CoroutineBase(const CoroutineBase&) = delete;
	CoroutineBase& operator=(const CoroutineBase&) = delete;

	// Move (not default since that wont invalidate the handle in the old one)
	CoroutineBase(CoroutineBase&& other) noexcept : _routineHandle(other._routineHandle) {
		std::cout << "moved coroutine" << std::endl;
		other._routineHandle = NULL;
	}

	CoroutineBase& operator=(CoroutineBase&& other) noexcept {
		if (this != &other) {
			if (_routineHandle) _routineHandle.destroy();
			_routineHandle = other._routineHandle;
			other._routineHandle = nullptr;
		}
		return *this;
	}

	bool MoveNext() {
		if (!_routineHandle.done())
		{
			_routineHandle.resume();
			return true;
		}
		return false;
	}

	T& GetCurrentValue() {
		return _routineHandle.promise().current_value;
	}

	bool IsRunning() const {
		if (!_routineHandle) return false;
		if (_routineHandle.done()) return false;
		return true;
	}

	struct promise_type {
		T current_value;

		auto get_return_object() { // when the coroutine is started this will be called to get the coroutine object for outside manipulation
			return CoroutineBase{ handle_type::from_promise(*this) };
		}

		auto initial_suspend() {
			return std::suspend_always{};
		}

		auto final_suspend() noexcept {
			return std::suspend_always{};
		}

		auto yield_value(T value) { // co_yield
			current_value = value;
			return std::suspend_always{}; // yielding suspends
		}

		void return_void() {} // co_return

		void unhandled_exception() {} // todo :P
	};

private:
	handle_type _routineHandle;
};

class CoroutineWaiter;
typedef CoroutineBase<CoroutineWaiter> Coroutine;

class CoroutineWaiter
{
public:
	static const CoroutineWaiter WaitOneLoop;

	// Reference to a subroutine.
	Coroutine* Subroutine = NULL;

	CoroutineWaiter()
	{
	}

	~CoroutineWaiter()
	{
	}

	// Copyable
	CoroutineWaiter(const CoroutineWaiter&) = default;
	CoroutineWaiter& operator=(const CoroutineWaiter&) = default;
	// Movable
	CoroutineWaiter(CoroutineWaiter&&) noexcept = default;
	CoroutineWaiter& operator=(CoroutineWaiter&&) noexcept = default;

	CoroutineWaiter(Coroutine&& routine) {
		Subroutine = &routine;
	}
};

class CoroutineManager
{
public:
	std::vector<Coroutine> RunningRoutines;

	void AddRoutine(Coroutine&& routine)
	{
		std::cout << "started" << std::endl;
		RunningRoutines.push_back(std::move(routine));
	}

	void Update()
	{
		bool anyRan = false;

		RunningRoutines.erase(
			std::remove_if(
				RunningRoutines.begin(),
				RunningRoutines.end(),
				[](const Coroutine& routine) {
					return !routine.IsRunning();
				}
			),
			RunningRoutines.end()
		);

		for (Coroutine& routine : RunningRoutines)
		{
			if (routine.IsRunning())
			{
				RunRoutine(routine);
				anyRan = true;
			}
		}
		if (anyRan) std::cout << "tick complete" << std::endl;
	}

private:
	void RunRoutine(Coroutine& routine)
	{
		CoroutineWaiter& waiter = routine.GetCurrentValue();
		Coroutine* subroutine = waiter.Subroutine;
		if (subroutine && subroutine->IsRunning())
		{
			RunRoutine(*subroutine);
			return;
		}

		routine.MoveNext();
	}
};