
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