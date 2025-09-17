#include <gtest/gtest.h>
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include <vector>

TEST(LexerTest, BasicElementsAndAttributes) {
    std::string source = R"(
div {
    id: "box";
    class: red;
}
)";
    CHTLLexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    std::vector<std::pair<TokenType, std::string>> expected_tokens = {
        {TokenType::IDENTIFIER, "div"},
        {TokenType::LBRACE, "{"},
        {TokenType::IDENTIFIER, "id"},
        {TokenType::COLON, ":"},
        {TokenType::STRING_LITERAL, "\"box\""},
        {TokenType::SEMICOLON, ";"},
        {TokenType::IDENTIFIER, "class"},
        {TokenType::COLON, ":"},
        {TokenType::IDENTIFIER, "red"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::RBRACE, "}"},
        {TokenType::END_OF_FILE, ""}
    };

    ASSERT_EQ(tokens.size(), expected_tokens.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_tokens[i].first);
        EXPECT_EQ(tokens[i].lexeme, expected_tokens[i].second);
    }
}

TEST(LexerTest, CommentsAndWhitespace) {
    std::string source = R"(
// comment
# generator comment
body { /* multi-line */ }
)";
    CHTLLexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    std::vector<std::pair<TokenType, std::string>> expected_tokens = {
        {TokenType::GENERATOR_COMMENT, "# generator comment"},
        {TokenType::IDENTIFIER, "body"},
        {TokenType::LBRACE, "{"},
        {TokenType::RBRACE, "}"},
        {TokenType::END_OF_FILE, ""}
    };

    ASSERT_EQ(tokens.size(), expected_tokens.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_tokens[i].first);
        EXPECT_EQ(tokens[i].lexeme, expected_tokens[i].second);
    }
}
