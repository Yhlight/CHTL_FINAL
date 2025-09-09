#include <iostream>
#include <string>
#include <vector>
#include "CHTL/CHTLLexer.h"

using namespace CHTL;

void printToken(const Token& token) {
    std::cout << "Token: " << static_cast<int>(token.type) 
              << " | Value: '" << token.value 
              << "' | Line: " << token.line 
              << " | Column: " << token.column << std::endl;
}

void testBasicTokens() {
    std::cout << "=== Testing Basic Tokens ===" << std::endl;
    
    CHTLLexer lexer;
    std::string input = R"(
html
{
    head
    {
        title
        {
            text: "Hello World";
        }
    }
}
)";
    
    std::vector<Token> tokens = lexer.tokenize(input);
    
    for (const auto& token : tokens) {
        if (token.type != TokenType::EOF_TOKEN) {
            printToken(token);
        }
    }
    
    std::cout << "Total tokens: " << tokens.size() - 1 << std::endl;
}

void testComments() {
    std::cout << "\n=== Testing Comments ===" << std::endl;
    
    CHTLLexer lexer;
    std::string input = R"(
// This is a line comment
div
{
    /* This is a block comment */
    text: "Content";
    -- This is a generator comment
}
)";
    
    std::vector<Token> tokens = lexer.tokenize(input);
    
    for (const auto& token : tokens) {
        if (token.type != TokenType::EOF_TOKEN) {
            printToken(token);
        }
    }
}

void testStrings() {
    std::cout << "\n=== Testing Strings ===" << std::endl;
    
    CHTLLexer lexer;
    std::string input = R"(
div
{
    text: "Double quoted string";
    text: 'Single quoted string';
    text: Unquoted literal;
}
)";
    
    std::vector<Token> tokens = lexer.tokenize(input);
    
    for (const auto& token : tokens) {
        if (token.type != TokenType::EOF_TOKEN) {
            printToken(token);
        }
    }
}

void testOperators() {
    std::cout << "\n=== Testing Operators ===" << std::endl;
    
    CHTLLexer lexer;
    std::string input = R"(
div
{
    style
    {
        width: 100px;
        height: 200px;
        background-color: width > 50px ? "red" : "blue";
        display: width > 100px && height < 200px ? "block" : "inline";
    }
}
)";
    
    std::vector<Token> tokens = lexer.tokenize(input);
    
    for (const auto& token : tokens) {
        if (token.type != TokenType::EOF_TOKEN) {
            printToken(token);
        }
    }
}

int main() {
    std::cout << "CHTL Lexer Test Suite" << std::endl;
    std::cout << "====================" << std::endl;
    
    testBasicTokens();
    testComments();
    testStrings();
    testOperators();
    
    std::cout << "\nAll tests completed!" << std::endl;
    return 0;
}