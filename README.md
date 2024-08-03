## VSTL
Very Simple Test Library

```
#include "vstl.hpp"

BEGIN(VSTL_MODE_LENIENT)

TEST(hello_vstl) {

	std::vector<int> vec {1, 2, 3, 4, 5, 6, 7, 8};

	// use the CHECK macro to assert equality
	CHECK(vec[0], 1);
	CHECK(vec[2], 3);
	CHECK(vec[3], 4);

	// oh no! will print this message:
	// Error: Expected 2 to be equal 4, vec[1] != 4, on line 22!
	CHECK(vec[1], 4);

	// to learn more see `demo.cpp`

};
```

### Documentation
See the `vstl.hpp` header file and
the attached usage example `demo.cpp`.

### License
MIT, see `vstl.hpp`
