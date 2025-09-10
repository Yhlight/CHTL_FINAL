#include "../CHTL_JS/CHTLJSLexer/CHTLJSLexer.h"
#include <iostream>
#include <vector>
#include <map>

std::string tokenTypeToString(CHTLJSTokenType type) {
    static const std::map<CHTLJSTokenType, std::string> typeMap = {
        {CHTLJSTokenType::TOKEN_EOF, "EOF"},
        {CHTLJSTokenType::TOKEN_UNKNOWN, "UNKNOWN"},
        {CHTLJSTokenType::TOKEN_IDENTIFIER, "IDENTIFIER"},
        {CHTLJSTokenType::TOKEN_STRING, "STRING"},
        {CHTLJSTokenType::TOKEN_NUMBER, "NUMBER"},
        {CHTLJSTokenType::TOKEN_LBRACE, "LBRACE"},
        {CHTLJSTokenType::TOKEN_RBRACE, "RBRACE"},
        {CHTLJSTokenType::TOKEN_COLON, "COLON"},
        {CHTLJSTokenType::TOKEN_COMMA, "COMMA"},
        {CHTLJSTokenType::TOKEN_ARROW, "ARROW"},
        {CHTLJSTokenType::TOKEN_DOUBLE_LBRACE, "DOUBLE_LBRACE"},
        {CHTLJSTokenType::TOKEN_DOUBLE_RBRACE, "DOUBLE_RBRACE"},
        {CHTLJSTokenType::TOKEN_LPAREN, "LPAREN"},
        {CHTLJSTokenType::TOKEN_RPAREN, "RPAREN"},
        {CHTLJSTokenType::TOKEN_LBRACKET, "LBRACKET"},
        {CHTLJSTokenType::TOKEN_RBRACKET, "RBRACKET"},
    };
    auto it = typeMap.find(type);
    if (it != typeMap.end()) {
        return it->second;
    }
    return "UNHANDLED_TOKEN_TYPE";
}

int main() {
    std::string source = R"(
        {{.my-button}}->listen {
            click: someClickHandler,
            mouseenter: someFunction
        }
    )";

    std::cout << "--- Starting CHTL JS Lexer Test ---" << std::endl;
    CHTLJSLexer lexer(source);

    CHTLJSToken token;
    do {
        token = lexer.nextToken();
        std::cout << "Token | Type: " << tokenTypeToString(token.type)
                  << " | Lexeme: '" << token.lexeme << "'"
                  << " | Line: " << token.line << std::endl;
    } while (token.type != CHTLJSTokenType::TOKEN_EOF);

    std::cout << "--- CHTL JS Lexer Test Finished ---" << std::endl;
    return 0;
}
