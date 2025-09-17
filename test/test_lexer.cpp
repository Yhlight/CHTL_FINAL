#include "test_main.h"
#include "../src/CHTL/CHTLLexer.h"
#include <iostream>

TEST(LexerBasic) {
    std::string source = "div { text: \"Hello World\"; }";
    CHTLLexer lexer(source);
    TokenList tokens = lexer.tokenize();
    
    ASSERT_TRUE(tokens.size() > 0);
    ASSERT_STREQ("div", tokens[0]->getValue().c_str());
    ASSERT_EQ(TokenType::IDENTIFIER, tokens[0]->getType());
}

TEST(LexerString) {
    std::string source = "\"Hello World\"";
    CHTLLexer lexer(source);
    TokenList tokens = lexer.tokenize();
    
    ASSERT_TRUE(tokens.size() > 0);
    ASSERT_STREQ("Hello World", tokens[0]->getValue().c_str());
    ASSERT_EQ(TokenType::STRING, tokens[0]->getType());
}

TEST(LexerLiteral) {
    std::string source = "HelloWorld";
    CHTLLexer lexer(source);
    TokenList tokens = lexer.tokenize();
    
    ASSERT_TRUE(tokens.size() > 0);
    ASSERT_STREQ("HelloWorld", tokens[0]->getValue().c_str());
    ASSERT_EQ(TokenType::LITERAL, tokens[0]->getType());
}

TEST(LexerSymbols) {
    std::string source = "{}();:=";
    CHTLLexer lexer(source);
    TokenList tokens = lexer.tokenize();
    
    ASSERT_TRUE(tokens.size() >= 7);
    ASSERT_EQ(TokenType::LEFT_BRACE, tokens[0]->getType());
    ASSERT_EQ(TokenType::RIGHT_BRACE, tokens[1]->getType());
    ASSERT_EQ(TokenType::LEFT_PAREN, tokens[2]->getType());
    ASSERT_EQ(TokenType::RIGHT_PAREN, tokens[3]->getType());
    ASSERT_EQ(TokenType::SEMICOLON, tokens[4]->getType());
    ASSERT_EQ(TokenType::COLON, tokens[5]->getType());
    ASSERT_EQ(TokenType::EQUALS, tokens[6]->getType());
}

TEST(LexerKeywords) {
    std::string source = "[Template] @Style";
    CHTLLexer lexer(source);
    TokenList tokens = lexer.tokenize();
    
    ASSERT_TRUE(tokens.size() >= 2);
    ASSERT_EQ(TokenType::TEMPLATE, tokens[0]->getType());
    ASSERT_EQ(TokenType::STYLE, tokens[1]->getType());
}

TEST(LexerComments) {
    std::string source = "// This is a comment\n/* This is a multiline comment */";
    CHTLLexer lexer(source);
    TokenList tokens = lexer.tokenize();
    
    ASSERT_TRUE(tokens.size() >= 2);
    ASSERT_EQ(TokenType::COMMENT, tokens[0]->getType());
    ASSERT_EQ(TokenType::MULTILINE_COMMENT, tokens[1]->getType());
}

TEST(LexerGeneratorComment) {
    std::string source = "# This is a generator comment";
    CHTLLexer lexer(source);
    TokenList tokens = lexer.tokenize();
    
    ASSERT_TRUE(tokens.size() > 0);
    ASSERT_EQ(TokenType::GENERATOR_COMMENT, tokens[0]->getType());
}

TEST(LexerOperators) {
    std::string source = "+ - * / % ** && || == != < > <= >=";
    CHTLLexer lexer(source);
    TokenList tokens = lexer.tokenize();
    
    ASSERT_TRUE(tokens.size() >= 13);
    ASSERT_EQ(TokenType::PLUS, tokens[0]->getType());
    ASSERT_EQ(TokenType::MINUS, tokens[1]->getType());
    ASSERT_EQ(TokenType::MULTIPLY, tokens[2]->getType());
    ASSERT_EQ(TokenType::DIVIDE, tokens[3]->getType());
    ASSERT_EQ(TokenType::MODULO, tokens[4]->getType());
    ASSERT_EQ(TokenType::POWER, tokens[5]->getType());
    ASSERT_EQ(TokenType::AND, tokens[6]->getType());
    ASSERT_EQ(TokenType::OR, tokens[7]->getType());
    ASSERT_EQ(TokenType::EQUAL, tokens[8]->getType());
    ASSERT_EQ(TokenType::NOT_EQUAL, tokens[9]->getType());
    ASSERT_EQ(TokenType::LESS, tokens[10]->getType());
    ASSERT_EQ(TokenType::GREATER, tokens[11]->getType());
    ASSERT_EQ(TokenType::LESS_EQUAL, tokens[12]->getType());
    ASSERT_EQ(TokenType::GREATER_EQUAL, tokens[13]->getType());
}

TEST(LexerComplex) {
    std::string source = R"(
div
{
    id: box;
    class: welcome;
    
    text
    {
        Hello World
    }
}
)";
    CHTLLexer lexer(source);
    TokenList tokens = lexer.tokenize();
    
    ASSERT_TRUE(tokens.size() > 0);
    
    // 检查第一个token是div
    ASSERT_STREQ("div", tokens[0]->getValue().c_str());
    ASSERT_EQ(TokenType::LITERAL, tokens[0]->getType());
}