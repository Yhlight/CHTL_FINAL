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


TEST(GeneratorTest, CustomStyleSpecialization) {
    std::string source = R"(
[Custom] @Style MyText {
    color,
    font-size: 16px;
    font-weight: bold;
}

div {
    style {
        @Style MyText {
            color: blue;
            delete font-weight;
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
</head>
<body>
<div style="color: blue; font-size: 16px; "></div>
</body>
</html>
)";

    ASSERT_EQ(removeWhitespace(final_html), removeWhitespace(expected_html));
}
