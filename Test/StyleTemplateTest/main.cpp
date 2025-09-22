#include "../../Scanner/CHTLUnifiedScanner.h"
#include "../../CHTL/CompilerDispatcher.h"
#include "../../CHTL/Config/Configuration.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>
#include <memory>

// Helper function to remove all whitespace from a string for robust comparison
std::string remove_whitespace(const std::string& str) {
    std::string out;
    out.reserve(str.size());
    for (char c : str) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            out += c;
        }
    }
    return out;
}

int main() {
    std::string file_path = "Test/StyleTemplateTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);
    auto config = std::make_shared<CHTL::Configuration>();

    // 1. Scanner
    CHTL::CHTLUnifiedScanner scanner(source, config);
    auto fragments = scanner.scan();

    // 2. Dispatcher
    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    std::cout << "--- HTML ---" << std::endl;
    std::cout << result.html << std::endl;

    std::string html_no_space = remove_whitespace(result.html);

    // Test 1: Basic Inheritance
    std::string expected1 = R"(<buttonid="btn1"style="font-size:16px;padding:10px20px;border:1pxsolidblack;background-color:red;color:white;"></button>)";
    assert(html_no_space.find(remove_whitespace(expected1)) != std::string::npos);

    // Test 2: Specialization (Override)
    std::string expected2 = R"(<buttonid="btn2"style="font-size:16px;padding:10px20px;border:1pxsolidblack;background-color:darkred;color:white;"></button>)";
    assert(html_no_space.find(remove_whitespace(expected2)) != std::string::npos);

    // Test 3: Specialization (Delete)
    std::string expected3_present = R"(<buttonid="btn3"style="font-size:16px;padding:10px20px;background-color:red;color:white;border-radius:5px;"></button>)";
    std::string expected3_absent = "border:1pxsolidblack;";
    assert(html_no_space.find(remove_whitespace(expected3_present)) != std::string::npos);
    assert(html_no_space.find(remove_whitespace(expected3_absent)) == std::string::npos);

    std::cout << "StyleTemplateTest PASSED!" << std::endl;

    return 0;
}
