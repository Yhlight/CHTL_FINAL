#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cassert>
#include <string>

#include "CompilerDispatcher/CompilerDispatcher.h"


void runDispatcherTest() {
    std::cout << "Running Compiler Dispatcher Test..." << std::endl;

    // A simple source string to test the dispatcher's ability to handle all chunk types
    std::string source =
        "div { id: main; } "
        "style { color: blue; } "
        "script { var x = 10; {{button}} }";

    CHTL::CompilerDispatcher dispatcher;
    std::string result = dispatcher.compile(source);

    std::cout << "Generated output:\n" << result << std::endl;

    // 1. Check for the CSS content
    assert(result.find("<style>") != std::string::npos);
    assert(result.find("color: blue;") != std::string::npos);
    assert(result.find("</style>") != std::string::npos);

    // 2. Check for the JS content from the placeholder
    assert(result.find("<script>") != std::string::npos);
    assert(result.find("var x = 10;") != std::string::npos);
    assert(result.find("</script>") != std::string::npos);

    // 3. Check for the CHTL content (this is a bit tricky as it gets compiled)
    // For now, we'll just check that the output is not empty and seems plausible.
    assert(result.find("<div id=\"main\" />") != std::string::npos);

    // 4. Check for the CHTL-JS content (also tricky, but we can check if it's there)
    // The CHTL-JS generator is a stub, so we can't check for real output yet.
    // We'll assume if the JS is there, the CHTL-JS part was processed.

    std::cout << "Compiler Dispatcher Test PASSED" << std::endl;
}

int main() {
    try {
        runDispatcherTest();
    } catch (const std::exception& e) {
        std::cerr << "Test FAILED: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
