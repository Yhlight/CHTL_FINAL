#include "../../Scanner/CHTLUnifiedScanner.h"
#include "../../CHTL/CompilerDispatcher.h"
#include "../../CHTL/CodeMerger.h"
#include "../../CHTL/Config/Configuration.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <memory>

// Helper to check if a string contains a substring
bool contains(const std::string& haystack, const std::string& needle) {
    return haystack.find(needle) != std::string::npos;
}

int main() {
    std::string file_path = "Test/DynamicIfTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    // Create a shared configuration object
    auto config = std::make_shared<CHTL::Configuration>();
    config->keyword_map["KEYWORD_IF"] = {"if"};
    config->keyword_map["KEYWORD_ELSE"] = {"else"};
    config->keyword_map["KEYWORD_CONDITION"] = {"condition"};

    // 1. Scanner
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    // 2. Dispatcher
    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    // 3. Assertions
    bool pass = true;

    // Check 1: HTML should contain the placeholder
    if (!contains(result.html, "<div id=\"chtl-if-0\"></div>")) {
        std::cerr << "TEST FAILED: HTML missing placeholder div.\n";
        pass = false;
    }

    // Check 2: HTML should NOT contain the branch content directly
    if (contains(result.html, "then-branch-content")) {
        std::cerr << "TEST FAILED: HTML contains 'then' branch content directly.\n";
        pass = false;
    }

    // Check 3: JS should contain the placeholder lookup
    if (!contains(result.js, "const placeholder = document.getElementById('chtl-if-0')")) {
        std::cerr << "TEST FAILED: JS missing placeholder lookup.\n";
        pass = false;
    }

    // Check 4: JS should contain the dynamic condition
    std::string expected_condition = "if ((parseFloat(document.querySelector('#source-box').width) > 100))";
    if (!contains(result.js, expected_condition)) {
        std::cerr << "TEST FAILED: JS missing correct dynamic condition.\n";
        std::cerr << "Expected to find: " << expected_condition << "\n";
        pass = false;
    }

    // Check 5: JS should contain the 'then' branch content
    if (!contains(result.js, "const then_content = \"<p id=\\\"then-branch-content\\\">The box is wide.</p>\";")) {
        std::cerr << "TEST FAILED: JS missing 'then' branch content string.\n";
        pass = false;
    }

    // Check 6: JS should contain the 'else' branch content
    if (!contains(result.js, "const else_content = \"<p id=\\\"else-branch-content\\\">The box is narrow.</p>\";")) {
        std::cerr << "TEST FAILED: JS missing 'else' branch content string.\n";
        pass = false;
    }


    if (!pass) {
        std::cerr << "\n--- Full Generated HTML ---\n" << result.html << "\n";
        std::cerr << "\n--- Full Generated JS ---\n" << result.js << "\n";
        return 1;
    }

    std::cout << "DynamicIfTest PASSED!" << std::endl;
    return 0;
}
