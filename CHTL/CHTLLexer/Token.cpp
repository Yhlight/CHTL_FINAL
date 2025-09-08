#include "Token.h"
#include <map>

std::map<std::string, TokenType> keywordTokenMap = {
    {"KEYWORD_TEXT", TokenType::TEXT},
    {"KEYWORD_STYLE", TokenType::STYLE},
    {"KEYWORD_TEMPLATE", TokenType::KEYWORD_TEMPLATE},
    {"KEYWORD_CUSTOM", TokenType::KEYWORD_CUSTOM},
    {"KEYWORD_ORIGIN", TokenType::KEYWORD_ORIGIN},
    {"KEYWORD_IMPORT", TokenType::KEYWORD_IMPORT},
    {"KEYWORD_NAMESPACE", TokenType::KEYWORD_NAMESPACE},
    {"KEYWORD_CONFIGURATION", TokenType::KEYWORD_CONFIGURATION},
    {"KEYWORD_FROM", TokenType::KEYWORD_FROM},
    {"KEYWORD_AS", TokenType::KEYWORD_AS},
    {"KEYWORD_INHERIT", TokenType::KEYWORD_INHERIT},
    {"KEYWORD_DELETE", TokenType::KEYWORD_DELETE},
    {"KEYWORD_INSERT", TokenType::KEYWORD_INSERT},
    {"KEYWORD_AFTER", TokenType::KEYWORD_AFTER},
    {"KEYWORD_BEFORE", TokenType::KEYWORD_BEFORE},
    {"KEYWORD_REPLACE", TokenType::KEYWORD_REPLACE},
    {"KEYWORD_ATTOP", TokenType::KEYWORD_ATTOP},
    {"KEYWORD_ATBOTTOM", TokenType::KEYWORD_ATBOTTOM}
};

TokenType stringToTokenType(const std::string& s) {
    if (keywordTokenMap.count(s)) {
        return keywordTokenMap.at(s);
    }
    return TokenType::IDENTIFIER; // Default or error case
}
