
#include "vstl.hpp"
#include "args.hpp"

#define VSTL_UNKNOWN_SIGNAL_NAME "unknown signal"

namespace vstl {

	static thread_local volatile int received_signal_number;
	static thread_local volatile uintptr_t received_signal_address;
	static thread_local volatile bool received_timer_alarm = false;

	static std::vector<Test> tests;
	static std::vector<Handler> handlers;

	static void init();
	static void catch_signal(int signum);
	static const char* get_signal_name(int signum);
	static std::string get_signal_message(int signum, uint64_t address, bool include_address);
	static std::string get_received_signal_message();

	/*
	 * Implementation of local static functions
	 */

	static std::string get_signal_message(int signum, uintptr_t address, bool include_address) {
		std::stringstream ss;

		ss << "Received " << get_signal_name(signum) << " ";
		ss << "(#" + std::to_string(signum) + ")";

		if (include_address) {
			ss << " while trying to access memory address: 0x" << std::hex << address;
		}

		ss << "!";

		return ss.str();
	}

	static void print_summary(const auto& time, int passed, int skipped, int failed) {
		size_t executed = failed + passed + skipped;
		double millis = std::chrono::duration<double, std::milli>(time).count();

		std::stringstream out;
		out << std::endl << std::dec << "Executed " << executed << " ";
		out << (executed == 1 ? "test" : "tests") << ", ";
		out << failed << " failed, ";
		out << passed << " passed.";

		if (config_print_time) {
			out << " (time: " << millis << "ms)";
		}

		auto string = out.str();
		printf("%s\n", string.c_str());
	}

	static void shared_signal_handler(int signum) {
		if (expected_signal == signum) {
			expected_signal = 0;
			VSTL_JMP_SIG(expect_jmp);
		}
	}

	static Result test_once(Test& test) {

		// seed both the per-test random generator and the global C one
		test.random.set_seed(config_seed);
		srand(config_seed);

		// if we go into this then a deadly signal was raised during test execution
		if (VSTL_JMP_SET(jmp)) {
			return {FAILED, get_received_signal_message()};
		}

		try {
			test.func(test);
		} catch (const TestSkip& skip) {
			return {SKIPPED, skip.what()};
		} catch (const TestError& e) {
			return {FAILED, std::string("Error: ") + e.what()};
		} catch (const std::exception& e) {
			return {FAILED, std::string("Exception: ") + e.what()};
		} catch (...) {
			std::exception_ptr ptr = std::current_exception();

			// try to convert the error using the defined error handlers
			for (const Handler& handler : handlers) {
				try {
					handler.call(ptr);
				} catch(const TestError& e) {
					return {FAILED, std::string("Error: ") + e.what()};
				} catch (...) {
					// ignore
				}
			}

			std::string error {"Error: "};

			// everything has failed us, just try to print *some* reason
			try {
				std::rethrow_exception(ptr);
			} catch (const char* e) {
				return {FAILED, "String " + error + e};
			} catch (const std::string& e) {
				return {FAILED, "String " + error + e};
			} catch (int16_t e) {
				return {FAILED, "Integer " + error + std::to_string(e)};
			} catch (int32_t e) {
				return {FAILED, "Integer " + error + std::to_string(e)};
			} catch (...) {
				return {FAILED, "Unknown exception thrown!"};
			}
		}

		// remove timeout
		set_timeout(0);

		// sanity check, in some extreme cases, when the program gets corrupted VSTL itself can crash
		// we set this so we don't then fall back into running a test but exit, somewhat, gracefully
		if (VSTL_JMP_SET(jmp)) {
			printf("VSTL fatal error occurred!\n");
			exit(1);
		}

		return {PASSED};

	}

	static Result test_times(Test& test, int count) {
		for (int i = 0; i < count; i ++) {
			const Result res = test_once(test);

			// stop as soon as something is wrong
			if (res.status != PASSED) {
				return res;
			}
		}

		return {PASSED};
	}

	static bool run_tests(std::vector<Test>& tests) {

		// if no seed is set pick one at random
		if (config_seed == 0) {
			std::random_device dev;
			config_seed = dev();
		}

		printf("Running with seed: %lu\n\n", config_seed);

		const char* str_passed = config_print_color ? VSTL_COLOR_PASSED : "passed";
		const char* str_skipped = config_print_color ? VSTL_COLOR_SKIPPED : "skipped";
		const char* str_failed = config_print_color ? VSTL_COLOR_FAILED : "failed";
		const char* str_bold = config_print_color ? VSTL_COLOR_BOLD : "";
		const char* str_reset = config_print_color ? VSTL_COLOR_RESET : "";

		const auto start = std::chrono::steady_clock::now();
		const char* current_module = "";

		bool first = true;
		int passed = 0;
		int failed = 0;
		int skipped = 0;

		// order of static constructor initialization is not guaranteed between compilation units
		// so that the order of tests sources from multiple modules (units) is constant and deterministic
		// we sort them by the file path of their origin and line.
		std::sort(tests.begin(), tests.end(), [] (const Test& a, const Test& b) {
			int srt = std::strcmp(a.meta.module, b.meta.module);

			if (srt < 0) return true;
			if (srt > 0) return false;

			return a.meta.line - b.meta.line < 0;
		});

		for (Test& test : tests) {

			if (config_print_modules && std::strcmp(current_module, test.meta.module) != 0) {
				auto module_path = std::filesystem::relative(test.meta.module);
				auto module_name = module_path.filename().u8string();

				if (!first) {
					printf("\n");
				}

				printf("%sModule: %s%s\n", str_bold, (char*) module_name.c_str(), str_reset);
				current_module = test.meta.module;
			}

			first = false;
			Result res = test_times(test, config_repeats);

			if (config_print_passed && res.status == PASSED) {
				printf("Test '%s' %s!\n", test.meta.name, str_passed);
				passed ++;
			}

			if (config_print_skip && res.status == SKIPPED) {
				printf("Test '%s' %s! %s\n", test.meta.name, str_skipped, res.message.c_str());
				skipped ++;
			}

			if (res.status == FAILED) {
				printf("Test '%s' %s! %s\n", test.meta.name, str_failed, res.message.c_str());
				failed ++;
			}
		}

		print_summary(std::chrono::steady_clock::now() - start, passed, skipped, failed);
		return failed == 0;
	}

	static void load_config(const ProgramArguments& args) {

		if (args.get_string("help").has_value()) {
			printf("Usage: %s [OPTIONS]\n", args.program().c_str());
			printf("VSTL test application, %lu tests loaded\n\n", tests.size());

			auto btos = [] (bool flag) -> const char* {
				return flag ? "true" : "false";
			};

			printf("OPTIONS:\n");
			printf(" --seed             <int>  [= %" PRIu64 "] \n", config_seed);
			printf(" --repeats          <int>  [= %d] \n", config_repeats);
			printf(" --trigger-debugger <bool> [= %s] \n", btos(config_trigger_debugger));
			printf(" --print-time       <bool> [= %s] \n", btos(config_print_time));
			printf(" --print-skip       <bool> [= %s] \n", btos(config_print_skip));
			printf(" --print-passed     <bool> [= %s] \n", btos(config_print_passed));
			printf(" --print-modules    <bool> [= %s] \n", btos(config_print_modules));
			printf(" --print-color      <bool> [= %s] \n", btos(config_print_color));

			exit(0);
		}

		config_seed = args.get_int("seed").value_or(config_seed);
		config_repeats = args.get_int("repeats").value_or(config_repeats);
		config_print_time = args.get_bool("print-time").value_or(config_print_time);
		config_print_skip = args.get_bool("print-skip").value_or(config_print_skip);
		config_print_passed = args.get_bool("print-passed").value_or(config_print_passed);
		config_trigger_debugger = args.get_bool("trigger-debugger").value_or(config_trigger_debugger);
		config_print_modules = args.get_bool("print-modules").value_or(config_print_modules);
		config_print_color = args.get_bool("print-color").value_or(config_print_color);
	}

	/*
	 * General Platform-Independent API Implementation
	 */

	print_hint guess_print_hint(const std::string_view& one, const std::string_view& two) {
		print_hint hints {};

		if (one.starts_with("0x") || two.starts_with("0x")) hints.base = 16;
		else if (one.starts_with("0b") || two.starts_with("0b")) hints.base = 2;
		else if (one.starts_with('0') || two.starts_with('0')) hints.base = 8;

		if (hints.base == 16) {
			auto check_letter_case = [&] (char c) {
				if (c >= 'A' && c <= 'Z') hints.uppercase = true;
			};

			for (const char c : one) check_letter_case(c);
			for (const char c : two) check_letter_case(c);
		}

		return hints;
	}

	size_t get_longest_test_name() {
		size_t length = 0;

		for (const Test& test : tests) {
			const size_t size = test.meta.length();

			if (size > length) {
				length = size;
			}
		}

		return length;
	}

	void register_test(const Test& test) {
		tests.emplace_back(test);
	}

	void register_handler(const Handler& handler) {
		handlers.emplace_back(handler);
	}

	std::string color(const char* escape) {
		return config_print_color ? escape : "";
	}

	/*
	 * region Microsoft Windows Implementation
	 */

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <debugapi.h>
#include <errhandlingapi.h>

	static void signal_handler(int sig) {
		received_timer_alarm = false;
		shared_signal_handler(sig);
		received_signal_number = sig;
		VSTL_JMP_SIG(jmp);
	}

	static LONG WINAPI vector_exception_handler(EXCEPTION_POINTERS* info) {
		if (info->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
			signal_handler(SIGSEGV);
		}

		return EXCEPTION_CONTINUE_SEARCH;
	}

	void trap() {
		DebugBreak();
	}

	void init() {
		AddVectoredExceptionHandler(1, vector_exception_handler);

		catch_signal(SIGSEGV);
		catch_signal(SIGILL);
		catch_signal(SIGFPE);
		catch_signal(SIGABRT);
		catch_signal(SIGTERM);
	}

	void set_timeout(size_t milliseconds) {
		// TODO: Unimplemented on windows
	}

	static const char* get_signal_name(int signum) {
		if (signum == SIGSEGV) return "SIGSEGV";
		if (signum == SIGILL) return "SIGILL";
		if (signum == SIGFPE) return "SIGFPE";
		if (signum == SIGABRT) return "SIGABRT";
		if (signum == SIGTERM) return "SIGTERM";
		// SIGBUS is unsupported
		// SIGTRAP is unsupported

		return VSTL_UNKNOWN_SIGNAL_NAME;
	}

	static std::string get_received_signal_message() {
		return get_signal_message(received_signal_number, 0, false); // we don't have the address available on Windows
	}

	static void catch_signal(int signum) {
		signal(signum, signal_handler);
	}

#endif

	/*
	 * region POSIX Implementation
	 */

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#include <sys/time.h>

	void trap() {
		raise(SIGTRAP);
	}

	void init() {

		// custom stack to make VSTL more resilient if the stack pointer gets corrupted
		stack_t stack;
		stack.ss_sp = malloc(SIGSTKSZ);
		stack.ss_size = SIGSTKSZ;
		stack.ss_flags = 0;
		sigaltstack(&stack, nullptr);

		catch_signal(SIGALRM);
		catch_signal(SIGBUS);

		if (config_trigger_debugger) {
			signal(SIGTRAP, SIG_IGN);
		}

		catch_signal(SIGSEGV);
		catch_signal(SIGILL);
		catch_signal(SIGFPE);
		catch_signal(SIGABRT);
		catch_signal(SIGTERM);
	}

	void set_timeout(size_t milliseconds) {
		size_t seconds = milliseconds / 1000;
		size_t reminder = milliseconds % 1000;
		size_t microseconds = reminder * 1000;

		itimerval timer {};
		timer.it_interval.tv_sec = 0;
		timer.it_interval.tv_usec = 0;
		timer.it_value.tv_sec = static_cast<long>(seconds);
		timer.it_value.tv_usec = static_cast<long>(microseconds);

		setitimer(ITIMER_REAL, &timer, nullptr);
	}

	static const char* get_signal_name(int signum) {
		if (signum == SIGSEGV) return "SIGSEGV";
		if (signum == SIGILL) return "SIGILL";
		if (signum == SIGFPE) return "SIGFPE";
		if (signum == SIGABRT) return "SIGABRT";
		if (signum == SIGTERM) return "SIGTERM";
		if (signum == SIGBUS) return "SIGBUS";
		if (signum == SIGTRAP) return "SIGTRAP";
		if (signum == SIGALRM) return "SIGALRM";

		return VSTL_UNKNOWN_SIGNAL_NAME;
	}

	static std::string get_received_signal_message() {
		if (received_timer_alarm) {
			return "Timeout reached!";
		}

		return get_signal_message(received_signal_number, received_signal_address, true);
	}

	static void signal_handler(int sig, siginfo_t* si, void* unused) {
		received_timer_alarm = (sig == SIGALRM);
		shared_signal_handler(sig);
		received_signal_number = sig;
		received_signal_address = reinterpret_cast<uintptr_t>(si->si_addr);
		VSTL_JMP_SIG(jmp);
	}

	static void catch_signal(int signum) {

		// our signal action description
		struct sigaction action {};
		action.sa_flags = SA_SIGINFO;
		action.sa_flags = SA_SIGINFO | SA_NODEFER | SA_ONSTACK;
		sigemptyset(&action.sa_mask);
		action.sa_sigaction = signal_handler;

		sigaction(signum, &action, nullptr);
	}

#endif
}

/*
 * Entrypoint
 */

int main(int argc, const char* argv[]) {
	ProgramArguments args {argc, argv};
	vstl::load_config(args);

	vstl::init();
	return vstl::run_tests(vstl::tests) ? 0 : 1;
}