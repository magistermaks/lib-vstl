
#define VSTL_USE_ANSI false
#define VSTL_PRINT_TIME false
#define VSTL_PRINT_SKIP_REASON true
#define VSTL_TRIGGER_DEBUGGER false

#include "vstl.hpp"

#include <thread>
#include <chrono>

// each test begins with the TEST(name) clause
// the test names need not be unique
TEST(vstl_check) {
	std::vector<int> vec {1, 2, 3, 4, 5, 6, 7, 8};

	// use the CHECK macro to assert equality
	CHECK(vec[0], 1);
	CHECK(vec[2], 3);
	CHECK(vec[3], 4);

	// oh no! will print this message:
	// Error: Expected 2 to be equal 4, vec[1] != 4!
	CHECK(vec[1], 4);

	// remember to put ';' at the end, a TEST is not a function!
};


TEST(vstl_fail) {

	// you can use the FAIL macro to manually
	// fail the test with a custom message, like this:

	if (bool oops = true) {

		// oh no! will print this message:
		// Error: Oops!
		FAIL("Oops");
	}

};


TEST(vstl_assert) {

	// there are also simple ASSERT and
	// ASSERT_MSG macros available, if CHECK is not your cup of tea

	int a = 21;
	int b = 42;

	ASSERT(a * 2 == b);
	ASSERT_MSG(a == b / 2, "Joker");

	// prints: Expected a * 3 == b to be true, but it was not!
	// ASSERT(a * 3 == b);

	// prints: Error: Thief!
	ASSERT_MSG(a == b / 3, "Thief");

};


TEST(vstl_expect) {

	// you can also assert that an exception
	// should be thrown in some block,
	// either of any or some specific type

	// check if any exception was thrown
	EXPECT_ANY() {
		throw "oh my!";
	};

	// check if runtime_error was thrown
	EXPECT_THROW(std::runtime_error) {
		throw std::runtime_error {"Error of a runtime type"};
	};

	// any exception type is valid
	EXPECT_THROW(int) {
		throw 42;
	};

	// prints: Error: Expected exception of type std::runtime_error!
	EXPECT_THROW(std::runtime_error) {
		throw std::string {"Oh no!"};
	};

	// prints: Error: Expected exception!
	EXPECT_THROW(std::runtime_error) {
		// nothing gets thrown
	};

};


TEST(vstl_signal) {

	// the test must end in 1 second
	TIMEOUT(1);

	// you can only expect predefined signals
	// by default VSTL handles SIGSEGV, SIGILL, SIGFPE, SIGABRT, and SIGTERM
	EXPECT_SIGNAL(SIGSEGV) {
		int* ptr = nullptr;
		*ptr = 42;
	};

	std::this_thread::sleep_for(std::chrono::milliseconds(1500));

};


TEST(vstl_fault) {

	// VSTL can also detect and recover from SEGFAULT
	// at least usually and on most platform
	// some issues may arise on Windows

	// Error: Received SIGSEGV while trying to access: 0x0!
	int* ptr = nullptr;
	*ptr = 42;

};


TEST(vstl_skip) {

	// If you want to skip the test you can use the SKIP macro
	// the test will not count as successful (nor failed) and will print the status "skipped"
	// the reason will not be shown by default but you can make VSTL show it
	// by adding `#define VSTL_PRINT_SKIP_MESSAGES true` before the VSTL include
	SKIP("I don't feel like testing rn");
};


TEST(vstl_integer_hex) {

	int a = -0xFF;

	// VSTL will deduce those values are hexadecimal and show the
	// value of 'a' in hexadecimal notation. This is done by looking at
	// the string values of both sides (if any of them is a hex number)
	CHECK(a, 0xAB);

};


TEST(vstl_integer_bin) {

	int a = 0b101;

	// VSTL will deduce those values are binary and show the
	// value of 'a' in binary notation. This is done by looking at
	// the string values of both sides (if any of them is a binary number)
	CHECK(a, 0b101000);

};


TEST(vstl_integer_dec) {

	int a = 100;

	// ofc in most cases normal base 10 notation will be used
	CHECK(a, 200);

};


TEST(vstl_integer_oct) {

	int a = 777;

	// common misstate (for me) is to use decimal when I type file permissions
	CHECK(a, 0777);

};


TEST(vstl_vector) {

	std::vector values = {1, 2, 3, 4, 5, 6, 7, 8};
	std::vector expected = {1, 2, 3, 4, 5, 6, 8, 7};

	CHECK(values, expected);

};


TEST(vstl_map) {

	std::unordered_map<int, int> values = {{0, 3}, {1, 1}, {2, 5}};
	std::unordered_map<int, int> expected = {{0, 3}, {1, 1}, {2, 4}};

	CHECK(values, expected);

}


TEST(vstl_final) {

	// only test here that is successful
	// happy testing!

};
