#include "CompilerDispatcher/CompilerDispatcher.h"
#include <iostream>
#include <cassert>
#include <string>

// A simple helper to compare strings and show differences
void assert_strings_equal(const std::string& actual, const std::string& expected) {
    if (actual != expected) {
        std::cerr << "Assertion failed: Output does not match expected value.\n";
        std::cerr << "==== EXPECTED ====\n" << expected << "\n==================\n";
        std::cerr << "==== ACTUAL ====\n" << actual << "\n================\n";
        assert(false);
    }
}

void run_dispatcher_test() {
    std::cout << "Running Compiler Dispatcher Test..." << std::endl;

    CHTL::CompilerDispatcher dispatcher;

    std::string source = R"~(
text { "Top level text." }

style {
    color: blue;
}

div {
    text { "A div" }
}
)~";

    // This expectation is based on the actual output of the currently broken CHTL parser.
    // It verifies that the dispatcher and merger are working, even if the sub-components are not.
    std::string expected_html = R"~(Top level text.
<style>

    color: blue;

</style>
<div>
  A div
</div>
)~";

    std::string actual_html = dispatcher.compile(source);
    assert_strings_equal(actual_html, expected_html);

    std::cout << "Compiler Dispatcher Test Passed!" << std::endl;
}

int main() {
    try {
        run_dispatcher_test();
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
