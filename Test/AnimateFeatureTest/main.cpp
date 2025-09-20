#include "../../Scanner/CHTLUnifiedScanner.h"
#include "../../CHTL/CompilerDispatcher.h"
#include "../../CHTL/Config/Configuration.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <memory>

// Helper to check if a substring exists, ignoring whitespace
bool contains_ignore_whitespace(const std::string& str, const std::string& sub) {
    std::string str_no_space;
    for (char c : str) {
        if (!std::isspace(c)) str_no_space += c;
    }
    std::string sub_no_space;
    for (char c : sub) {
        if (!std::isspace(c)) sub_no_space += c;
    }
    return str_no_space.find(sub_no_space) != std::string::npos;
}

int main() {
    std::string file_path = "Test/AnimateFeatureTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);
    auto config = std::make_shared<CHTL::Configuration>();

    // Run the compiler
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    std::cout << "Generated JS:\n" << result.js << std::endl;

    // --- Verification ---
    assert(contains_ignore_whitespace(result.js, "const targets = []"));
    assert(contains_ignore_whitespace(result.js, "targets.push(...document.querySelectorAll('#box'))"));
    assert(contains_ignore_whitespace(result.js, "const duration = 1000"));
    assert(contains_ignore_whitespace(result.js, "const beginState = {'opacity':'0','width':'100px'}"));
    assert(contains_ignore_whitespace(result.js, "const endState = {'opacity':'1','width':'200px'}"));
    assert(contains_ignore_whitespace(result.js, "requestAnimationFrame(step)"));
    assert(contains_ignore_whitespace(result.js, "const progress = Math.min((timestamp - startTime) / duration, 1)"));
    assert(contains_ignore_whitespace(result.js, "target.style[prop] = (start + (end - start) * progress) + unit;"));

    std::cout << "AnimateFeatureTest PASSED!" << std::endl;

    return 0;
}
