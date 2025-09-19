#include "../../Scanner/UnifiedScanner.h"
#include "../../CompilerDispatcher/CompilerDispatcher.h"
#include "../../CodeMerger/CodeMerger.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>

// Helper to remove all whitespace from a string
void removeWhitespace(std::string& str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
}

int main() {
    std::string entry_point = "Test/ScriptTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(entry_point);

    std::cout << "--- Input CHTL from " << entry_point << " ---\n" << source << "\n------------------\n" << std::endl;

    try {
        CHTL::UnifiedScanner scanner(source);
        CHTL::ScanResult scan_result = scanner.scan();
        CHTL::CodeMerger merger;
        CHTL::CompilerDispatcher dispatcher(merger);
        dispatcher.dispatch(scan_result, entry_point);
        CHTL::CompilationResult result = merger.getResult();

        std::cout << "--- Generated HTML ---\n" << result.html << "\n----------------------\n" << std::endl;
        std::cout << "--- Generated JS ---\n" << result.js << "\n--------------------\n" << std::endl;

        std::string expected_html = "<body><div id=\"myDiv\"></div></body>";
        std::string expected_js = "{\n  const targets = [document.querySelector('#myDiv')];\n  const duration = 1000;\n  let startTime = null;\n  function step(timestamp) {\n    if (!startTime) startTime = timestamp;\n    const progress = Math.min((timestamp - startTime) / duration, 1);\n    targets.forEach(target => {\n    });\n    if (progress < 1) {\n      requestAnimationFrame(step);\n    }\n  }\n  requestAnimationFrame(step);\n}\n";

        std::string actual_html = result.html;
        std::string actual_js = result.js;

        removeWhitespace(expected_html);
        removeWhitespace(expected_js);
        removeWhitespace(actual_html);
        removeWhitespace(actual_js);

        assert(actual_html == expected_html);
        assert(actual_js == expected_js);

        std::cout << "Test Passed!" << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Caught a runtime_error exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
