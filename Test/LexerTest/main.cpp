#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include <iostream>
#include <vector>
#include <cassert>

// We need a proper implementation of tokenTypeToString to make test output readable.
// Since it's declared in Token.h but not defined in the provided code, I'll add a simple one here.
namespace CHTL {
std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::AT: return "AT";
        case TokenType::COLON: return "COLON";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::DOT: return "DOT";
        case TokenType::QUESTION: return "QUESTION";
        case TokenType::COMMA: return "COMMA";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::SLASH: return "SLASH";
        case TokenType::PERCENT: return "PERCENT";
        case TokenType::GREATER: return "GREATER";
        case TokenType::LESS: return "LESS";
        case TokenType::HASH: return "HASH";
        case TokenType::DOLLAR: return "DOLLAR";
        case TokenType::BANG: return "BANG";
        case TokenType::BANG_EQUAL: return "BANG_EQUAL";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::AMPERSAND: return "AMPERSAND";
        case TokenType::AMPERSAND_AMPERSAND: return "AMPERSAND_AMPERSAND";
        case TokenType::PIPE: return "PIPE";
        case TokenType::PIPE_PIPE: return "PIPE_PIPE";
        case TokenType::ARROW: return "ARROW";
        case TokenType::STAR_STAR: return "STAR_STAR";
        case TokenType::LEFT_BRACE_BRACE: return "LEFT_BRACE_BRACE";
        case TokenType::RIGHT_BRACE_BRACE: return "RIGHT_BRACE_BRACE";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING: return "STRING";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case TokenType::KEYWORD_STYLE: return "KEYWORD_STYLE";
        case TokenType::KEYWORD_TEXT: return "KEYWORD_TEXT";
        case TokenType::KEYWORD_SCRIPT: return "KEYWORD_SCRIPT";
        case TokenType::KEYWORD_INHERIT: return "KEYWORD_INHERIT";
        case TokenType::KEYWORD_FROM: return "KEYWORD_FROM";
        case TokenType::KEYWORD_AS: return "KEYWORD_AS";
        case TokenType::KEYWORD_DELETE: return "KEYWORD_DELETE";
        case TokenType::KEYWORD_INSERT: return "KEYWORD_INSERT";
        case TokenType::KEYWORD_AFTER: return "KEYWORD_AFTER";
        case TokenType::KEYWORD_BEFORE: return "KEYWORD_BEFORE";
        case TokenType::KEYWORD_REPLACE: return "KEYWORD_REPLACE";
        case TokenType::KEYWORD_AT_TOP: return "KEYWORD_AT_TOP";
        case TokenType::KEYWORD_AT_BOTTOM: return "KEYWORD_AT_BOTTOM";
        case TokenType::KEYWORD_EXCEPT: return "KEYWORD_EXCEPT";
        case TokenType::KEYWORD_USE: return "KEYWORD_USE";
        case TokenType::KEYWORD_HTML5: return "KEYWORD_HTML5";
        case TokenType::KEYWORD_TEMPLATE: return "KEYWORD_TEMPLATE";
        case TokenType::KEYWORD_CUSTOM: return "KEYWORD_CUSTOM";
        case TokenType::KEYWORD_ORIGIN: return "KEYWORD_ORIGIN";
        case TokenType::KEYWORD_IMPORT: return "KEYWORD_IMPORT";
        case TokenType::KEYWORD_NAMESPACE: return "KEYWORD_NAMESPACE";
        case TokenType::KEYWORD_CONFIGURATION: return "KEYWORD_CONFIGURATION";
        case TokenType::KEYWORD_INFO: return "KEYWORD_INFO";
        case TokenType::KEYWORD_EXPORT: return "KEYWORD_EXPORT";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::UNKNOWN: return "UNKNOWN";
        case TokenType::COMMENT: return "COMMENT";
        default: return "UNDEFINED_TOKEN";
    }
}
}

void printTokens(const std::vector<CHTL::Token>& tokens) {
    for (const auto& token : tokens) {
        std::cout << "Type: " << CHTL::tokenTypeToString(token.type)
                  << ", Lexeme: '" << token.lexeme << "'"
                  << ", Line: " << token.line << std::endl;
    }
}

void runTest(const std::string& testName, const std::string& source, const std::vector<CHTL::TokenType>& expectedTypes) {
    std::cout << "Running test: " << testName << "..." << std::endl;
    CHTL::CHTLLexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.scanTokens();

    // printTokens(tokens);

    assert(tokens.size() == expectedTypes.size() + 1); // +1 for EOF

    bool pass = true;
    for (size_t i = 0; i < expectedTypes.size(); ++i) {
        if (tokens[i].type != expectedTypes[i]) {
            std::cerr << "FAIL: " << testName << " - Token " << i << " mismatch." << std::endl;
            std::cerr << "  Expected: " << CHTL::tokenTypeToString(expectedTypes[i])
                      << ", Got: " << CHTL::tokenTypeToString(tokens[i].type)
                      << " ('" << tokens[i].lexeme << "')" << std::endl;
            pass = false;
            break;
        }
    }

    if(pass) {
        std::cout << "PASS: " << testName << std::endl;
    }
     std::cout << "--------------------------------------" << std::endl;
}

int main() {
    runTest("Single Character Tokens", "+ - * / % # $", {
        CHTL::TokenType::PLUS,
        CHTL::TokenType::MINUS,
        CHTL::TokenType::STAR,
        CHTL::TokenType::SLASH,
        CHTL::TokenType::PERCENT,
        CHTL::TokenType::HASH,
        CHTL::TokenType::DOLLAR,
    });

    runTest("Multi-character Tokens", "-> ** == != <= >= && ||", {
        CHTL::TokenType::ARROW,
        CHTL::TokenType::STAR_STAR,
        CHTL::TokenType::EQUAL_EQUAL,
        CHTL::TokenType::BANG_EQUAL,
        CHTL::TokenType::LESS_EQUAL,
        CHTL::TokenType::GREATER_EQUAL,
        CHTL::TokenType::AMPERSAND_AMPERSAND,
        CHTL::TokenType::PIPE_PIPE,
    });

    runTest("Comments", "// this is a comment\n# this is a generator comment", {
        CHTL::TokenType::COMMENT,
        CHTL::TokenType::HASH,
        CHTL::TokenType::IDENTIFIER, CHTL::TokenType::IDENTIFIER, CHTL::TokenType::IDENTIFIER, CHTL::TokenType::IDENTIFIER, CHTL::TokenType::IDENTIFIER,
    });

    runTest("Block Keywords", "[Template] [Custom] [Origin] [Import] [Namespace] [Configuration] [Info] [Export]", {
        CHTL::TokenType::KEYWORD_TEMPLATE,
        CHTL::TokenType::KEYWORD_CUSTOM,
        CHTL::TokenType::KEYWORD_ORIGIN,
        CHTL::TokenType::KEYWORD_IMPORT,
        CHTL::TokenType::KEYWORD_NAMESPACE,
        CHTL::TokenType::KEYWORD_CONFIGURATION,
        CHTL::TokenType::KEYWORD_INFO,
        CHTL::TokenType::KEYWORD_EXPORT,
    });

    runTest("Invalid Block Keyword", "[Invalid]", {
        CHTL::TokenType::LEFT_BRACKET,
        CHTL::TokenType::IDENTIFIER,
        CHTL::TokenType::RIGHT_BRACKET,
    });

    runTest("Simple Keywords", "style text script use html5", {
        CHTL::TokenType::KEYWORD_STYLE,
        CHTL::TokenType::KEYWORD_TEXT,
        CHTL::TokenType::KEYWORD_SCRIPT,
        CHTL::TokenType::KEYWORD_USE,
        CHTL::TokenType::KEYWORD_HTML5,
    });

    runTest("At Keywords", "at top at bottom", {
        CHTL::TokenType::KEYWORD_AT_TOP,
        CHTL::TokenType::KEYWORD_AT_BOTTOM,
    });

    runTest("Complex Identifier", "my_var_123", {
        CHTL::TokenType::IDENTIFIER,
    });

    runTest("String Literals", "'hello' \"world\"", {
        CHTL::TokenType::STRING,
        CHTL::TokenType::STRING,
    });

    runTest("Number Literals", "123 45.67", {
        CHTL::TokenType::NUMBER,
        CHTL::TokenType::NUMBER,
    });

    std::cout << "All lexer tests completed." << std::endl;

    return 0;
}
