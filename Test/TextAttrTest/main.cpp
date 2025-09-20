#include "../../CHTL/CompilerDispatcher/CompilerDispatcher.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>

void test_text_attribute() {
    std::cout << "\n--- Running Test: Text Attribute Functionality ---\n";
    try {
        std::string source = CHTL::FileSystem::readFile("Test/TextAttrTest/test.chtl");
        CHTL::CompilerDispatcher dispatcher(source);
        CHTL::FinalOutput result = dispatcher.dispatch();

        std::cout << "--- Generated HTML ---\n" << result.html << "\n----------------------\n";

        // Check that the div contains the correct text from the attribute.
        assert(result.html.find("<div>hello from attribute</div>") != std::string::npos);

        std::cout << "PASS\n";

    } catch (const std::runtime_error& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        assert(false);
    }
}

int main() {
    try {
        test_text_attribute();
        std::cout << "\nAll text attribute tests passed!\n";
    } catch (...) {
        std::cerr << "An unknown exception occurred." << std::endl;
        return 1;
    }
    return 0;
}
