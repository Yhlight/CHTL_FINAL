#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/Config/Configuration.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <memory>
#include <stdexcept>
#include <vector>

void run_test(const std::string& file_path, const std::string& expected_error_part) {
    std::cout << "--- Running test: " << file_path << " ---" << std::endl;
    std::string source = CHTL::FileSystem::readFile(file_path);
    auto config = std::make_shared<CHTL::Configuration>();

    CHTL::CHTLLexer lexer(source, config);
    auto tokens = lexer.scanTokens();

    CHTL::CHTLParser parser(source, tokens, file_path, config);

    bool exception_thrown = false;
    std::string error_message;
    try {
        parser.parse();
    } catch (const std::runtime_error& e) {
        exception_thrown = true;
        error_message = e.what();
        std::cout << "Caught expected exception: " << error_message << std::endl;
    }

    assert(exception_thrown);
    assert(error_message.find(expected_error_part) != std::string::npos);
    std::cout << "--- PASSED ---" << std::endl << std::endl;
}

int main() {
    std::vector<std::pair<std::string, std::string>> tests = {
        {"Test/ExceptTest/tests/except_html.chtl", "except @Html"},
        {"Test/ExceptTest/tests/except_template.chtl", "except' constraint"},
        {"Test/ExceptTest/tests/except_custom_type.chtl", "except [Custom]"}
        // {"Test/ExceptTest/tests/except_var_template.chtl", "except [Template] @Var"} // Not implemented
    };

    for (const auto& test : tests) {
        run_test(test.first, test.second);
    }

    std::cout << "All ExceptTests PASSED!" << std::endl;

    return 0;
}
