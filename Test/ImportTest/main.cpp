#include "../../CHTL/CompilerDispatcher/CompilerDispatcher.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>

void test_import_functionality() {
    std::cout << "\n--- Running Test: Import Functionality ---\n";
    try {
        std::string source = CHTL::FileSystem::readFile("Test/ImportTest/main.chtl");
        CHTL::CompilerDispatcher dispatcher(source);
        CHTL::FinalOutput result = dispatcher.dispatch();

        std::cout << "--- Generated HTML ---\n" << result.html << "\n----------------------\n";

        // Check that the content from the imported template is present
        assert(result.html.find("This content is from an imported file!") != std::string::npos);
        assert(result.html.find("imported-box") != std::string::npos);

        // Check that content from the main file is also present
        assert(result.html.find("Main File Content") != std::string::npos);

        std::cout << "PASS\n";

    } catch (const std::runtime_error& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        assert(false);
    }
}

int main() {
    try {
        test_import_functionality();
        std::cout << "\nAll import tests passed!\n";
    } catch (...) {
        std::cerr << "An unknown exception occurred." << std::endl;
        return 1;
    }
    return 0;
}
