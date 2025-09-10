#pragma once

#include "CHTLJSToken.h"
#include <string>

class CHTLJSLexer {
public:
    CHTLJSLexer(const std::string& source);
    CHTLJSToken nextToken();

private:
    std::string source;
    size_t start;
    size_t current;
    int line;

    bool isAtEnd();
    char advance();
    char peek();
    bool match(char expected);

    CHTLJSToken makeToken(CHTLJSTokenType type);
    CHTLJSToken errorToken(const std::string& message);
    void skipWhitespace();
    CHTLJSToken identifierToken();
    CHTLJSToken stringToken();
    CHTLJSToken numberToken();
};
