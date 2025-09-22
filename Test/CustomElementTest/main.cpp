#include "../../Scanner/CHTLUnifiedScanner.h"
#include "../../CHTL/CompilerDispatcher.h"
#include "../../CHTL/Config/Configuration.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>
#include <memory>

// Helper function to remove all whitespace from a string for robust comparison
std::string remove_whitespace(const std::string& str) {
    std::string out;
    out.reserve(str.size());
    for (char c : str) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            out += c;
        }
    }
    return out;
}

int main() {
    std::string file_path = "Test/CustomElementTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    auto config = std::make_shared<CHTL::Configuration>();

    // 1. Scanner
    CHTL::CHTLUnifiedScanner scanner(source, config);
    auto fragments = scanner.scan();

    // 2. Dispatcher
    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    // 3. Assert
    std::string expected_html = R"(
        <html>
            <body>
                <div class="card" style="border:1pxsolid#ccc;border-radius:8px;">
                    <h2 class="card-title">Specialized Title</h2>
                    <div class="card-footer">
                        <button class="btn">Click Me</button>
                    </div>
                </div>
            </body>
        </html>
    )";
    // Note: The original <p> tag is gone, the <h2> text is changed,
    // the button is added, and the outer div has new styles.

    std::string processed_html_result = remove_whitespace(result.html);
    std::string processed_html_expected = remove_whitespace(expected_html);

    if (processed_html_result != processed_html_expected) {
        std::cerr << "CustomElementTest FAILED!" << std::endl;
        std::cerr << "Expected: " << processed_html_expected << std::endl;
        std::cerr << "Got: " << processed_html_result << std::endl;
        return 1;
    }

    std::cout << "CustomElementTest PASSED!" << std::endl;

    return 0;
}
