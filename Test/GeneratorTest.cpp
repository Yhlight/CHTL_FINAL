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

TEST(GeneratorTest, FullPipelineWithMixedStyles) {
    std::string source = R"(
div {
    id: "main-box";
    style {
        color: red;
        .box {
            border: 1px solid black;
        }
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
<style>
.box {
  border: 1px solid black;
}
</style>
</head>
<body>
<div id="main-box" class="box" style="color: red; "></div>
</body>
</html>
)";

    ASSERT_EQ(removeWhitespace(final_html), removeWhitespace(expected_html));
}

TEST(GeneratorTest, AutomaticAttributeMerging) {
    std::string source = R"(
div {
    class: "explicit-class";
    style {
        .implicit-class {
            font-weight: bold;
        }
        #implicit-id {
            display: block;
        }
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
<style>
.implicit-class {
  font-weight: bold;
}
#implicit-id {
  display: block;
}
</style>
</head>
<body>
<div id="implicit-id" class="explicit-class implicit-class"></div>
</body>
</html>
)";

    ASSERT_EQ(removeWhitespace(final_html), removeWhitespace(expected_html));
}


TEST(GeneratorTest, AmpersandSelectorReplacement) {
    std::string source = R"(
button {
    class: btn;
    style {
        &:hover {
            background-color: blue;
        }
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
<style>
.btn:hover {
  background-color: blue;
}
</style>
</head>
<body>
<button class="btn"></button>
</body>
</html>
)";

    ASSERT_EQ(removeWhitespace(final_html), removeWhitespace(expected_html));
}
