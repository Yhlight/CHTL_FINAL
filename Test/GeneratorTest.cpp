#include <gtest/gtest.h>
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLGenerator/CHTLGenerator.h"
#include <string>
#include <algorithm>

// Helper function to remove whitespace to make string comparisons more robust
std::string removeWhitespace(std::string str) {
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\t'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    return str;
}


TEST(GeneratorTest, StyleTemplate) {
    std::string source = R"(
[Template] @Style DefaultText {
    color: black;
    font-size: 16px;
}

div {
    style {
        @Style DefaultText;
        border: 1px solid grey;
    }
}
)";

    CHTLLexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    CHTLParser parser(tokens);
    std::unique_ptr<ProgramNode> ast = parser.parse();
    ASSERT_TRUE(ast != nullptr);
    CHTLGenerator generator;
    std::string final_html = generator.generate(*ast);

    std::string expected_html = R"(
<!DOCTYPE html>
<html>
<head>
</head>
<body>
<div style="color: black; font-size: 16px; border: 1px solid grey; "></div>
</body>
</html>
)";

    ASSERT_EQ(removeWhitespace(final_html), removeWhitespace(expected_html));
}

TEST(GeneratorTest, ElementTemplate) {
    std::string source = R"(
[Template] @Element MyBox {
    span { text { "I am in a box" } }
}

div {
    @Element MyBox;
}
)";

    CHTLLexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    CHTLParser parser(tokens);
    std::unique_ptr<ProgramNode> ast = parser.parse();
    ASSERT_TRUE(ast != nullptr);
    CHTLGenerator generator;
    std::string final_html = generator.generate(*ast);

    std::string expected_html = R"(
<!DOCTYPE html>
<html>
<head>
</head>
<body>
<div><span>I am in a box</span></div>
</body>
</html>
)";

    ASSERT_EQ(removeWhitespace(final_html), removeWhitespace(expected_html));
}

TEST(GeneratorTest, TemplateInheritance) {
    std::string source = R"(
[Template] @Style Base {
    padding: 10px;
}

[Template] @Style RedBox {
    @Style Base;
    background-color: red;
}

div {
    style {
        @Style RedBox;
    }
}
)";

    CHTLLexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    CHTLParser parser(tokens);
    std::unique_ptr<ProgramNode> ast = parser.parse();
    ASSERT_TRUE(ast != nullptr);
    CHTLGenerator generator;
    std::string final_html = generator.generate(*ast);

    std::string expected_html = R"(
<!DOCTYPE html>
<html>
<head>
</head>
<body>
<div style="padding: 10px; background-color: red; "></div>
</body>
</html>
)";

    ASSERT_EQ(removeWhitespace(final_html), removeWhitespace(expected_html));
}
