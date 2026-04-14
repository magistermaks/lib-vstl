
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
 *    The following settings can be configured with VCONF() macros placed anywhere in the
 *    test suite, they apply globally to all tests and all modules. The listing shows the
 *    default values of all available options.
 *
 *    VCONF(repeats, 1)             - Number of times each test should be invoked, each invocations MUST succeed for the test to pass
 *    VCONF(trigger_debugger, true) - Should VSTL try to trigger the debugger (when attached) when a assertion fails?
 *    VCONF(print_skip, true)       - Should VSTL print a log entry when the test is skipped?
 *    VCONF(print_passed, true)     - Should VSTL print a log entry when the test is successful?
 *    VCONF(print_time, true)       - Should VSTL print the time taken to run the tests?
 *    VCONF(print_modules, false)   - Should VSTL print the current module (source unit)?
 *    VCONF(print_color, true)      - Should VSTL use color codes when printing?
 *
 * 2) Structure
 *
 *    Each test executable that is to use the VSTL framework must include the
 *    VSTL header, entrypoint will already be defined by VSTL, so all you need are the (optional)
 *    settings, VSTL include, and the tests themselves.
 *
 *    ```c++
 *    #include <vstl.hpp>
 *
 *    // optional settings, they are global and apply to all files linked together
 *    // VCONF(retries, 2);
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
 *        Check if the specified signal (one of, SIGSEGV, SIGILL, SIGFPE, SIGABRT, SIGTERM, and SIGBUS) was raised in the following
 *        block, either by calling raise() or from any other source, otherwise FAIL the test. This assertion is capable of catching
 *        errors generated by faulty code, such as SEGFAULTS triggered by illegal memory operations.
 *
 * 4) Exception Handlers
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

#define VSTL_COLOR_RED "\033[31m"
#define VSTL_COLOR_YELLOW "\033[33m"
#define VSTL_COLOR_GREEN "\033[32m"
#define VSTL_COLOR_CYAN "\033[36m"
#define VSTL_COLOR_PURPLE "\033[35m"
#define VSTL_COLOR_BOLD "\033[1m"
#define VSTL_COLOR_RESET "\033[0m"

// internal platform abstraction macros, don't use :gun:
#ifdef _WIN32
#	define VSTL_JMP_SIG(jmp) longjmp(jmp, 1)
#	define VSTL_JMP_SET(jmp) setjmp(jmp)
#else
#	define VSTL_JMP_SIG(jmp) siglongjmp(jmp, 1)
#	define VSTL_JMP_SET(jmp) sigsetjmp(jmp, 0xffffffff)
#endif

#include <csignal>
#include <csetjmp>
#include <functional>
#include <exception>
#include <chrono>
#include <sstream>
#include <cstring>
#include <filesystem>

#define VSTL_VERSION "3.4"

// internal macros, don't use :gun:
#define VSTL_COLOR_FAILED VSTL_COLOR_RED VSTL_COLOR_BOLD "failed" VSTL_COLOR_RESET
#define VSTL_COLOR_SKIPPED VSTL_COLOR_YELLOW VSTL_COLOR_BOLD "skipped" VSTL_COLOR_RESET
#define VSTL_COLOR_PASSED VSTL_COLOR_GREEN VSTL_COLOR_BOLD "passed" VSTL_COLOR_RESET
#define VSTL_STR_HIGHLIGHT vstl::color(VSTL_COLOR_RED)
#define VSTL_STR_LINE_SPEC vstl::color(VSTL_COLOR_BOLD)
#define VSTL_STR_END vstl::color(VSTL_COLOR_RESET)
#define VSTL_UNEQUAL(va, vb) for(auto __vstl_a__ = (va), __vstl_b__ = (decltype(__vstl_a__)) (vb); __vstl_a__ != __vstl_b__;)
#define VSTL_BLC ;
#define VSTL_JOIN(prefix, suffix) prefix##suffix
#define VSTL_CAT(prefix, suffix) VSTL_JOIN(prefix, suffix)
#define VSTL_UNIQUE(prefix) VSTL_CAT(prefix, __LINE__)
#define VSTL_STR_VALUE(value) #value
#define VSTL_TO_STR(macro) VSTL_STR_VALUE(macro)
#define VSTL_LINE ("on line " + VSTL_STR_LINE_SPEC + VSTL_TO_STR(__LINE__) + VSTL_STR_END)
#define VSTL_EXCEPT "Expected exception"
#define VSTL_RETHROW catch (vstl::TestError& fail) { throw fail; }
#define VSTL_VTOS(value, hints) vstl::to_printable(value, hints)
#define VSTL_CLAUSE [&] () noexcept(false) -> void
#define VSTL_WRAP void(0), [] (const std::function<void()>& inner)
#define VSTL_GLOBAL static inline

/*
 * BEGIN VSTL API
 */

/// define a test of the given [name]: TEST(example_test) { /* the test */ }
#define TEST(name) \
VSTL_BLC VSTL_GLOBAL vstl::Test VSTL_UNIQUE(__vstl_test_##name##__) = vstl::Metadata{__FILE__, #name}+[] (const vstl::Test& vstl_self) noexcept(false) -> void

/// used to defined error handlers (converters), place anywhere in the test file. use like this: HANDLER { CATCH_PTR (my_error_class& err) { FAIL(err.str())  } }
#define HANDLER \
VSTL_BLC VSTL_GLOBAL vstl::Handler VSTL_UNIQUE(__vstl_handler__) = "handler"+[] (const std::exception_ptr& ptr) noexcept(false) -> void

/// helper used in defining error handlers
#define CATCH_PTR \
try { if(ptr) std::rethrow_exception(ptr); } catch

/// fails the test with the given [reason] when called
#define FAIL(reason) \
vstl::fail("" + VSTL_VTOS(reason, {}) + ", " + VSTL_LINE + "!");

/// skip the test with the given [reason] when called
#define SKIP(reason) \
vstl::skip("" + VSTL_VTOS(reason, {}) + ", " + VSTL_LINE + "!");

/// asserts the [condition] is true, otherwise fails the test with the custom [reason]
#define ASSERT_MSG(condition, reason) \
if(!(condition)) FAIL(reason)

/// asserts the [condition] is true, otherwise fails the test
#define ASSERT(condition) \
ASSERT_MSG(condition, "Expected " #condition " to be true, but it was not")

/// checks if the [va] equals [vb], otherwise fails the test
#define CHECK(va, vb) \
VSTL_UNEQUAL(va, vb) FAIL("Expected " + VSTL_STR_HIGHLIGHT + VSTL_VTOS(__vstl_a__, vstl::guess_print_hint(#va, #vb)) + VSTL_STR_END + " to be equal " + VSTL_STR_HIGHLIGHT + VSTL_VTOS(__vstl_b__, vstl::guess_print_hint(#va, #vb)) + VSTL_STR_END + ", " #va " != " #vb)

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

/// set configuration [key] to the given [value] globaly for all tests and modules
#define VCONF(key, value) \
VSTL_GLOBAL int VSTL_UNIQUE(__cfg_##key##__) = [] () noexcept(true) -> int { vstl::config_##key = value; return 0; } ()

/*
 * END VSTL API
 */

// internal namespace, don't use :gun:
namespace vstl {

	/*
	 * Printer hints used to manipulate how some value should be displayed,
	 * this is used to use hexadecimal notation in some cases.
	 */

	struct print_hint {
		int base = 10;
		bool uppercase = false;
	};

	print_hint guess_print_hint(const std::string_view& one, const std::string_view& two);



	/*
	 * Those concepts are used to determine how a value can be converted to std::string,
	 * given a value the vstl::to_printable() WILL return some value based on those concepts
	 */

	template <typename T>
	concept string_castable = std::convertible_to<T, std::string>;

	template <typename T>
	concept string_convertible = requires(T value) { std::to_string(value); };

	template <typename T>
	concept string_appendable = requires(T value, std::stringstream ss) { ss << value; };

	template <typename T>
	concept any_pair = requires(T value) { value.first; value.second; };

	template <typename T>
	concept iterable = requires(T value) { value.cbegin(); value.cend(); };



	/*
	 * The vstl::to_printable() function is used by VSTL to print useful representations of
	 * tested values, if all fails a generic fallback string will be used
	 */

	template <typename T>
	std::string to_printable(const T& value, print_hint hint) {

		// if the value can be cast to string (or is a string already) use that
		if constexpr (string_castable<T>) {
			return static_cast<std::string>(value);
		}

		// check for user defined "to string" style method - str()
		if constexpr (requires { {value.str()} -> std::convertible_to<std::string>; }) {
			return static_cast<std::string>(value.str());
		}

		// check for user defined "to string" style method - string()
		if constexpr (requires { {value.string()} -> std::convertible_to<std::string>; }) {
			return static_cast<std::string>(value.string());
		}

		// check for user defined "to string" style method - to_string()
		if constexpr (requires { {value.to_string()} -> std::convertible_to<std::string>; }) {
			return static_cast<std::string>(value.to_string());
		}

		// check for user defined "to string" style method - toString()
		if constexpr (requires { {value.toString()} -> std::convertible_to<std::string>; }) {
			return static_cast<std::string>(value.toString());
		}

		// custom logic for bool values
		if constexpr (std::is_same_v<T, bool>) {
			return value ? "true" : "false";
		}

		// we need to exclude bools explicitly as is_integral includes them, and causes compilers to complain
		if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
			const char* digits = hint.uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
			const char* base_prefix = "";

			using U = std::make_unsigned<T>::type;

			uint32_t base = hint.base;
			U local = std::bit_cast<U>(value);

			if (base == 2) {
				base_prefix = "0b";
			} else if (base == 8) {
				base_prefix = "0";
			} else if (base == 16) {
				base_prefix = "0x";
			} else {
				return std::to_string(local);
			}

			std::string result;

			while (local != 0) {
				result.push_back(digits[local % base]);
				local /= base;
			}

			if (result.empty()) return "0";

			std::reverse(result.begin(), result.end());
			return base_prefix + result + " (decimal " + std::to_string(value) + ")";
		}

		// mostly for user defined operator<< as most other values are already handled above
		if constexpr (string_appendable<T>) {
			std::stringstream ss;
			ss << value;
			return ss.str();
		}

		// we check for pairs above lists as pairs can also have the begin()/end() methods, this is needed for maps
		if constexpr (any_pair<T>) {
			return "{" + to_printable(value.first, hint) + ", " + to_printable(value.second, hint) + "}";
		}

		// as the last thing try printing collections
		if constexpr (iterable<T>) {
			std::string result;

			for (const auto& element : value) {
				result += to_printable(element, hint);
				result += ", ";
			}

			// remove trailing ", "
			if (!result.empty()) {
				result.pop_back();
				result.pop_back();
			}

			return "[" + result + "]";
		}

		// we don't know how to print this thing...
		return "<non-printable value>";
	}



	/*
	 * Internal VSTL state and functions to manipulate it,
	 * as well as some other small bits and pieces
	 */

	enum Status {
		PASSED,
		SKIPPED,
		FAILED,
	};

	struct Result {
		Status status;
		std::string message;
	};

	struct Test;
	struct Handler;

	struct Metadata {
		const char* module;
		const char* name;

		size_t length() const {
			return strlen(name);
		}
	};

	inline int config_repeats = 1;
	inline bool config_print_time = true;
	inline bool config_print_skip = true;
	inline bool config_print_passed = true;
	inline bool config_trigger_debugger = false;
	inline bool config_print_modules = false;
	inline bool config_print_color = true;

	/// jump buffer used to return out of signal handlers
	inline jmp_buf jmp {};

	/// used by the EXPECT_SIGNAL block
	inline int expected_signal = 0;

	/// used by the EXPECT_SIGNAL block, where to jump to when the expected signal comes
	inline jmp_buf expect_jmp;

	/// Exceptions used by VSTL assertions
	struct TestError final : std::runtime_error { explicit TestError(const std::string& error) : runtime_error(error) {} };
	struct TestSkip final : std::runtime_error { explicit TestSkip(const std::string& error) : runtime_error(error) {} };

	void set_timeout(size_t milliseconds);

	std::string color(const char* escape);



	/*
	 * The vstl::handler class automatically registers itself to the handlers vector
	 * upon construction, it contains logic to translate custom exceptions and rethow them as TestError
	 */

	void register_handler(const Handler& handler);

	struct Handler final {

		using functor = std::function<void(const std::exception_ptr&)>;

		const functor func;

		Handler(const functor& func)
		: func(func) {
			register_handler(*this);
		}

		void call(const std::exception_ptr& ptr) const {
			func(ptr);
		}

	};



	/*
	 * The vstl::test class automatically registers itself to the tests vector
	 * upon construction.
	 */

	void register_test(const Test& test);

	struct Test final {

		using functor = std::function<void(const Test& test)>;

		Metadata meta;
		functor func;

		Test(Metadata meta, functor func)
		: meta(meta), func(std::move(func)) {
			register_test(*this);
		}

		[[nodiscard]]
		const char* name() const {
			return meta.name;
		}

	};



	/*
	 * core VSTL functions that handle test running, initialization
	 * and result summarization
	 */

	/// Trigger debugger
	void trap();

	/// Get the longest test name in characters
	size_t get_longest_test_name();

	template<typename S>
	void fail(const S& message) {
		if (config_trigger_debugger) {

			// Why am I taken here?
			// By default, VSTL will send a signal to the attached debugger when an assertion fails
			// so that you can debug your code without restarting the test suite. If that is not desired
			// you can run you program without a debugger or suppress it using #define VSTL_TRIGGER_DEBUGGER false
			trap();
		}

		throw TestError {message};
	}

	template<typename S>
	void skip(const S& message) {
		throw TestSkip {message};
	}

}



/*
 * Those operators are used as special constructors and are needed
 * so the "TEST(name) {}" syntax can be used
 */

inline vstl::Test operator +(vstl::Metadata info, const vstl::Test::functor& tester) {
	return vstl::Test {info, tester};
}

inline vstl::Handler operator +(const char* name, const vstl::Handler::functor& handler) {
	return vstl::Handler {handler};
}

inline void operator +(const std::function<void(const std::function<void()>&)>& wrapper, const std::function<void()>& inner) {
	wrapper(inner);
}
