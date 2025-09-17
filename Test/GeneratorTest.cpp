#include <gtest/gtest.h>
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLGenerator/CHTLGenerator.h"
#include <string>

// Helper function to remove whitespace to make string comparisons more robust
std::string removeWhitespace(std::string str) {
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\t'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    return str;
}

TEST(GeneratorTest, FullPipeline) {
    std::string source = R"(
div {
    id: "main-box";
    class: box;

    style {
        color: red;
        width: 100px;
    }

    text {
        "Hello, CHTL!"
    }
}
)";

    // 1. Lex
    CHTLLexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    // 2. Parse
    CHTLParser parser(tokens);
    std::unique_ptr<ProgramNode> ast = parser.parse();

    // 3. Generate
    ASSERT_TRUE(ast != nullptr);
    CHTLGenerator generator;
    std::string final_html = generator.generate(*ast);

    std::string expected_html = R"(
<!DOCTYPE html>
<html>
<head>
<style>
  color: red;
  width: 100px;
</style>
</head>
<body>
<div id="main-box" class="box">Hello, CHTL!</div>
</body>
</html>
)";

    ASSERT_EQ(removeWhitespace(final_html), removeWhitespace(expected_html));
}
