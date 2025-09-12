#ifndef CHTL_GLOBALMAP_H
#define CHTL_GLOBALMAP_H

#include "Token.h"
#include <string>
#include <unordered_map>

namespace CHTL {

class GlobalMap {
public:
    // Constructor initializes the keyword map
    GlobalMap();

    // Returns the TokenType for a given string, or IDENTIFIER if not a keyword.
    TokenType getKeywordType(const std::string& keyword) const;

private:
    std::unordered_map<std::string, TokenType> keywordMap;
};

} // namespace CHTL

#endif // CHTL_GLOBALMAP_H
