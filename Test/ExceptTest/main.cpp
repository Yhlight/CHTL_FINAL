#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include <iostream>
#include <cassert>
#include <string>

bool run_test(const std::string& test_name, const std::string& source, bool should_throw) {
    std::cout << "--- Running test: " << test_name << " ---\n";
    bool exception_thrown = false;
    try {
        CHTL::CHTLLexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();
        CHTL::CHTLParser parser(source, tokens, "test.chtl");
        std::unique_ptr<CHTL::BaseNode> ast = parser.parse();
        // We only need to parse to test constraints
    } catch (const std::runtime_error& e) {
        exception_thrown = true;
        std::cout << "Caught expected exception: " << e.what() << std::endl;
    }

    if (should_throw && exception_thrown) {
        std::cout << "--- " << test_name << ": PASSED ---\n\n";
        return true;
    }
    if (!should_throw && !exception_thrown) {
        std::cout << "--- " << test_name << ": PASSED ---\n\n";
        return true;
    }

    std::cout << "--- " << test_name << ": FAILED ---\n\n";
    return false;
}

int main() {
    int failed_tests = 0;

    std::string success_source =
        "body {\n"
        "    div {\n"
        "        except span;\n"
        "        p { text: \"allowed\"; }\n"
        "    }\n"
        "}";
    if (!run_test("Success Case: Tag Name", success_source, false)) {
        failed_tests++;
    }

    std::string failure_source =
        "body {\n"
        "    div {\n"
        "        except span;\n"
        "        p { text: \"allowed\"; }\n"
        "        span { text: \"not allowed\"; }\n"
        "    }\n"
        "}";
    if (!run_test("Failure Case: Tag Name", failure_source, true)) {
        failed_tests++;
    }

    std::string any_html_failure_source =
        "[Template] @Element MyTemplate { p{} }\n" // Dummy template
        "body {\n"
        "    div {\n"
        "        except @Html;\n"
        "        p { text: \"not allowed\"; }\n"
        "    }\n"
        "}";
    if (!run_test("Failure Case: @Html", any_html_failure_source, true)) {
        failed_tests++;
    }


    if (failed_tests == 0) {
        std::cout << "All except tests passed!\n";
        return 0;
    } else {
        std::cout << failed_tests << " except test(s) failed!\n";
        return 1;
    }
}
