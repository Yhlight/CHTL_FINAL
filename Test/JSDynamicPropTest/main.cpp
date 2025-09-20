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
    std::string file_path = "Test/JSDynamicPropTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    auto config = std::make_shared<CHTL::Configuration>();

    // 1. Scanner
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    // 2. Dispatcher
    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    // 3. Assert HTML
    // The target div should have a new ID
    assert(contains(result.html, "<divid=\"chtl-dyn-0\""));

    // 4. Assert JS
    // The generated JS should contain the MutationObserver logic
    assert(contains(result.js, "constsource=document.querySelector('#sourceBox');"));
    assert(contains(result.js, "consttarget=document.getElementById('chtl-dyn-0');"));
    assert(contains(result.js, "constobserver=newMutationObserver(updateStyle);"));
    assert(contains(result.js, "observer.observe(source,{attributes:true,attributeFilter:['style']});"));

    std::cout << "JSDynamicPropTest PASSED!" << std::endl;

    return 0;
}
