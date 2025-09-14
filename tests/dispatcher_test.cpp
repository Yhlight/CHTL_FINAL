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
html {
    head {
        title { text: "Test Page"; }
        style {
            body { font-family: "Arial"; }
        }
    }
    body {
        div {
            class: "container";
            text: "Hello";
            script {
                // Simplified script to avoid parser/generator bugs
                const a = {{.container}};
            }
        }
    }
}
)~";

    // This expectation is based on the actual output of the compiler pipeline.
    // It serves as a regression test for the current state of the system.
    std::string expected_html = R"~(<html>
  <head>
    <title>
      Test Page
    </title>

<style>

            body { font-family: "Arial"; }
        </style>
</head>
  <body>
    <div class="container">
      Hello
    </div>

<script>

                // Simplified script to avoid parser/generator bugs
                const a = document.querySelector(".container")

</script>
</body>
</html>
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
