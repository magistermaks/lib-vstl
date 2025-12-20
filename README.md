## VSTL
Very Simple Test Library

```c++
#include "vstl.hpp"

TEST(hello_vstl) {

	std::vector<int> vec {1, 2, 3, 4, 5, 6, 7, 8};

	// use the CHECK macro to assert equality
	CHECK(vec[0], 1);
	CHECK(vec[2], 3);
	CHECK(vec[3], 4);

	// oh no! will print this message:
	// Error: Expected 2 to be equal 4, vec[1] != 4, on line 20!
	CHECK(vec[1], 4);

	// to learn more see `demo.cpp`

};
```

VSTL demo (`demo.cpp` & `demo-module.cpp`), with `VSTL_USE_ANSI` and `VSTL_PRINT_TIME` enabled (the demo has them disabled, as it is used for VSTL self-testing,
by default those two options are enabled)
<pre>$ ./vstl_demo 
<b>Module: demo-module.cpp</b>
Test &apos;vstl_submodule&apos; <font color="#33D17A"><b>passed</b></font>!

<b>Module: demo.cpp</b>
Test &apos;vstl_check&apos; <font color="#F66151"><b>failed</b></font>! Error: Expected <font color="#F66151"><b>2</b></font> to be equal <font color="#F66151"><b>4</b></font>, vec[1] != 4, on line <b>25</b>!
Test &apos;vstl_fail&apos; <font color="#F66151"><b>failed</b></font>! Error: Oops, on line <b>40</b>!
Test &apos;vstl_assert&apos; <font color="#F66151"><b>failed</b></font>! Error: Thief, on line <b>61</b>!
Test &apos;vstl_expect&apos; <font color="#F66151"><b>failed</b></font>! Error: Expected exception of type std::runtime_error, on line <b>88</b>!
Test &apos;vstl_signal&apos; <font color="#F66151"><b>failed</b></font>! Timeout reached!
Test &apos;vstl_fault&apos; <font color="#F66151"><b>failed</b></font>! Error: Received SIGSEGV (#11) while trying to access: 0x0!
Test &apos;vstl_skip&apos; <font color="#E9AD0C"><b>skipped</b></font>! I don&apos;t feel like testing rn, on line <b>136</b>!
Test &apos;vstl_integer_hex&apos; <font color="#F66151"><b>failed</b></font>! Error: Expected <font color="#F66151"><b>0xFFFFFF01 (decimal -255)</b></font> to be equal <font color="#F66151"><b>0xAB (decimal 171)</b></font>, a != 0xAB, on line <b>147</b>!
Test &apos;vstl_integer_bin&apos; <font color="#F66151"><b>failed</b></font>! Error: Expected <font color="#F66151"><b>0b101 (decimal 5)</b></font> to be equal <font color="#F66151"><b>0b101000 (decimal 40)</b></font>, a != 0b101000, on line <b>159</b>!
Test &apos;vstl_integer_dec&apos; <font color="#F66151"><b>failed</b></font>! Error: Expected <font color="#F66151"><b>100</b></font> to be equal <font color="#F66151"><b>200</b></font>, a != 200, on line <b>169</b>!
Test &apos;vstl_integer_oct&apos; <font color="#F66151"><b>failed</b></font>! Error: Expected <font color="#F66151"><b>01411 (decimal 777)</b></font> to be equal <font color="#F66151"><b>0777 (decimal 511)</b></font>, a != 0777, on line <b>179</b>!
Test &apos;vstl_vector&apos; <font color="#F66151"><b>failed</b></font>! Error: Expected <font color="#F66151"><b>[1, 2, 3, 4, 5, 6, 7, 8]</b></font> to be equal <font color="#F66151"><b>[1, 2, 3, 4, 5, 6, 8, 7]</b></font>, values != expected, on line <b>189</b>!
Test &apos;vstl_map&apos; <font color="#F66151"><b>failed</b></font>! Error: Expected <font color="#F66151"><b>[{2, 5}, {1, 1}, {0, 3}]</b></font> to be equal <font color="#F66151"><b>[{2, 4}, {1, 1}, {0, 3}]</b></font>, values != expected, on line <b>199</b>!
Test &apos;vstl_final&apos; <font color="#33D17A"><b>passed</b></font>!

Executed 15 tests, 12 failed, 2 passed. (time: 1001.15ms)
</pre>

### Documentation
See the `vstl.hpp` header file and
the attached usage example `demo.cpp`.

### License
VSTL is licensed under the MIT License, see `vstl.hpp`
