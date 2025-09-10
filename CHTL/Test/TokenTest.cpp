#include "TokenTest.h"

namespace CHTL {
namespace Test {

std::unique_ptr<TestSuite> TokenTest::createTestSuite() {
    auto suite = std::make_unique<TestSuite>("TokenTest");
    
    // Token基本功能测试
    suite->addTestCase(std::make_unique<TestCase>("testTokenCreation", "测试Token创建功能", testTokenCreation));
    suite->addTestCase(std::make_unique<TestCase>("testTokenComparison", "测试Token比较功能", testTokenComparison));
    suite->addTestCase(std::make_unique<TestCase>("testTokenToString", "测试Token字符串转换功能", testTokenToString));
    suite->addTestCase(std::make_unique<TestCase>("testTokenTypeName", "测试Token类型名称功能", testTokenTypeName));
    
    // TokenStream测试
    suite->addTestCase(std::make_unique<TestCase>("testTokenStream", "测试TokenStream基本功能", testTokenStream));
    suite->addTestCase(std::make_unique<TestCase>("testTokenStreamOperations", "测试TokenStream操作功能", testTokenStreamOperations));
    suite->addTestCase(std::make_unique<TestCase>("testTokenStreamFind", "测试TokenStream查找功能", testTokenStreamFind));
    suite->addTestCase(std::make_unique<TestCase>("testTokenStreamSkip", "测试TokenStream跳过功能", testTokenStreamSkip));
    suite->addTestCase(std::make_unique<TestCase>("testTokenStreamBacktrack", "测试TokenStream回退功能", testTokenStreamBacktrack));
    
    // 词法分析器测试
    suite->addTestCase(std::make_unique<TestCase>("testLexerBasic", "测试词法分析器基本功能", testLexerBasic));
    suite->addTestCase(std::make_unique<TestCase>("testLexerComments", "测试词法分析器注释功能", testLexerComments));
    suite->addTestCase(std::make_unique<TestCase>("testLexerStrings", "测试词法分析器字符串功能", testLexerStrings));
    suite->addTestCase(std::make_unique<TestCase>("testLexerNumbers", "测试词法分析器数字功能", testLexerNumbers));
    suite->addTestCase(std::make_unique<TestCase>("testLexerOperators", "测试词法分析器运算符功能", testLexerOperators));
    suite->addTestCase(std::make_unique<TestCase>("testLexerSymbols", "测试词法分析器符号功能", testLexerSymbols));
    suite->addTestCase(std::make_unique<TestCase>("testLexerKeywords", "测试词法分析器关键字功能", testLexerKeywords));
    suite->addTestCase(std::make_unique<TestCase>("testLexerTemplates", "测试词法分析器模板功能", testLexerTemplates));
    suite->addTestCase(std::make_unique<TestCase>("testLexerColonEqual", "测试词法分析器CE对等式功能", testLexerColonEqual));
    suite->addTestCase(std::make_unique<TestCase>("testLexerElementNames", "测试词法分析器元素名称功能", testLexerElementNames));
    suite->addTestCase(std::make_unique<TestCase>("testLexerAttributeNames", "测试词法分析器属性名称功能", testLexerAttributeNames));
    suite->addTestCase(std::make_unique<TestCase>("testLexerTextContent", "测试词法分析器文本内容功能", testLexerTextContent));
    suite->addTestCase(std::make_unique<TestCase>("testLexerLiterals", "测试词法分析器字面量功能", testLexerLiterals));
    
    return suite;
}

// Token基本功能测试实现
void TokenTest::testTokenCreation() {
    Token token1(TokenType::IDENTIFIER, "test", 1, 1, 0);
    CHTL_ASSERT_EQUAL("test", token1.value);
    CHTL_ASSERT_TRUE(token1.type == TokenType::IDENTIFIER);
    CHTL_ASSERT_EQUAL("1", std::to_string(token1.line));
    CHTL_ASSERT_EQUAL("1", std::to_string(token1.column));
    
    Token token2;
    CHTL_ASSERT_TRUE(token2.type == TokenType::UNKNOWN);
    CHTL_ASSERT_EQUAL("", token2.value);
}

void TokenTest::testTokenComparison() {
    Token token1(TokenType::IDENTIFIER, "test", 1, 1, 0);
    Token token2(TokenType::IDENTIFIER, "test", 1, 1, 0);
    Token token3(TokenType::STRING, "test", 1, 1, 0);
    
    CHTL_ASSERT_TRUE(token1 == token2);
    CHTL_ASSERT_FALSE(token1 == token3);
    CHTL_ASSERT_TRUE(token1 != token3);
}

void TokenTest::testTokenToString() {
    Token token(TokenType::IDENTIFIER, "test", 1, 1, 0);
    std::string str = token.toString();
    CHTL_ASSERT_TRUE(str.find("IDENTIFIER") != std::string::npos);
    CHTL_ASSERT_TRUE(str.find("test") != std::string::npos);
}

void TokenTest::testTokenTypeName() {
    Token token(TokenType::IDENTIFIER, "test", 1, 1, 0);
    std::string typeName = token.getTypeName();
    CHTL_ASSERT_EQUAL("IDENTIFIER", typeName);
    
    Token token2(TokenType::STRING, "hello", 1, 1, 0);
    std::string typeName2 = token2.getTypeName();
    CHTL_ASSERT_EQUAL("STRING", typeName2);
}

// TokenStream测试实现
void TokenTest::testTokenStream() {
    TokenStream stream;
    CHTL_ASSERT_EQUAL("0", std::to_string(stream.size()));
    CHTL_ASSERT_FALSE(stream.hasNext());
    
    Token token(TokenType::IDENTIFIER, "test", 1, 1, 0);
    stream.addToken(token);
    
    CHTL_ASSERT_EQUAL("1", std::to_string(stream.size()));
    CHTL_ASSERT_TRUE(stream.hasNext());
}

void TokenTest::testTokenStreamOperations() {
    TokenStream stream;
    
    Token token1(TokenType::IDENTIFIER, "test1", 1, 1, 0);
    Token token2(TokenType::STRING, "hello", 1, 6, 5);
    Token token3(TokenType::NUMBER, "123", 1, 12, 11);
    
    stream.addToken(token1);
    stream.addToken(token2);
    stream.addToken(token3);
    
    CHTL_ASSERT_EQUAL("3", std::to_string(stream.size()));
    
    auto peeked = stream.peek(0);
    CHTL_ASSERT_EQUAL("test1", peeked.value);
    
    auto consumed = stream.consume();
    CHTL_ASSERT_EQUAL("test1", consumed.value);
    CHTL_ASSERT_EQUAL("2", std::to_string(stream.size()));
}

void TokenTest::testTokenStreamFind() {
    TokenStream stream;
    
    Token token1(TokenType::IDENTIFIER, "test1", 1, 1, 0);
    Token token2(TokenType::STRING, "hello", 1, 6, 5);
    Token token3(TokenType::NUMBER, "123", 1, 12, 11);
    
    stream.addToken(token1);
    stream.addToken(token2);
    stream.addToken(token3);
    
    auto found = stream.findNext(TokenType::STRING);
    CHTL_ASSERT_EQUAL("hello", found.value);
    
    CHTL_ASSERT_TRUE(stream.hasNext(TokenType::NUMBER));
    CHTL_ASSERT_FALSE(stream.hasNext(TokenType::IDENTIFIER));
}

void TokenTest::testTokenStreamSkip() {
    TokenStream stream;
    
    Token token1(TokenType::WHITESPACE, " ", 1, 1, 0);
    Token token2(TokenType::SINGLE_COMMENT, "// comment", 1, 2, 1);
    Token token3(TokenType::IDENTIFIER, "test", 1, 12, 11);
    
    stream.addToken(token1);
    stream.addToken(token2);
    stream.addToken(token3);
    
    stream.skipWhitespaceAndComments();
    
    auto current = stream.current();
    CHTL_ASSERT_EQUAL("test", current.value);
}

void TokenTest::testTokenStreamBacktrack() {
    TokenStream stream;
    
    Token token1(TokenType::IDENTIFIER, "test1", 1, 1, 0);
    Token token2(TokenType::STRING, "hello", 1, 6, 5);
    
    stream.addToken(token1);
    stream.addToken(token2);
    
    stream.consume(); // 消费第一个token
    stream.backtrack(); // 回退
    
    auto current = stream.current();
    CHTL_ASSERT_EQUAL("test1", current.value);
}

// 词法分析器测试实现
void TokenTest::testLexerBasic() {
    CHTLLexer lexer("div: Hello World");
    auto tokens = lexer.tokenize();
    
    CHTL_ASSERT_TRUE(tokens.size() > 0);
    CHTL_ASSERT_TRUE(tokens.hasNext());
}

void TokenTest::testLexerComments() {
    CHTLLexer lexer("// single comment\n/* multi comment */\n-- generator comment");
    auto tokens = lexer.tokenize();
    
    int commentCount = 0;
    while (tokens.hasNext()) {
        auto token = tokens.consume();
        if (token.type == TokenType::SINGLE_COMMENT || 
            token.type == TokenType::MULTI_COMMENT || 
            token.type == TokenType::GENERATOR_COMMENT) {
            commentCount++;
        }
    }
    
    CHTL_ASSERT_EQUAL("3", std::to_string(commentCount));
}

void TokenTest::testLexerStrings() {
    CHTLLexer lexer("\"double quoted\" 'single quoted'");
    auto tokens = lexer.tokenize();
    
    int stringCount = 0;
    while (tokens.hasNext()) {
        auto token = tokens.consume();
        if (token.type == TokenType::DOUBLE_QUOTED_LITERAL || 
            token.type == TokenType::SINGLE_QUOTED_LITERAL) {
            stringCount++;
        }
    }
    
    CHTL_ASSERT_EQUAL("2", std::to_string(stringCount));
}

void TokenTest::testLexerNumbers() {
    CHTLLexer lexer("123 45.67 1e10");
    auto tokens = lexer.tokenize();
    
    int numberCount = 0;
    while (tokens.hasNext()) {
        auto token = tokens.consume();
        if (token.type == TokenType::NUMBER) {
            numberCount++;
        }
    }
    
    CHTL_ASSERT_EQUAL("3", std::to_string(numberCount));
}

void TokenTest::testLexerOperators() {
    CHTLLexer lexer("+ - * / = == != < > <= >=");
    auto tokens = lexer.tokenize();
    
    int operatorCount = 0;
    while (tokens.hasNext()) {
        auto token = tokens.consume();
        if (token.isOperator()) {
            operatorCount++;
        }
    }
    
    CHTL_ASSERT_TRUE(operatorCount > 0);
}

void TokenTest::testLexerSymbols() {
    CHTLLexer lexer("{}()[];:,.!@#$%^&*");
    auto tokens = lexer.tokenize();
    
    int symbolCount = 0;
    while (tokens.hasNext()) {
        auto token = tokens.consume();
        if (token.isDelimiter()) {
            symbolCount++;
        }
    }
    
    CHTL_ASSERT_TRUE(symbolCount > 0);
}

void TokenTest::testLexerKeywords() {
    CHTLLexer lexer("text style script use inherit delete");
    auto tokens = lexer.tokenize();
    
    int keywordCount = 0;
    while (tokens.hasNext()) {
        auto token = tokens.consume();
        if (token.isKeyword()) {
            keywordCount++;
        }
    }
    
    CHTL_ASSERT_EQUAL("6", std::to_string(keywordCount));
}

void TokenTest::testLexerTemplates() {
    CHTLLexer lexer("[Template] [Custom] [Origin] [Import] [Namespace] [Configuration]");
    auto tokens = lexer.tokenize();
    
    int templateCount = 0;
    while (tokens.hasNext()) {
        auto token = tokens.consume();
        if (token.type == TokenType::TEMPLATE || 
            token.type == TokenType::CUSTOM ||
            token.type == TokenType::ORIGIN ||
            token.type == TokenType::IMPORT ||
            token.type == TokenType::NAMESPACE ||
            token.type == TokenType::CONFIGURATION) {
            templateCount++;
        }
    }
    
    CHTL_ASSERT_EQUAL("6", std::to_string(templateCount));
}

void TokenTest::testLexerColonEqual() {
    CHTLLexer lexer("div := Hello World");
    auto tokens = lexer.tokenize();
    
    bool foundColonEqual = false;
    while (tokens.hasNext()) {
        auto token = tokens.consume();
        if (token.type == TokenType::COLON_EQUAL) {
            foundColonEqual = true;
            CHTL_ASSERT_EQUAL(":=", token.value);
        }
    }
    
    CHTL_ASSERT_TRUE(foundColonEqual);
}

void TokenTest::testLexerElementNames() {
    CHTLLexer lexer("div span p h1 h2");
    auto tokens = lexer.tokenize();
    
    int elementCount = 0;
    while (tokens.hasNext()) {
        auto token = tokens.consume();
        if (token.type == TokenType::ELEMENT_NAME) {
            elementCount++;
        }
    }
    
    CHTL_ASSERT_EQUAL("5", std::to_string(elementCount));
}

void TokenTest::testLexerAttributeNames() {
    CHTLLexer lexer("class id style data-value");
    auto tokens = lexer.tokenize();
    
    int attributeCount = 0;
    while (tokens.hasNext()) {
        auto token = tokens.consume();
        if (token.type == TokenType::ATTRIBUTE_NAME) {
            attributeCount++;
        }
    }
    
    CHTL_ASSERT_TRUE(attributeCount > 0);
}

void TokenTest::testLexerTextContent() {
    CHTLLexer lexer("Hello World This is text content");
    auto tokens = lexer.tokenize();
    
    int textCount = 0;
    while (tokens.hasNext()) {
        auto token = tokens.consume();
        if (token.type == TokenType::TEXT_CONTENT) {
            textCount++;
        }
    }
    
    CHTL_ASSERT_TRUE(textCount > 0);
}

void TokenTest::testLexerLiterals() {
    CHTLLexer lexer("unquoted 'single' \"double\"");
    auto tokens = lexer.tokenize();
    
    int literalCount = 0;
    while (tokens.hasNext()) {
        auto token = tokens.consume();
        if (token.type == TokenType::UNQUOTED_LITERAL || 
            token.type == TokenType::SINGLE_QUOTED_LITERAL ||
            token.type == TokenType::DOUBLE_QUOTED_LITERAL) {
            literalCount++;
        }
    }
    
    CHTL_ASSERT_EQUAL("3", std::to_string(literalCount));
}

} // namespace Test
} // namespace CHTL