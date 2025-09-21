#include "../../CHTL/CompilerDispatcher/CompilerDispatcher.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>

void test_custom_delete_functionality() {
    std::cout << "\n--- Running Test: Custom Style 'delete' Functionality ---\n";
    try {
        std::string source = CHTL::FileSystem::readFile("Test/CustomizationTest/main.chtl");
        CHTL::CompilerDispatcher dispatcher(source);
        CHTL::FinalOutput result = dispatcher.dispatch();

        std::cout << "--- Generated HTML ---\n" << result.html << "\n----------------------\n";

        // 1. Check that the overridden property is correct and the original is gone.
        assert(result.html.find("color: red") != std::string::npos);
        assert(result.html.find("color: blue") == std::string::npos);

        // 2. Check that the deleted property is NOT present.
        assert(result.html.find("font-weight") == std::string::npos);

        // 3. Check that the other inherited property IS present.
        assert(result.html.find("font-size: 16px") != std::string::npos);

        std::cout << "PASS\n";

    } catch (const std::runtime_error& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        assert(false);
    }
}

int main() {
    try {
        test_custom_delete_functionality();
        std::cout << "\nAll customization tests passed!\n";
    } catch (...) {
        std::cerr << "An unknown exception occurred." << std::endl;
        return 1;
    }
    return 0;
}
