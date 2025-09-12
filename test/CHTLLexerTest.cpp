#include <gtest/gtest.h>
#include "CHTL/CHTLLexer.h"

class CHTLLexerTest : public ::testing::Test {
protected:
    void SetUp() override {
        lexer = std::make_unique<CHTL::CHTLLexer>();
    }

    void TearDown() override {
        lexer.reset();
    }

    std::unique_ptr<CHTL::CHTLLexer> lexer;
};

TEST_F(CHTLLexerTest, BasicElement) {
    std::string source_code = "div";
    
    auto tokens = lexer->tokenize(source_code);
    
    EXPECT_EQ(tokens.size(), 2); // IDENTIFIER + EOF_TOKEN
    EXPECT_EQ(tokens[0].type, CHTL::TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[0].value, "div");
}

TEST_F(CHTLLexerTest, ElementWithAttributes) {
    std::string source_code = R"(div { id: "box"; class: "container"; })";
    
    auto tokens = lexer->tokenize(source_code);
    
    EXPECT_GT(tokens.size(), 5);
    
    // 检查关键token
    bool hasDiv = false;
    bool hasId = false;
    bool hasClass = false;
    
    for (const auto& token : tokens) {
        if (token.value == "div") hasDiv = true;
        if (token.value == "id") hasId = true;
        if (token.value == "class") hasClass = true;
    }
    
    EXPECT_TRUE(hasDiv);
    EXPECT_TRUE(hasId);
    EXPECT_TRUE(hasClass);
}

TEST_F(CHTLLexerTest, TemplateDeclaration) {
    std::string source_code = "[Template] @Style DefaultText";
    
    auto tokens = lexer->tokenize(source_code);
    
    EXPECT_GT(tokens.size(), 3);
    
    bool hasTemplate = false;
    bool hasStyle = false;
    bool hasDefaultText = false;
    
    for (const auto& token : tokens) {
        if (token.type == CHTL::TokenType::TEMPLATE) hasTemplate = true;
        if (token.type == CHTL::TokenType::STYLE_TEMPLATE) hasStyle = true;
        if (token.value == "DefaultText") hasDefaultText = true;
    }
    
    EXPECT_TRUE(hasTemplate);
    EXPECT_TRUE(hasStyle);
    EXPECT_TRUE(hasDefaultText);
}

TEST_F(CHTLLexerTest, StringLiterals) {
    std::string source_code = R"("Hello World" 'Single Quote')";
    
    auto tokens = lexer->tokenize(source_code);
    
    EXPECT_GT(tokens.size(), 3);
    
    bool hasDoubleQuote = false;
    bool hasSingleQuote = false;
    
    for (const auto& token : tokens) {
        if (token.type == CHTL::TokenType::STRING) {
            if (token.value == "Hello World") hasDoubleQuote = true;
            if (token.value == "Single Quote") hasSingleQuote = true;
        }
    }
    
    EXPECT_TRUE(hasDoubleQuote);
    EXPECT_TRUE(hasSingleQuote);
}

TEST_F(CHTLLexerTest, Comments) {
    std::string source_code = R"(
        // Single line comment
        /* Multi-line
           comment */
        div
    )";
    
    auto tokens = lexer->tokenize(source_code);
    
    // 注释应该被跳过，只留下div
    bool hasDiv = false;
    for (const auto& token : tokens) {
        if (token.value == "div") hasDiv = true;
    }
    
    EXPECT_TRUE(hasDiv);
}

TEST_F(CHTLLexerTest, Operators) {
    std::string source_code = "a + b * c / d % e ** f";
    
    auto tokens = lexer->tokenize(source_code);
    
    EXPECT_GT(tokens.size(), 10);
    
    bool hasPlus = false;
    bool hasMultiply = false;
    bool hasDivide = false;
    bool hasModulo = false;
    bool hasPower = false;
    
    for (const auto& token : tokens) {
        if (token.type == CHTL::TokenType::PLUS) hasPlus = true;
        if (token.type == CHTL::TokenType::MULTIPLY) hasMultiply = true;
        if (token.type == CHTL::TokenType::DIVIDE) hasDivide = true;
        if (token.type == CHTL::TokenType::MODULO) hasModulo = true;
        if (token.type == CHTL::TokenType::POWER) hasPower = true;
    }
    
    EXPECT_TRUE(hasPlus);
    EXPECT_TRUE(hasMultiply);
    EXPECT_TRUE(hasDivide);
    EXPECT_TRUE(hasModulo);
    EXPECT_TRUE(hasPower);
}

TEST_F(CHTLLexerTest, EmptyInput) {
    std::string source_code = "";
    
    auto tokens = lexer->tokenize(source_code);
    
    EXPECT_EQ(tokens.size(), 1); // Only EOF_TOKEN
    EXPECT_EQ(tokens[0].type, CHTL::TokenType::EOF_TOKEN);
}

TEST_F(CHTLLexerTest, WhitespaceHandling) {
    std::string source_code = "   div   {   id   :   \"test\"   ;   }   ";
    
    auto tokens = lexer->tokenize(source_code);
    
    // 空白字符应该被跳过
    for (const auto& token : tokens) {
        EXPECT_NE(token.type, CHTL::TokenType::WHITESPACE);
    }
}