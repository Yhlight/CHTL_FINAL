#pragma once

#include "Token.h"
#include <string>
#include <vector>

class ChtlJsLexer {
public:
    ChtlJsLexer(const std::string& source);
    std::vector<ChtlJsToken> tokenize();

private:
    std::string source;
    size_t current = 0;

    char peek() const;
    char peekNext() const;
    bool isAtEnd() const;
};
