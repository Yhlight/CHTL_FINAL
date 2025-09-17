#include "../Scanner/UnifiedScanner.h"
#include "../CompilerDispatcher/CompilerDispatcher.h"
#include <iostream>

int main() {
    std::string source = R"(
div {
    text: "This is CHTL.";
}

script {
    const x = 10;
    console.log("This is JS, but will be treated as CHTL_JS for now.");
}

p {
    text: "More CHTL here.";
}
)";

    try {
        std::cout << "--- Compiling with Scanner and Dispatcher ---" << std::endl;

        // In a real scenario, Preprocessor would run first.
        // For this test, we have no imports.

        UnifiedScanner scanner;
        std::vector<CodeSnippet> snippets = scanner.scan(source);

        CompilerDispatcher dispatcher;
        std::string finalHtml = dispatcher.dispatch(snippets);

        std::cout << "--- Generated HTML ---" << std::endl;
        std::cout << finalHtml << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Compilation failed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
