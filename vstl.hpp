
/*
 * MIT License
 *
 * Copyright (c) 2020 - 2025 magistermaks
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * VSTL - Very Simple Test Library
 *
 * This library uses an single header that should be included into each test unit (program).
 * This library uses C++20 features, and can't be used on older C++ versions, see demo.cpp for reference.
 *
 * 1) Settings
 *
 *    The following settings can be configured with #define directives placed before
 *    the inclusion of VSTL, the listing shows the default values of all available options.
 *
 *    #define VSTL_TEST_COUNT 1            - Number of times each test should be invoked, each invocations needs to succeed for the test to be passed
 *    #define VSTL_USE_ANSI true           - Should VSTL use ANSI escape codes to show colored output?
 *    #define VSTL_TRIGGER_DEBUGGER true   - Should VSTL try to trigger the debugger (when attached) when a assertion fails?
 *    #define VSTL_PRINT_SKIP_REASON false - Should VSTL print the messages given to SKIP()?
 *    #define VSTL_PRINT_SUCCESS true      - Should VSTL print a log entry when the test is successful?
 *    #define VSTL_PRINT_TIME true         - Should VSTL print the time taken to run the tests?
 *
 * 2) Structure
 *
 *    Each test executable that is to use the VSTL framework must include the
 *    VSTL header, entrypoint will already be defined by VSTL, so all you need are the (optional)
 *    settings, VSTL include, and the tests themselves.
 *
 *    ```c++
 *    // optional settings have to be placed before the include
 *    #define VSTL_PRINT_SKIP_REASON true
 *    #include <vstl.hpp>
 *
 *    TEST(any_test_name) {
 *        // test itself
 *    };
 *
 *    TEST(another_test) {
 *        // tests need not have a unique name, but it is advised that they do
 *    };
 *    ```
 *
 * 3) Usage
 *
 *    Inside a test you can now use VSTL assertions to check the validity of
 *    your code, all of them can be seen listed below. Each of them will automatically appended the line number.
 *
 *    FAIL(reason)
 *	      This one is not an assertion, but you can use it at any point to FAIL the test with a
 *        custom message. For example, when inside an if.
 *
 *    SKIP(reason)
 *        This one is not an assertion, but you can use it at any point to SKIP the test with a
 *        custom message. That message will only be printed if the `VSTL_PRINT_SKIP_REASON` setting
 *        was enabled. It can be used to disable environment dependant tests.
 *
 *    ASSERT(condition)
 *        Asserts the given condition is true, otherwise FAIL the test. The error message will
 *        include the condition itself, if you wish to use a custom message use `ASSERT_MSG`.
 *
 *    ASSERT_MSG(condition, message)
 *        Asserts the given condition is true, otherwise FAIL the test with the message provided.
 *        If most cases you should use the plain `ASSERT` to derive the message from the condition itself.
 *
 *    CHECK(first, second)
 *        Check if the two values provided can be equated using the operator==, otherwise FAIL the test, with a message
 *        that includes the actual values of the two values, converted to printable strings.
 *
 *    EXPECT_ANY() {}
 *        Check if ANY exception is thrown (throw X;) in the following code block, otherwise FAIL the test.
 *        This includes sts::exception and non-standard exception types (such as primitives and custom types).
 *        For a more fine-grained assertion see `EXPECT_THROW`.
 *
 *    EXPECT_THROW(type) {}
 *        Check if exception of type `type` is thrown (throw X;) in the following code block otherwise FAIL the test.
 *        This can include any type, including sts::exception and non-standard exception types (such as primitives and custom types).
 *        For just check if anything was throw see `EXPECT_ANY`, and to check if a POSIX signal was raised see `EXPECT_SIGNAL`.
 *
 *    EXPECT_SIGNAL(signum) {}
 *        Check if the specified signal (one of, SIGSEGV, SIGILL, SIGFPE, SIGABRT, and SIGTERM) was raised in the following
 *        block, either by calling raise() or from any other source, otherwise FAIL the test. This assertion is capable of catching
 *        errors generated by faulty code, such as SEGFAULTS triggered by illegal memory operations.
 *
 * 4) Handlers
 *
 *    If your code makes use of custom exceptions that do not derive from std::exception, you may define a custom VSTL
 *    error handler to translate your exception into appropriate printable strings (those will then be used if that custom
 *    exception causes a test-fail). This is generally unadvised (your exception should derive from std::exception) but can
 *    be achieved in VSTL as shown below.
 *
 *    ```c++
 *    #include <vstl.hpp>
 *
 *    struct Custom { int x; };
 *
 *    HANDLER {
 *        CATCH_PTR (const Custom& e) {
 *            FAIL("Custom " + std::to_string(e.x));
 *        }
 *    };
 *
 *    TEST(failed) {
 *        Custom val;
 *        val.x = 42;
 *
 *        // will print "Error: Custom 42"
 *        throw val;
 *    };
 *    ```
 */

#pragma once

/// Number of times each test should be run before declaring it a success
#ifndef VSTL_TEST_COUNT
#	define VSTL_TEST_COUNT 1
#endif

/// Should we try to automatically trigger the debugger when an assertion fails?
#ifndef VSTL_TRIGGER_DEBUGGER
#	define VSTL_TRIGGER_DEBUGGER true
#endif

/// Should VSTL print the messages given to SKIP()?
#ifndef VSTL_PRINT_SKIP_REASON
#	define VSTL_PRINT_SKIP_REASON false
#endif

/// Should VSTL print a log entry when the test is successful?
#ifndef VSTL_PRINT_SUCCESS
#	define VSTL_PRINT_SUCCESS true
#endif

/// Should VSTL print the time taken to run the tests?
#ifndef VSTL_PRINT_TIME
#	define VSTL_PRINT_TIME true
#endif

/// Should VSTL use ANSI escape codes to show colored output?
#ifndef VSTL_USE_ANSI
#	define VSTL_USE_ANSI true
#endif

#if VSTL_USE_ANSI
#	define VSTL_FAILED "\033[31;1mfailed\033[0m"
#	define VSTL_SKIPPED "\033[33;1mskipped\033[0m"
#	define VSTL_SUCCESSFUL "\033[32;1msuccessful\033[0m"
#else
#	define VSTL_FAILED "failed"
#	define VSTL_SKIPPED "skipped"
#	define VSTL_SUCCESSFUL "successful"
#endif

// internal platform abstraction macros, don't use :gun:
#ifdef _WIN32
#	include <windows.h>
#	define VSTL_JMP_SIG(jmp) longjmp(jmp, 1)
#	define VSTL_JMP_SET(jmp) setjmp(jmp)
#	define VSTL_TRAP() DebugBreak()
#else
#	include <sys/time.h>
#	define VSTL_JMP_SIG(jmp) siglongjmp(jmp, 1)
#	define VSTL_JMP_SET(jmp) sigsetjmp(jmp, 0xffffffff)
#	define VSTL_TRAP() raise(SIGTRAP)
#endif

#include <csignal>
#include <csetjmp>
#include <vector>
#include <functional>
#include <exception>
#include <chrono>
#include <iostream>
#include <sstream>

#define VSTL_VERSION "3.3"

// internal macros, don't use :gun:
#define VSTL_UNEQUAL(va, vb) for(auto __vstl_a__ = (va), __vstl_b__ = (decltype(__vstl_a__)) (vb); __vstl_a__ != __vstl_b__;)
#define VSTL_BLC ;
#define VSTL_JOIN(prefix, suffix) prefix##suffix
#define VSTL_CAT(prefix, suffix) VSTL_JOIN(prefix, suffix)
#define VSTL_UNIQUE(prefix) VSTL_CAT(prefix, __LINE__)
#define VSTL_STR_VALUE(value) #value
#define VSTL_TO_STR(macro) VSTL_STR_VALUE(macro)
#define VSTL_LINE "on line " VSTL_TO_STR(__LINE__)
#define VSTL_EXCEPT "Expected exception"
#define VSTL_RETHROW catch (vstl::test_error& fail) { throw fail; }
#define VSTL_VTOS(value) + vstl::to_printable(value) +
#define VSTL_CLAUSE []() noexcept(false) -> void
#define VSTL_WRAP void(0), [] (const std::function<void()>& inner)

/// define a test of the given [name]: TEST(example_test) { /* the test */ }
#define TEST(name) \
VSTL_BLC vstl::test VSTL_UNIQUE(__vstl_test__) = #name+[] (const vstl::test& vstl_self) noexcept(false) -> void

/// used to defined error handlers (converters), place anywhere in the test file. use like this: HANDLER { CATCH_PTR (my_error_class& err) { FAIL(err.str())  } }
#define HANDLER \
VSTL_BLC vstl::handler VSTL_UNIQUE(__vstl_handler__) = "handler"+[] (const std::exception_ptr& ptr) noexcept(false) -> void

/// helper used in defining error handlers
#define CATCH_PTR \
try { if(ptr) std::rethrow_exception(ptr); } catch

/// fails the test with the given [reason] when called
#define FAIL(reason) \
vstl::fail("" VSTL_VTOS(reason) ", " VSTL_LINE "!");

/// skip the test with the given [reason] when called
#define SKIP(reason) \
vstl::skip("" VSTL_VTOS(reason) ", " VSTL_LINE "!");

/// asserts the [condition] is true, otherwise fails the test with the custom [reason]
#define ASSERT_MSG(condition, reason) \
if(!(condition)) FAIL(reason)

/// asserts the [condition] is true, otherwise fails the test
#define ASSERT(condition) \
ASSERT_MSG(condition, "Expected " #condition " to be true, but it was not")

/// checks if the [va] equals [vb], otherwise fails the test
#define CHECK(va, vb) \
VSTL_UNEQUAL(va, vb) FAIL("Expected " VSTL_VTOS(__vstl_a__) " to be equal " VSTL_VTOS(__vstl_b__) ", " #va " != " #vb)

/// checks if the following block throws an exception, otherwise fails the test
#define EXPECT_ANY() \
VSTL_WRAP { try { inner(); FAIL(VSTL_EXCEPT); } VSTL_RETHROW catch (...) {} } + VSTL_CLAUSE

/// checks if the following block throws an exception of the given [type], otherwise fails the test
#define EXPECT_THROW(type) \
VSTL_WRAP { try { inner(); FAIL(VSTL_EXCEPT); } VSTL_RETHROW catch (type& t) {} catch (...) { FAIL("Expected exception of type " #type); } } + VSTL_CLAUSE

/// assert signal of number [signum] being raised in te following block, otherwise fails the test
#define EXPECT_SIGNAL(signum) \
VSTL_WRAP { vstl::expected_signal = signum; if (VSTL_JMP_SET(vstl::expect_jmp) == 0) { inner(); FAIL("Expected signal " #signum); } } + VSTL_CLAUSE

/// set test timeout
#define TIMEOUT(seconds) \
vstl::set_timeout(seconds * 1000);

// internal VSTL namespace, don't use :gun:
namespace vstl {



	/*
	 * Those concepts are used to determine how a value can be converted to std::string,
	 * given a value the vstl::to_printable() WILL return some value based on those concepts
	 */

	template <typename T>
	concept string_convertible = requires(T value) { std::to_string(value); };

	template <typename T>
	concept string_castable = std::convertible_to<T, std::string>;

	template <typename T>
	concept string_appendable = requires(T value, std::stringstream ss) { ss << value; } && !string_convertible<T> && !string_castable<T>;



	/*
	 * The vstl::to_printable() function is used by VSTL to print useful representations of
	 * tested values, if all fails a generic fallback string will be used
	 */

	template <typename T>
	std::string to_printable(const T& value) {
		return "<non-printable value>";
	}

	template <string_convertible T>
	std::string to_printable(const T& value) {
		return std::to_string(value);
	}

	template <string_castable T>
	std::string to_printable(const T& value) {
		return value;
	}

	template <string_appendable T>
	std::string to_printable(const T& value) {
		std::stringstream ss;
		ss << value;
		return ss.str();
	}



	/*
	 * Internal VSTL state and functions to manipulate it,
	 * as well as some other small bits and pieces
	 */

	struct test;
	struct handler;

	/// index of the current test, needed by the signal handlers
	size_t index = 0;

	/// number of failed tests
	size_t failed = 0;

	/// number of successful tests (includes skipped tests)
	size_t successful = 0;

	/// number of skipped tests
	size_t skipped = 0;

	/// jump buffer used to return out of signal handlers
	jmp_buf jmp {};

	/// should the test treat SIGALRM as an error condition
	bool fail_on_alarm = false;

	/// used by the EXPECT_SIGNAL block
	int expected_signal = 0;

	/// used by the EXPECT_SIGNAL block, where to jump to when the expected signal comes
	jmp_buf expect_jmp;

	/// Exceptions used by VSTL assertions
	struct test_error final : std::runtime_error { explicit test_error(const std::string& error) : runtime_error(error) {} };
	struct test_skip final : std::runtime_error { explicit test_skip(const std::string& error) : runtime_error(error) {} };

	void set_timeout(size_t milliseconds) {

		fail_on_alarm = (milliseconds != 0);

#ifdef _WIN32

		// TODO: Unimplemented on windows

#else

		size_t seconds = milliseconds / 1000;
		size_t reminder = milliseconds % 1000;
		size_t microseconds = reminder * 1000;

		struct itimerval timer;
		timer.it_interval.tv_sec = 0;
		timer.it_interval.tv_usec = 0;
		timer.it_value.tv_sec = seconds;
		timer.it_value.tv_usec = microseconds;

		setitimer(ITIMER_REAL, &timer, nullptr);

#endif
	}



	/*
	 * The vstl::handler class automatically registers itself to the handlers vector
	 * upon construction, it contains logic to translate custom exceptions and print them
	 */

	// handler self-registers itself so it needs access to this
	void append(const handler& handler);

	struct handler final {

		using functor = std::function<void(const std::exception_ptr&)>;

		const functor func;

		handler(const functor& func)
		: func(func) {
			append(*this);
		}

		void call(const std::exception_ptr& ptr) const {
			func(ptr);
		}

	};

	/// error handlers, lambdas that can catch specific exceptions and rethrow them as vstl::test_error
	std::vector<handler> handlers;

	/// called by handler constructor
	void append(const handler& handler) {
		handlers.emplace_back(handler);
	}



	/*
	 * The vstl::test class automatically registers itself to the tests vector
	 * upon construction, it contains the logic to handle excpetions but not signal
	 */

	// test self-registers itself so it needs access to this
	void append(const test& test);

	struct test final {

		using functor = std::function<void(const test& test)>;

		const char* name;
		const functor func;

		test(const char* name, const functor& func)
		: name(name), func(func) {
			append(*this);
		}

		void call(const size_t count) const {
			for (size_t i = 0; i < count; i ++) {

				// reset any pending alarm
				set_timeout(0);

				// reset test state per invocation
				expected_signal = 0;
				fail_on_alarm = false;

				func(*this);
			}
		}

		bool run(std::ostream& out) const throw() {
			try {
				call(VSTL_TEST_COUNT);
			} catch (const test_skip& skip) {
				out << "Test '" << this->name << "' " VSTL_SKIPPED "!";

				if (VSTL_PRINT_SKIP_REASON) {
					out << " " << skip.what();
				}

				skipped ++;
				out << std::endl;
				return true;

			} catch (const test_error& fail) {
				out << "Test '" << this->name << "' " VSTL_FAILED "! Error: " << fail.what() << std::endl;
				return false;

			} catch (const std::exception& error) {
				out << "Test '" << this->name << "' " VSTL_FAILED "! Exception: " << error.what() << std::endl;
				return false;

			} catch (...) {
				std::exception_ptr ptr = std::current_exception();

				// try to convert the error using the defined error handlers
				for (const handler& handler : handlers) {
					try {
						handler.call(ptr);
					} catch(const test_error& fail) {
						out << "Test '" << this->name << "' " VSTL_FAILED "! Error: " << fail.what() << std::endl;
						return false;
					} catch (...) {
						// ignore
					}
				}

				// everything has failed us, just try to print *some* reason
				try {
					out << "Test '" << this->name << "' " VSTL_FAILED "! Unregistered exception thrown! ";
					std::rethrow_exception(ptr);
				} catch (const char* err) {
					out << "Error: " << err << std::endl;
				} catch (int err) {
					out << "Error: (int) " << err << std::endl;
				} catch (long err) {
					out << "Error: (long) " << err << std::endl;
				} catch (...) {
					out << "Error: unknown" << std::endl;
				}

				return false;
			}

			if (VSTL_PRINT_SUCCESS) {
				out << "Test '" << this->name << "' " VSTL_SUCCESSFUL "!" << std::endl;
			}
			return true;
		}

	};

	/// tests to execute
	std::vector<test> tests;

	/// called by test constructor
	void append(const test& test) {
		tests.emplace_back(test);
	}



	/*
	 * signal handlers invoked by the harness, as we can't really pass any user data into them
	 * we rely on there being only one test harness at a time - VSTL can only be included once per executable
	 */

	const char* get_signal_name(int sig) {
		if (sig == SIGSEGV) return "SIGSEGV";
		if (sig == SIGILL) return "SIGILL";
		if (sig == SIGFPE) return "SIGFPE";
		if (sig == SIGABRT) return "SIGABRT";
		if (sig == SIGTERM) return "SIGTERM";

#ifndef _WIN32
		if (sig == SIGTRAP) return "SIGTRAP";
#endif

		return "unknown signal";
	}

	bool handle_shared_signal(int sig) {
#ifndef _WIN32
		if (sig == SIGALRM && fail_on_alarm) {
			printf("Test '%s' " VSTL_FAILED "! Timeout reached!\n", tests[index].name);
			return false; // skip default print
		}
#endif

		if (expected_signal == sig) {
			expected_signal = 0;
			VSTL_JMP_SIG(expect_jmp);
		}

		// all other cases, refer to print_signal()
		return true;
	}

#ifdef _WIN32
	void print_signal(const char* test, int sig) {
		printf("Test '%s' " VSTL_FAILED "! Error: Received %s (#%d)!\n", test, get_signal_name(sig), sig);
	}

	void signal_handler(int sig) {
		if (handle_shared_signal(sig)) {
			print_signal(tests[index].name, sig);
		}

		VSTL_JMP_SIG(jmp);
	}
#else
	void print_signal(const char* test, int sig, int64_t address) {
		printf("Test '%s' " VSTL_FAILED "! Error: Received %s (#%d) while trying to access: 0x%lx!\n", test, get_signal_name(sig), sig, address);
	}

	void signal_handler(int sig, siginfo_t* si, void* unused) {
		if (handle_shared_signal(sig)) {
			print_signal(tests[index].name, sig, reinterpret_cast<int64_t>(si->si_addr));
		}

		VSTL_JMP_SIG(jmp);
	}
#endif



	/*
	 * core VSTL functions that handle test running, initialization
	 * and result summarization
	 */

	/// output a simple summary line
	void summary(std::ostream& out, const auto& time) {
		size_t executed = failed + successful;
		double millis = std::chrono::duration<double, std::milli>(time).count();

		out << std::endl << std::dec << "Executed " << executed << " ";
		out << (executed == 1 ? "test" : "tests") << ", ";
		out << failed << " failed, ";
		out << (successful - skipped) << " succeeded.";

		if (VSTL_PRINT_TIME) {
			out << " (time: " << millis << "ms)";
		}
		out << std::endl;
	}

	void catch_signal(int signum) {
#ifdef _WIN32
		signal(signum, signal_handler);
#else

		// our signal action description
		struct sigaction action;
		action.sa_flags = SA_SIGINFO;
		action.sa_flags = SA_SIGINFO | SA_NODEFER | SA_ONSTACK;
		sigemptyset(&action.sa_mask);
		action.sa_sigaction = signal_handler;

		sigaction(signum, &action, nullptr);

#endif
	}

#ifdef _WIN32
	LONG WINAPI vector_exception_handler(EXCEPTION_POINTERS* info) {
		if (info->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
			signal_handler(SIGSEGV);
		}

		return EXCEPTION_CONTINUE_SEARCH;
	}
#endif

	/// setup signal handlers
	void init() {
#ifdef _WIN32
		AddVectoredExceptionHandler(1, vector_exception_handler);
#else

		// custom stack to make VSTL more resilient if the stack pointer gets corrupted
		stack_t stack;
		stack.ss_sp = malloc(SIGSTKSZ);
		stack.ss_size = SIGSTKSZ;
		stack.ss_flags = 0;
		sigaltstack(&stack, nullptr);

		catch_signal(SIGALRM);

		if (VSTL_TRIGGER_DEBUGGER) {
			signal(SIGTRAP, SIG_IGN);
		}

#endif

		catch_signal(SIGSEGV);
		catch_signal(SIGILL);
		catch_signal(SIGFPE);
		catch_signal(SIGABRT);
		catch_signal(SIGTERM);

	}

	/// Invoke all tests
	void run(std::ostream& out) {
		const auto start = std::chrono::steady_clock::now();

		index = 0;
		successful = 0;
		failed = 0;
		skipped = 0;

		for (const test& test : tests) {

			// if we go into this if a signal was
			// raised during test execution
			if (VSTL_JMP_SET(jmp)) {
				failed ++;
				goto skip;
			}

			// if we enter this if an exception was
			// thrown during test execution
			if (!test.run(out)) {
				failed ++;
				goto skip;
			}

			// test completed successfully
			successful ++;

			skip:
			index ++;
		}

		summary(out, std::chrono::steady_clock::now() - start);
	}

	/// returns the value the test program should return
	size_t get_exit_code() {
		return failed != 0 ? 1 : 0;
	}

	template<typename S>
	void fail(const S& message) {
		if (VSTL_TRIGGER_DEBUGGER) {

			// Why am I taken here?
			// By default VSTL will send a signal to the attached debugger when an assertion fails
			// so that you can debug your code without restarting the test suite. If that is not desired
			// you can run you program without a debugger or suppress it using #define VSTL_TRIGGER_DEBUGGER false
			VSTL_TRAP();
		}

		throw test_error {message};
	}

	template<typename S>
	void skip(const S& message) {
		throw test_skip {message};
	}

}



/*
 * Those operators are used as special constructors and are needed
 * so the "TEST(name) {}" syntax can be used
 */

inline vstl::test operator +(const char* name, const vstl::test::functor& tester) {
    return vstl::test {name, tester};
}

inline vstl::handler operator +(const char* name, const vstl::handler::functor& handler) {
    return vstl::handler {handler};
}

inline void operator +(const std::function<void(const std::function<void()>&)>& wrapper, const std::function<void()>& inner) {
	wrapper(inner);
}



/*
 * VSTL entrypoint
 * this library should be included once per executable target
 */

int main() {
	vstl::init();
	vstl::run(std::cout);
	return vstl::get_exit_code();
}