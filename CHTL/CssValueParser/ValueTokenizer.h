#ifndef CHTL_VALUE_TOKENIZER_H
#define CHTL_VALUE_TOKENIZER_H

#include "ValueToken.h"
#include <string>
#include <vector>

class ValueTokenizer {
public:
    ValueTokenizer(const std::string& source);
    std::vector<ValueToken> tokenize();

private:
    bool isAtEnd();
    char advance();
    char peek();
    ValueToken makeToken(ValueTokenType type, const std::string& text);

    ValueToken number();
    ValueToken identifier();

    const std::string& source;
    size_t start = 0;
    size_t current = 0;
};

#endif // CHTL_VALUE_TOKENIZER_H
