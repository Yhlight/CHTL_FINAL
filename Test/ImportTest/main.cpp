#include "../../CHTL/CompilerDispatcher.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <memory>

int main() {
    std::string entry_point = "Test/ImportTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(entry_point);

    if (source.empty()) {
        std::cerr << "Failed to read test file: " << entry_point << std::endl;
        return 1;
    }

    std::cout << "--- Input CHTL from " << entry_point << " ---\n" << source << "\n------------------\n" << std::endl;

    try {
        // Use the full CompilerDispatcher now that the architecture is refactored
        auto config = std::make_shared<CHTL::Configuration>();
        CHTL::CompilerDispatcher dispatcher(config, entry_point);
        CHTL::FinalCompilationResult result = dispatcher.dispatch(source);

        std::cout << "--- Generated HTML ---\n" << result.html << "\n----------------------\n" << std::endl;
        std::cout << "--- Generated CSS ---\n" << result.css << "\n---------------------\n" << std::endl;
        std::cout << "--- Generated JS ---\n" << result.js << "\n--------------------\n" << std::endl;

        // Basic check for success
        if (result.html.find("This is the imported box.") != std::string::npos) {
            std::cout << "Test Passed: Imported content was found in the output." << std::endl;
        } else {
            std::cerr << "Test Failed: Imported content was NOT found in the output." << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Caught an exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
