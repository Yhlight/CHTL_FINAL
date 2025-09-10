#pragma once

#include "Token.h"
#include <string>
#include <vector>
#include <map>

class CHTLJSLexer {
public:
    CHTLJSLexer(const std::string& source);
    std::vector<CHTLJSToken> tokenize();

private:
    CHTLJSToken nextToken();
    char peek();
    char advance();
    void skipWhitespace();

    CHTLJSToken makeToken(CHTLJSTokenType type) const;
    CHTLJSToken makeToken(CHTLJSTokenType type, const std::string& value) const;
    CHTLJSToken errorToken(const std::string& message) const;

    CHTLJSToken identifier();
    CHTLJSToken number();
    CHTLJSToken stringLiteral(char opening_quote);
    CHTLJSToken symbol();

    bool isAtEnd() const;
    bool isAlpha(char c) const;
    bool isDigit(char c) const;

    std::string source;
    size_t start = 0;
    size_t current = 0;
    int line = 1;
    int column = 1;

    static const std::map<std::string, CHTLJSTokenType> keywords;
};
