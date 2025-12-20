
#define VSTL_USE_ANSI false
#define VSTL_PRINT_TIME false
#define VSTL_TRIGGER_DEBUGGER false

// On POSIX this just works:tm:, on Windows we need to be explicit
// the #if is just to show that, you can safely define VSTL_SUBMODULE on linux too.
#if _WIN32
#	define VSTL_SUBMODULE true
#endif

#include "vstl.hpp"

TEST(vstl_submodule) {

	ASSERT(true);

};

TEST(vstl_string_method) {

	struct Foo {
		const char* magic = "unset";

		const char* str() const {
			return magic;
		}

		bool operator==(const Foo& other) const {
			return strcmp(magic, other.magic) == 0;
		}
	};

	Foo f1 {"hmm"};
	Foo f2 {};

	CHECK(f1, f2);

};