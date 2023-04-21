#include <coroutine>
#include <iostream>
#include <utility>

using namespace std::string_literals;

struct Chat {
	struct promise_type {
		std::string _msgOut{}, _msgIn{}; // Storing a value from or for the coroutine

		void unhandled_exception() noexcept {} // What to do in case of an exception
		Chat get_return_object() { return Chat(this); } // Coroutine creation
		std::suspend_always initial_suspend() noexcept { return {}; } // Startup
		std::suspend_always yield_value(std::string msg) noexcept // Value from co_yield
		{
			_msgOut = std::move(msg);
			return {};
		}

		auto await_transform(std::string) noexcept // Value from co_await
		{
			struct awaiter { // Customized version instead of using suspend_always or suspend_never
				promise_type& pt;
				constexpr bool await_ready() const noexcept { return true; }
				std::string await_resume() const noexcept { return std::move(pt._msgIn); }
				void await_suspend(std::coroutine_handle<>) const noexcept {}
			};

			return awaiter{*this};
		}

		void return_value(std::string msg) noexcept { _msgOut = std::move(msg); } // Value from co_return
		std::suspend_always final_suspend() noexcept { return {}; } // Ending
	};

	using Handle = std::coroutine_handle<promise_type>; // Shortcut for the handle type
	Handle mCoroHdl{};

	explicit Chat(promise_type* p) : mCoroHdl{Handle::from_promise(*p)} {} // Get the handle from the promise
	Chat(Chat&& rhs) : mCoroHdl{std::exchange(rhs.mCoroHdl, nullptr)} {} // Move only!

	~Chat() // Care taking, destroying the handle if needed
	{
		if (mCoroHdl) { mCoroHdl.destroy(); }
	}

	std::string listen() // Active the coroutine and wait for data
	{
		if (not mCoroHdl.done()) { mCoroHdl.resume(); }
		return std::move(mCoroHdl.promise()._msgOut);
	}

	void answer(std::string msg) // Send data to the coroutine and activate it
	{
		mCoroHdl.promise()._msgIn = msg;
		if (not mCoroHdl.done()) { mCoroHdl.resume(); }
	}
};

Chat Fun() // Wrapper type Chat containing the promise type
{
	co_yield "Hello!\n"s; // Calls promise_type.yield_value
	std::cout << co_await std::string{}; // Calls promise_type.await_transform
	co_return "Here!\n"s; // Calls promise_type.return_value
}

int main() {
	Chat chat = Fun(); // Creation of the coroutine
	std::cout << chat.listen(); // Trigger the machine
	chat.answer("Where are you?\n"s); // Send data into the coroutine
	std::cout << chat.listen(); // Wait for more data from the coroutine
}

