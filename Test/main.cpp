#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include <iostream>
#include <string>
#include <vector>

// For simplicity, we'll include the cpp files directly.
// In a real build system, we would link the compiled object files.
#include "../Util/StringUtil/StringUtil.cpp"
#include "../Util/FileSystem/FileSystem.cpp"
#include "../CHTL/CHTLLexer/Token.cpp"
#include "../CHTL/CHTLLexer/Lexer.cpp"
#include "../CHTL/CHTLNode/Node.cpp"
#include "../CHTL/CHTLExpression/ExprNode.cpp"
#include "../CHTL/CHTLExpression/ExpressionParser.cpp"
#include "../CHTL/CHTLParser/Parser.cpp"
#include "../CHTL/CHTLGenerator/Generator.cpp"

void runTest(const std::string& testName, const std::string& chtlInput, const std::string& expectedHtml)
{
    std::cout << "Running test: " << testName << "..." << std::endl;

    // 1. Lexing
    CHTL::Lexer lexer(chtlInput);
    auto tokens = lexer.tokenize();

    // 2. Parsing
    CHTL::Parser parser(tokens);
    auto ast = parser.parse();

    // 3. Generating
    CHTL::Generator generator(std::move(ast));
    std::string generatedHtml = generator.generate();

    // 4. Comparing
    // We need to normalize whitespace to make the comparison robust.
    std::string normalizedGenerated = generatedHtml;
    std::string normalizedExpected = expectedHtml;

    normalizedGenerated.erase(std::remove_if(normalizedGenerated.begin(), normalizedGenerated.end(), ::isspace), normalizedGenerated.end());
    normalizedExpected.erase(std::remove_if(normalizedExpected.begin(), normalizedExpected.end(), ::isspace), normalizedExpected.end());


    if (normalizedGenerated == normalizedExpected)
    {
        std::cout << "[PASSED]" << std::endl;
    }
    else
    {
        std::cout << "[FAILED]" << std::endl;
        std::cout << "  Expected:\n" << expectedHtml << std::endl;
        std::cout << "  Got:\n" << generatedHtml << std::endl;
    }
    std::cout << "---------------------------------" << std::endl;
}

int main()
{
    std::string test1_input = R"(
        html {
            head {
                title { text { "My CHTL Page" } }
            }
            body {
                div {
                    text { "Hello, World!" }
                }
            }
        }
    )";

    std::string test1_expected = R"(
        <html>
          <head>
            <title>
              My CHTL Page
            </title>
          </head>
          <body>
            <div>
              Hello, World!
            </div>
          </body>
        </html>
    )";

    runTest("Simple Nested Structure", test1_input, test1_expected);

    std::string test2_input = R"(
        div id: "main"; class: "container"; {
            p {
                text { "An element with attributes." }
            }
        }
    )";

    std::string test2_expected = R"(
        <div class="container" id="main">
          <p>
            An element with attributes.
          </p>
        </div>
    )";

    runTest("Element with Attributes", test2_input, test2_expected);

    std::string test3_input = R"(
        // This comment should be ignored.
        div {
            -- This comment should be rendered.
            /* This multi-line comment
               should also be ignored. */
            p { text { "Content" } }
        }
    )";

    std::string test3_expected = R"(
        <div>
          <!--  This comment should be rendered. -->
          <p>
            Content
          </p>
        </div>
    )";

    runTest("Comment Handling", test3_input, test3_expected);

    std::string test4_input = R"(
        div class: my-class; {
            p {
                text { This is an unquoted literal. }
            }
        }
    )";

    std::string test4_expected = R"(
        <div class="my-class">
          <p>
            This is an unquoted literal.
          </p>
        </div>
    )";

    runTest("Unquoted Literals", test4_input, test4_expected);

    std::string test5_input = R"(
        div class: "special"; {
            text: "This is the content.";
        }
    )";

    std::string test5_expected = R"(
        <div class="special">
          This is the content.
        </div>
    )";

    runTest("Text Attribute", test5_input, test5_expected);

    std::string test6_input = R"(
        div {
            style {
                width: 100px;
                height: 200px;
                color: red;
            }
            p { text: "Styled content" }
        }
    )";

    // Note: std::map in StyleNode will order properties alphabetically.
    std::string test6_expected = R"(
        <div style="color:red;height:200px;width:100px;">
          <p>
            Styled content
          </p>
        </div>
    )";

    runTest("Local Style Block", test6_input, test6_expected);

    std::string test7_input = R"(
        div {
            style {
                border: 1px solid black;
                .box {
                    color: red;
                }
            }
            text: "I am a styled div."
        }
    )";

    // Note: attribute order is alphabetical due to std::map
    std::string test7_expected = R"(
        <style>
        .box {
          color: red;
        }
        </style>
        <div class="box" style="border:1px solid black;">
          I am a styled div.
        </div>
    )";

    runTest("Advanced Style Block (auto class)", test7_input, test7_expected);

    std::string test8_input = R"(
        div {
            style {
                .box {
                    color: blue;
                }
                &:hover {
                    color: red;
                }
            }
            text: "Hover over me."
        }
    )";

    std::string test8_expected = R"(
        <style>
        .box {
          color: blue;
        }
        .box:hover {
          color: red;
        }
        </style>
        <div class="box">
          Hover over me.
        </div>
    )";

    runTest("Context-Aware & Selector", test8_input, test8_expected);

    std::string test9_input = R"(
        div {
            style {
                width: 100px + 20 * 2;
                padding: 10px + 5px;
            }
        }
    )";

    std::string test9_expected = R"(
        <div style="padding:15px;width:140px;">
        </div>
    )";

    runTest("Attribute Calculation", test9_input, test9_expected);

    std::string test10_input = R"(
        div class: box; {
            style { width: 100px; }
        }
        div {
            style {
                width: .box.width + 50px;
            }
        }
    )";

    std::string test10_expected = R"(
        <div class="box" style="width:100px;">
        </div>
        <div style="width:150px;">
        </div>
    )";

    runTest("Attribute Reference", test10_input, test10_expected);

    return 0;
}
