
#include "vstl.hpp"
#include <stdexcept>

// each test unit must contain the one begin clause
// LENIENT means don't stop on first failed test
// you can also use VSTL_MODE_STRICT for the opposite behaviour
BEGIN(VSTL_MODE_LENIENT)

// each test begins with the TEST(name) clause
// the test names need not be unique
TEST(vstl_check) {
	std::vector<int> vec {1, 2, 3, 4, 5, 6, 7, 8};

	// use the CHECK macro to assert equality
	CHECK(vec[0], 1);
	CHECK(vec[2], 3);
	CHECK(vec[3], 4);

	// oh no! will print this message:
	// Error: Expected 2 to be equal 4, vec[1] != 4, on line 22!
	CHECK(vec[1], 4);

	// remember to put ';' at the end, a TEST is not a function!
};


TEST(vstl_fail) {

	// you can use the FAIL macro to manually
	// fail the test with a custom message, like this:

	if (bool oops = true) {

		// oh no! will print this message:
		// Error: Oops, on line 37
		FAIL("Oops");
	}

};


TEST(vstl_assert) {

	// there are also simple ASSERT and
	// ASSERT_MSG macros avaible, if CHECK is not your cup of tea

	int a = 21;
	int b = 42;

	ASSERT(a * 2 == b);
	ASSERT_MSG(a == b / 2, "Joker");

	// prints: Expected a * 3 == b to be true, but it was not, on line 54!
	// ASSERT(a * 3 == b);

	// prints: Error: Thief, on line 58!
	ASSERT_MSG(a == b / 3, "Thief");

};


TEST(vstl_expect) {

	// you can also assert that an exception
	// should be thrown in some block,
	// either of any or some specific type

	// check if any exception was thrown
	EXPECT_ANY({
		throw "oh my!";
	});

	// check if runtime_error was thrown
	EXPECT(std::runtime_error, {
		throw std::runtime_error {"Error of a runtime type"};
	});

	// any exception type is valid
	EXPECT(int, {
		throw 42;
	});

	// prints: Error: Expected exception of type std::runtime_error, on line 86!
	EXPECT(std::runtime_error, {
		throw std::string {"Oh no!"};
	});

	// prints: Error: Expected exception, on line 91!
	EXPECT(std::runtime_error, {
		// nothing gets thrown
	});

};


TEST(vstl_fault) {

	// VSTL can also detect and recover from SEGFAULT
	// at least usually and on most platform
	// some issues may arise on Windows

	// Error: Received SIGSEGV while trying to access: 0x0!
	int* ptr = nullptr;
	*ptr = 42;

};


TEST(vstl_final) {

	// mostly a demonstration that the previous
	// test did not crash the program, (or maybe that it did)
	// also the only test here that dosn't fail

	// happy testing!

};
