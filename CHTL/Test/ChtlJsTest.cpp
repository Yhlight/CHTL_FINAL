#include "../Scanner/UnifiedScanner.h"
#include "../CompilerDispatcher/CompilerDispatcher.h"
#include <iostream>

int main() {
    std::string source = R"(
div {
    class: my-div;
}

script {
    const myDiv = {{.my-div}};
    myDiv.textContent = "Hello from CHTL JS!";
}
)";

    try {
        std::cout << "--- Compiling CHTL JS ---" << std::endl;

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
