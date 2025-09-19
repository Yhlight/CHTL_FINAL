#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <algorithm> // Required for std::remove

// Helper function to remove whitespace for easier comparison
std::string removeWhitespace(std::string str) {
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\t'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    return str;
}

void runGeneratorTest(const std::string& testName, const std::string& source, const std::string& expectedHtml, const std::string& expectedCss) {
    std::cout << "Running generator test: " << testName << "..." << std::endl;

    CHTL::CHTLLexer lexer(source);
    auto tokens = lexer.scanTokens();

    CHTL::CHTLParser parser(source, tokens, "test.chtl");
    auto root = parser.parse();

    assert(root != nullptr);

    auto templates = parser.getTemplateDefinitions();
    CHTL::CHTLGenerator generator(templates);
    auto result = generator.generate(root.get());

    // Check HTML
    std::string clean_generated_html = removeWhitespace(result.html);
    std::string clean_expected_html = removeWhitespace(expectedHtml);

    if (clean_generated_html == clean_expected_html) {
        std::cout << "  - HTML: PASS" << std::endl;
    } else {
        std::cerr << "  - HTML: FAIL" << std::endl;
        std::cerr << "    Expected: " << clean_expected_html << std::endl;
        std::cerr << "    Got:      " << clean_generated_html << std::endl;
    }
    assert(clean_generated_html == clean_expected_html);

    // Check CSS
    std::string clean_generated_css = removeWhitespace(result.css);
    std::string clean_expected_css = removeWhitespace(expectedCss);

     if (clean_generated_css == clean_expected_css) {
        std::cout << "  - CSS: PASS" << std::endl;
    } else {
        std::cerr << "  - CSS: FAIL" << std::endl;
        std::cerr << "    Expected: " << clean_expected_css << std::endl;
        std::cerr << "    Got:      " << clean_generated_css << std::endl;
    }
    assert(clean_generated_css == clean_expected_css);

    std::cout << "--------------------------------------" << std::endl;
}

void runGeneratorTest(const std::string& testName, const std::string& source, const std::string& expectedHtml) {
    runGeneratorTest(testName, source, expectedHtml, "");
}


int main() {
    runGeneratorTest("Simple Element",
        "div { text: \"hello\"; }",
        R"(<div>hello</div>)"
    );

    runGeneratorTest("Nested Elements",
        "body { p { text: \"world\"; } }",
        R"(<body><p>world</p></body>)"
    );

    runGeneratorTest("Generator Comment",
        "# A comment\n p {}",
        R"(<!-- A comment--><p></p>)"
    );

    runGeneratorTest("Namespace Content",
        "[Namespace] foo { h1 { text: \"Header\"; } }",
        R"(<h1>Header</h1>)"
    );

    runGeneratorTest("Style Block",
        R"(
            div {
                style {
                    width: 100px;
                    .my-class {
                        color: red;
                    }
                }
            }
        )",
        R"(<div style="width:100px;"></div>)",
        R"(.my-class{color:red;})"
    );

    std::cout << "All generator tests completed." << std::endl;
    return 0;
}
