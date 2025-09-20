#include "../../CHTL/CompilerDispatcher/CompilerDispatcher.h"
#include "../../CHTL/CodeMerger/CodeMerger.h"
#include <iostream>
#include <cassert>
#include <string>

int main() {
    std::cout << "--- Running Script Test ---" << std::endl;

    std::string source = R"(
        div {
            text { "Hello" }
        }

        script {
            const message = "Hello from JS!";
            console.log(message);
        }

        style {
            body { color: blue; }
        }
    )";

    CHTL::CompilerDispatcher dispatcher(source);
    CHTL::CompilationResult result = dispatcher.dispatch();

    std::cout << "--- Generated HTML ---\n" << result.html << "\n------------------\n" << std::endl;
    std::cout << "--- Generated CSS ---\n" << result.css << "\n------------------\n" << std::endl;
    std::cout << "--- Generated JS ---\n" << result.js << "\n------------------\n" << std::endl;

    assert(result.html.find("div") != std::string::npos);
    assert(result.css.find("body") != std::string::npos);
    assert(result.js.find("const message") != std::string::npos);

    std::cout << "Assertion Passed: HTML, CSS, and JS content correctly separated." << std::endl;

    // Test the merged output
    CHTL::CodeMerger merger(result);
    std::string merged_html = merger.mergeToSingleFile();
    std::cout << "\n--- Merged HTML ---\n" << merged_html << "\n------------------\n" << std::endl;
    assert(merged_html.find("<script>") != std::string::npos);
    assert(merged_html.find("const message") != std::string::npos);
    assert(merged_html.find("</script>") != std::string::npos);

    std::cout << "Assertion Passed: JS code correctly inlined." << std::endl;

    std::cout << "\n--- Script Test Passed! ---" << std::endl;

    return 0;
}
