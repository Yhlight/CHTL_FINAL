#include <gtest/gtest.h>
#include <CHTL/CHTLLexer/Token.hpp>
#include <CHTL/CHTLLexer/Lexer.hpp>

using namespace CHTL;

class TokenTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试前准备
    }
    
    void TearDown() override {
        // 测试后清理
    }
};

// 测试 Token 基本功能
TEST_F(TokenTest, TokenCreation) {
    Token token(TokenType::IDENTIFIER, "test", 1, 1);
    
    EXPECT_EQ(token.getType(), TokenType::IDENTIFIER);
    EXPECT_EQ(token.getValue(), "test");
    EXPECT_EQ(token.getLine(), 1);
    EXPECT_EQ(token.getColumn(), 1);
}

TEST_F(TokenTest, TokenToString) {
    Token token(TokenType::STRING_LITERAL, "hello", 2, 5);
    std::string result = token.toString();
    
    EXPECT_TRUE(result.find("STRING_LITERAL") != std::string::npos);
    EXPECT_TRUE(result.find("hello") != std::string::npos);
    EXPECT_TRUE(result.find("2:5") != std::string::npos);
}

TEST_F(TokenTest, TokenTypeChecks) {
    Token literalToken(TokenType::STRING_LITERAL, "test", 1, 1);
    Token keywordToken(TokenType::TEXT, "text", 1, 1);
    Token commentToken(TokenType::SINGLE_COMMENT, "// comment", 1, 1);
    
    EXPECT_TRUE(literalToken.isLiteral());
    EXPECT_FALSE(literalToken.isKeyword());
    EXPECT_FALSE(literalToken.isComment());
    
    EXPECT_TRUE(keywordToken.isKeyword());
    EXPECT_FALSE(keywordToken.isLiteral());
    EXPECT_FALSE(keywordToken.isComment());
    
    EXPECT_TRUE(commentToken.isComment());
    EXPECT_FALSE(commentToken.isLiteral());
    EXPECT_FALSE(commentToken.isKeyword());
}

// 测试关键字映射
TEST_F(TokenTest, KeywordMap) {
    EXPECT_TRUE(KeywordMap::isKeyword("text"));
    EXPECT_TRUE(KeywordMap::isKeyword("style"));
    EXPECT_TRUE(KeywordMap::isKeyword("script"));
    EXPECT_TRUE(KeywordMap::isKeyword("[Template]"));
    EXPECT_TRUE(KeywordMap::isKeyword("[Custom]"));
    EXPECT_TRUE(KeywordMap::isKeyword("[Origin]"));
    EXPECT_TRUE(KeywordMap::isKeyword("[Import]"));
    EXPECT_TRUE(KeywordMap::isKeyword("[Namespace]"));
    EXPECT_TRUE(KeywordMap::isKeyword("[Configuration]"));
    EXPECT_TRUE(KeywordMap::isKeyword("[Info]"));
    EXPECT_TRUE(KeywordMap::isKeyword("[Export]"));
    EXPECT_TRUE(KeywordMap::isKeyword("use"));
    EXPECT_TRUE(KeywordMap::isKeyword("except"));
    EXPECT_TRUE(KeywordMap::isKeyword("delete"));
    EXPECT_TRUE(KeywordMap::isKeyword("insert"));
    EXPECT_TRUE(KeywordMap::isKeyword("inherit"));
    EXPECT_TRUE(KeywordMap::isKeyword("from"));
    EXPECT_TRUE(KeywordMap::isKeyword("as"));
    EXPECT_TRUE(KeywordMap::isKeyword("after"));
    EXPECT_TRUE(KeywordMap::isKeyword("before"));
    EXPECT_TRUE(KeywordMap::isKeyword("replace"));
    EXPECT_TRUE(KeywordMap::isKeyword("html5"));
    EXPECT_TRUE(KeywordMap::isKeyword("@Style"));
    EXPECT_TRUE(KeywordMap::isKeyword("@Element"));
    EXPECT_TRUE(KeywordMap::isKeyword("@Var"));
    EXPECT_TRUE(KeywordMap::isKeyword("@Html"));
    EXPECT_TRUE(KeywordMap::isKeyword("@JavaScript"));
    EXPECT_TRUE(KeywordMap::isKeyword("@Chtl"));
    EXPECT_TRUE(KeywordMap::isKeyword("@CJmod"));
    
    EXPECT_FALSE(KeywordMap::isKeyword("unknown"));
    EXPECT_FALSE(KeywordMap::isKeyword(""));
}

TEST_F(TokenTest, KeywordTypeMapping) {
    EXPECT_EQ(KeywordMap::getKeywordType("text"), TokenType::TEXT);
    EXPECT_EQ(KeywordMap::getKeywordType("style"), TokenType::STYLE);
    EXPECT_EQ(KeywordMap::getKeywordType("script"), TokenType::SCRIPT);
    EXPECT_EQ(KeywordMap::getKeywordType("[Template]"), TokenType::TEMPLATE);
    EXPECT_EQ(KeywordMap::getKeywordType("[Custom]"), TokenType::CUSTOM);
    EXPECT_EQ(KeywordMap::getKeywordType("[Origin]"), TokenType::ORIGIN);
    EXPECT_EQ(KeywordMap::getKeywordType("[Import]"), TokenType::IMPORT);
    EXPECT_EQ(KeywordMap::getKeywordType("[Namespace]"), TokenType::NAMESPACE);
    EXPECT_EQ(KeywordMap::getKeywordType("[Configuration]"), TokenType::CONFIGURATION);
    EXPECT_EQ(KeywordMap::getKeywordType("[Info]"), TokenType::INFO);
    EXPECT_EQ(KeywordMap::getKeywordType("[Export]"), TokenType::EXPORT);
    EXPECT_EQ(KeywordMap::getKeywordType("use"), TokenType::USE);
    EXPECT_EQ(KeywordMap::getKeywordType("except"), TokenType::EXCEPT);
    EXPECT_EQ(KeywordMap::getKeywordType("delete"), TokenType::DELETE);
    EXPECT_EQ(KeywordMap::getKeywordType("insert"), TokenType::INSERT);
    EXPECT_EQ(KeywordMap::getKeywordType("inherit"), TokenType::INHERIT);
    EXPECT_EQ(KeywordMap::getKeywordType("from"), TokenType::FROM);
    EXPECT_EQ(KeywordMap::getKeywordType("as"), TokenType::AS);
    EXPECT_EQ(KeywordMap::getKeywordType("after"), TokenType::AFTER);
    EXPECT_EQ(KeywordMap::getKeywordType("before"), TokenType::BEFORE);
    EXPECT_EQ(KeywordMap::getKeywordType("replace"), TokenType::REPLACE);
    EXPECT_EQ(KeywordMap::getKeywordType("html5"), TokenType::HTML5);
    EXPECT_EQ(KeywordMap::getKeywordType("@Style"), TokenType::TEMPLATE_STYLE);
    EXPECT_EQ(KeywordMap::getKeywordType("@Element"), TokenType::TEMPLATE_ELEMENT);
    EXPECT_EQ(KeywordMap::getKeywordType("@Var"), TokenType::TEMPLATE_VAR);
    EXPECT_EQ(KeywordMap::getKeywordType("@Html"), TokenType::ORIGIN_HTML);
    EXPECT_EQ(KeywordMap::getKeywordType("@JavaScript"), TokenType::ORIGIN_JAVASCRIPT);
    EXPECT_EQ(KeywordMap::getKeywordType("@Chtl"), TokenType::ORIGIN_CHTL);
    EXPECT_EQ(KeywordMap::getKeywordType("@CJmod"), TokenType::ORIGIN_CJMOD);
    
    EXPECT_EQ(KeywordMap::getKeywordType("unknown"), TokenType::IDENTIFIER);
}

// 测试操作符映射
TEST_F(TokenTest, OperatorMap) {
    EXPECT_TRUE(OperatorMap::isOperator("{"));
    EXPECT_TRUE(OperatorMap::isOperator("}"));
    EXPECT_TRUE(OperatorMap::isOperator("["));
    EXPECT_TRUE(OperatorMap::isOperator("]"));
    EXPECT_TRUE(OperatorMap::isOperator("("));
    EXPECT_TRUE(OperatorMap::isOperator(")"));
    EXPECT_TRUE(OperatorMap::isOperator(";"));
    EXPECT_TRUE(OperatorMap::isOperator(":"));
    EXPECT_TRUE(OperatorMap::isOperator("="));
    EXPECT_TRUE(OperatorMap::isOperator(","));
    EXPECT_TRUE(OperatorMap::isOperator("."));
    EXPECT_TRUE(OperatorMap::isOperator("#"));
    EXPECT_TRUE(OperatorMap::isOperator("@"));
    EXPECT_TRUE(OperatorMap::isOperator("&"));
    EXPECT_TRUE(OperatorMap::isOperator("?"));
    EXPECT_TRUE(OperatorMap::isOperator("!"));
    EXPECT_TRUE(OperatorMap::isOperator("_"));
    EXPECT_TRUE(OperatorMap::isOperator("/"));
    EXPECT_TRUE(OperatorMap::isOperator("\\"));
    EXPECT_TRUE(OperatorMap::isOperator("|"));
    EXPECT_TRUE(OperatorMap::isOperator("~"));
    EXPECT_TRUE(OperatorMap::isOperator("^"));
    EXPECT_TRUE(OperatorMap::isOperator("$"));
    EXPECT_TRUE(OperatorMap::isOperator("%"));
    EXPECT_TRUE(OperatorMap::isOperator("->"));
    EXPECT_TRUE(OperatorMap::isOperator("::"));
    EXPECT_TRUE(OperatorMap::isOperator("..."));
    
    EXPECT_FALSE(OperatorMap::isOperator("unknown"));
    EXPECT_FALSE(OperatorMap::isOperator(""));
}

TEST_F(TokenTest, OperatorTypeMapping) {
    EXPECT_EQ(OperatorMap::getOperatorType("{"), TokenType::LEFT_BRACE);
    EXPECT_EQ(OperatorMap::getOperatorType("}"), TokenType::RIGHT_BRACE);
    EXPECT_EQ(OperatorMap::getOperatorType("["), TokenType::LEFT_BRACKET);
    EXPECT_EQ(OperatorMap::getOperatorType("]"), TokenType::RIGHT_BRACKET);
    EXPECT_EQ(OperatorMap::getOperatorType("("), TokenType::LEFT_PAREN);
    EXPECT_EQ(OperatorMap::getOperatorType(")"), TokenType::RIGHT_PAREN);
    EXPECT_EQ(OperatorMap::getOperatorType(";"), TokenType::SEMICOLON);
    EXPECT_EQ(OperatorMap::getOperatorType(":"), TokenType::COLON);
    EXPECT_EQ(OperatorMap::getOperatorType("="), TokenType::EQUAL);
    EXPECT_EQ(OperatorMap::getOperatorType(","), TokenType::COMMA);
    EXPECT_EQ(OperatorMap::getOperatorType("."), TokenType::DOT);
    EXPECT_EQ(OperatorMap::getOperatorType("#"), TokenType::HASH);
    EXPECT_EQ(OperatorMap::getOperatorType("@"), TokenType::AT);
    EXPECT_EQ(OperatorMap::getOperatorType("&"), TokenType::AMPERSAND);
    EXPECT_EQ(OperatorMap::getOperatorType("?"), TokenType::QUESTION);
    EXPECT_EQ(OperatorMap::getOperatorType("!"), TokenType::EXCLAMATION);
    EXPECT_EQ(OperatorMap::getOperatorType("_"), TokenType::UNDERSCORE);
    EXPECT_EQ(OperatorMap::getOperatorType("/"), TokenType::SLASH);
    EXPECT_EQ(OperatorMap::getOperatorType("\\"), TokenType::BACKSLASH);
    EXPECT_EQ(OperatorMap::getOperatorType("|"), TokenType::PIPE);
    EXPECT_EQ(OperatorMap::getOperatorType("~"), TokenType::TILDE);
    EXPECT_EQ(OperatorMap::getOperatorType("^"), TokenType::CARET);
    EXPECT_EQ(OperatorMap::getOperatorType("$"), TokenType::DOLLAR);
    EXPECT_EQ(OperatorMap::getOperatorType("%"), TokenType::PERCENT);
    EXPECT_EQ(OperatorMap::getOperatorType("->"), TokenType::ARROW);
    EXPECT_EQ(OperatorMap::getOperatorType("::"), TokenType::DOUBLE_COLON);
    EXPECT_EQ(OperatorMap::getOperatorType("..."), TokenType::TRIPLE_DOT);
    
    EXPECT_EQ(OperatorMap::getOperatorType("unknown"), TokenType::ERROR);
}