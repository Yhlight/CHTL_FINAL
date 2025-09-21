#include "../../Scanner/CHTLUnifiedScanner.h"
#include "../../CHTL/CompilerDispatcher.h"
#include "../../CHTL/Config/Configuration.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>
#include <memory>

// Helper function to check if a string contains a substring
bool contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

int main() {
    std::string file_path = "Test/DynamicConditionalRenderTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    auto config = std::make_shared<CHTL::Configuration>();
    config->keyword_map["KEYWORD_IF"] = {"if"};

    // 1. Scanner
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    // 2. Dispatcher
    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    // 3. Assert
    bool pass = true;
    std::string& js = result.js;

    if (!contains(js, "const parentEl = document.getElementById('target');")) {
        std::cerr << "DynamicConditionalRenderTest FAILED: JS does not get target element by ID." << std::endl;
        pass = false;
    }
    if (!contains(js, "const sourceEl = document.querySelector('#source');")) {
        std::cerr << "DynamicConditionalRenderTest FAILED: JS does not get source element by selector." << std::endl;
        pass = false;
    }
    if (!contains(js, "const observer = new MutationObserver(")) {
        std::cerr << "DynamicConditionalRenderTest FAILED: JS does not create a MutationObserver." << std::endl;
        pass = false;
    }
    if (!contains(js, "const condition = sourceValue > 100;")) {
        std::cerr << "DynamicConditionalRenderTest FAILED: JS does not perform the correct comparison." << std::endl;
        pass = false;
    }
    if (!contains(js, "parentEl.style.display = condition ? 'none' : '';")) {
        std::cerr << "DynamicConditionalRenderTest FAILED: JS does not correctly toggle the display style." << std::endl;
        pass = false;
    }
     if (!contains(js, "observer.observe(sourceEl, { attributes: true, subtree: true });")) {
        std::cerr << "DynamicConditionalRenderTest FAILED: JS does not observe the source element." << std::endl;
        pass = false;
    }


    if (!pass) {
        std::cerr << "Full generated JS: \n" << js << std::endl;
        return 1;
    }


    std::cout << "DynamicConditionalRenderTest PASSED!" << std::endl;

    return 0;
}
