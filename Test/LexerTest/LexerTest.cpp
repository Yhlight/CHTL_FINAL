#include "gtest/gtest.h"
#include "CHTLLexer/CHTLLexer.h"

TEST(LexerTest, TokenizesSimpleBlock) {
    std::string source = "div { id: \"box\"; }";
    CHTL::CHTLLexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.Tokenize();

    // Expected: div, {, id, :, "box", ;, }, EOF
    ASSERT_EQ(tokens.size(), 8);

    EXPECT_EQ(tokens[0].type, CHTL::TokenType::Identifier);
    EXPECT_EQ(tokens[0].value, "div");

    EXPECT_EQ(tokens[1].type, CHTL::TokenType::OpenBrace);
    EXPECT_EQ(tokens[1].value, "{");

    EXPECT_EQ(tokens[2].type, CHTL::TokenType::Identifier);
    EXPECT_EQ(tokens[2].value, "id");

    EXPECT_EQ(tokens[3].type, CHTL::TokenType::Colon);
    EXPECT_EQ(tokens[3].value, ":");

    EXPECT_EQ(tokens[4].type, CHTL::TokenType::StringLiteral);
    EXPECT_EQ(tokens[4].value, "box");

    EXPECT_EQ(tokens[5].type, CHTL::TokenType::Semicolon);
    EXPECT_EQ(tokens[5].value, ";");

    EXPECT_EQ(tokens[6].type, CHTL::TokenType::CloseBrace);
    EXPECT_EQ(tokens[6].value, "}");

    EXPECT_EQ(tokens[7].type, CHTL::TokenType::EndOfFile);
}
