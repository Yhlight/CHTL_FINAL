#include "Test.h"
#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include "../CompilerDispatcher/CompilerDispatcher.h"
#include "../Util/StringUtil/StringUtil.h"
#include <string>

// Helper function to normalize HTML for comparison by removing whitespace and newlines.
std::string normalize_html(const std::string& html) {
    std::string result;
    for (char c : html) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            result += c;
        }
    }
    return result;
}

// This test is now obsolete as the dispatcher creates a full document,
// but we'll keep its core to test basic element/attribute parsing.
TEST_CASE(Parser_UnquotedLiterals) {
    const std::string source = R"(div { class: some-class-name; })";
    const std::string expected_body = R"(<div class="some-class-name"></div>)";

    CHTL::Lexer lexer(source);
    CHTL::Parser parser(lexer);
    auto ast = parser.parseProgram();
    ASSERT_EQUAL(0, parser.getErrors().size());

    CHTL::Generator g;
    std::string actual_body = g.generate(ast.get());

    ASSERT_EQUAL(normalize_html(expected_body), normalize_html(actual_body));

    return true;
}

TEST_CASE(Compiler_InlineStyleBlock) {
    const std::string source = R"(
        div {
            id: my-div;
            style {
                color: red;
                border: 1px solid black;
            }
        }
    )";

    // The dispatcher now creates a full HTML document.
    const std::string expected_html = R"(
        <!DOCTYPE html>
        <html>
        <head>
          <meta charset="UTF-8">
          <title>CHTL Output</title>
        </head>
        <body>
          <div id="my-div" style="border: 1px solid black; color: red;">
          </div>
        </body>
        </html>
    )";

    std::vector<CHTL::CodeFragment> fragments = {{CHTL::CodeType::CHTL, source}};
    CHTL::CompilerDispatcher dispatcher;
    std::string actual_html = dispatcher.dispatch(fragments);

    ASSERT_EQUAL(normalize_html(expected_html), normalize_html(actual_html));

    return true;
}

TEST_CASE(Compiler_GlobalAndInlineStyles) {
    const std::string source = R"(
        div {
            style {
                // This is an inline style
                color: blue;

                // This is a global style rule
                .nested {
                    font-weight: bold;
                }
            }
            p { text { "Hello" } }
        }
    )";

    const std::string expected_html = R"(
        <!DOCTYPE html>
        <html>
        <head>
          <meta charset="UTF-8">
          <title>CHTL Output</title>
          <style>
          .nested {
            font-weight: bold;
          }
          </style>
        </head>
        <body>
          <div style="color: blue;">
            <p>
              Hello
            </p>
          </div>
        </body>
        </html>
    )";

    std::vector<CHTL::CodeFragment> fragments = {{CHTL::CodeType::CHTL, source}};
    CHTL::CompilerDispatcher dispatcher;
    std::string actual_html = dispatcher.dispatch(fragments);

    ASSERT_EQUAL(normalize_html(expected_html), normalize_html(actual_html));
    return true;
}
