#include "../../Scanner/CHTLUnifiedScanner.h"
#include "../../CHTL/CompilerDispatcher.h"
#include "../../CHTL/CodeMerger.h"
#include "../../CHTL/Config/Configuration.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>
#include <memory>
#include <vector>

// Helper function to check if a string contains a substring
bool contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

int main() {
    std::string file_path = "Test/AnimateTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);
    auto config = std::make_shared<CHTL::Configuration>();

    std::vector<CHTL::CodeFragment> fragments = {{source, CHTL::FragmentType::CHTL}};

    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    std::cout << "Generated JS: " << result.js << std::endl;

    // Assert that the generated JS contains the key parts of the animation
    assert(contains(result.js, "const targets = [document.querySelector('#animated-box')]"));
    assert(contains(result.js, "const duration = 1000"));
    assert(contains(result.js, "const progress = Math.min((timestamp - startTime) / duration, 1)"));
    assert(contains(result.js, "target.style.opacity = (0 + (1 - 0) * progress) + 'px'"));
    assert(contains(result.js, "requestAnimationFrame(step)"));

    std::cout << "AnimateTest PASSED!" << std::endl;

    return 0;
}
