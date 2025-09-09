#include "CHTLJSToken.h"
#include <sstream>
#include <stdexcept>

namespace CHTL {

const std::map<std::string, CHTLJSTokenType> CHTLJSToken::KEYWORDS = {
    // CHTL JS关键字
    {"fileloader", CHTLJSTokenType::FILELOADER},
    {"listen", CHTLJSTokenType::LISTEN},
    {"delegate", CHTLJSTokenType::DELEGATE},
    {"animate", CHTLJSTokenType::ANIMATE},
    {"vir", CHTLJSTokenType::VIR},
    {"router", CHTLJSTokenType::ROUTER},
    {"util", CHTLJSTokenType::UTIL},
    {"iNeverAway", CHTLJSTokenType::INEVERAWAY},
    
    // 位置关键字
    {"after", CHTLJSTokenType::AFTER},
    {"before", CHTLJSTokenType::BEFORE},
    {"replace", CHTLJSTokenType::REPLACE},
    {"at", CHTLJSTokenType::AT},
    {"top", CHTLJSTokenType::AT_TOP},
    {"bottom", CHTLJSTokenType::AT_BOTTOM},
    
    // 布尔值
    {"true", CHTLJSTokenType::BOOLEAN},
    {"false", CHTLJSTokenType::BOOLEAN},
    {"null", CHTLJSTokenType::BOOLEAN}
};

const std::map<std::string, CHTLJSTokenType> CHTLJSToken::OPERATORS = {
    {"+", CHTLJSTokenType::PLUS},
    {"-", CHTLJSTokenType::MINUS},
    {"*", CHTLJSTokenType::MULTIPLY},
    {"/", CHTLJSTokenType::DIVIDE},
    {"%", CHTLJSTokenType::MODULO},
    {"&&", CHTLJSTokenType::AND},
    {"||", CHTLJSTokenType::OR},
    {"!", CHTLJSTokenType::NOT},
    {">", CHTLJSTokenType::GREATER},
    {"<", CHTLJSTokenType::LESS},
    {">=", CHTLJSTokenType::GREATER_EQUAL},
    {"<=", CHTLJSTokenType::LESS_EQUAL},
    {"==", CHTLJSTokenType::EQUAL_EQUAL},
    {"!=", CHTLJSTokenType::NOT_EQUAL},
    {"?", CHTLJSTokenType::QUESTION},
    {"=", CHTLJSTokenType::EQUAL},
    {"->", CHTLJSTokenType::ARROW}
};

const std::map<std::string, CHTLJSTokenType> CHTLJSToken::DELIMITERS = {
    {"{", CHTLJSTokenType::LEFT_BRACE},
    {"}", CHTLJSTokenType::RIGHT_BRACE},
    {"[", CHTLJSTokenType::LEFT_BRACKET},
    {"]", CHTLJSTokenType::RIGHT_BRACKET},
    {"(", CHTLJSTokenType::LEFT_PAREN},
    {")", CHTLJSTokenType::RIGHT_PAREN},
    {";", CHTLJSTokenType::SEMICOLON},
    {",", CHTLJSTokenType::COMMA},
    {".", CHTLJSTokenType::DOT},
    {":", CHTLJSTokenType::COLON}
};

CHTLJSToken::CHTLJSToken(CHTLJSTokenType type, std::string value, size_t line, size_t column, size_t position)
    : type(type), value(std::move(value)), line(line), column(column), position(position) {}

std::string CHTLJSToken::toString() const {
    return "CHTLJSToken(Type: " + tokenTypeToString(type) + ", Value: \"" + value + "\", Line: " + 
           std::to_string(line) + ", Column: " + std::to_string(column) + ")";
}

std::string CHTLJSToken::tokenTypeToString(CHTLJSTokenType type) {
    switch (type) {
        case CHTLJSTokenType::IDENTIFIER: return "IDENTIFIER";
        case CHTLJSTokenType::STRING: return "STRING";
        case CHTLJSTokenType::NUMBER: return "NUMBER";
        case CHTLJSTokenType::BOOLEAN: return "BOOLEAN";
        case CHTLJSTokenType::LEFT_BRACE: return "LEFT_BRACE";
        case CHTLJSTokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case CHTLJSTokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case CHTLJSTokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case CHTLJSTokenType::LEFT_PAREN: return "LEFT_PAREN";
        case CHTLJSTokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case CHTLJSTokenType::SEMICOLON: return "SEMICOLON";
        case CHTLJSTokenType::COMMA: return "COMMA";
        case CHTLJSTokenType::DOT: return "DOT";
        case CHTLJSTokenType::COLON: return "COLON";
        case CHTLJSTokenType::EQUAL: return "EQUAL";
        case CHTLJSTokenType::PLUS: return "PLUS";
        case CHTLJSTokenType::MINUS: return "MINUS";
        case CHTLJSTokenType::MULTIPLY: return "MULTIPLY";
        case CHTLJSTokenType::DIVIDE: return "DIVIDE";
        case CHTLJSTokenType::MODULO: return "MODULO";
        case CHTLJSTokenType::AND: return "AND";
        case CHTLJSTokenType::OR: return "OR";
        case CHTLJSTokenType::NOT: return "NOT";
        case CHTLJSTokenType::GREATER: return "GREATER";
        case CHTLJSTokenType::LESS: return "LESS";
        case CHTLJSTokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case CHTLJSTokenType::LESS_EQUAL: return "LESS_EQUAL";
        case CHTLJSTokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
        case CHTLJSTokenType::NOT_EQUAL: return "NOT_EQUAL";
        case CHTLJSTokenType::AT: return "AT";
        case CHTLJSTokenType::HASH: return "HASH";
        case CHTLJSTokenType::DOLLAR: return "DOLLAR";
        case CHTLJSTokenType::QUESTION: return "QUESTION";
        case CHTLJSTokenType::EXCLAMATION: return "EXCLAMATION";
        case CHTLJSTokenType::AMPERSAND: return "AMPERSAND";
        case CHTLJSTokenType::PIPE: return "PIPE";
        case CHTLJSTokenType::TILDE: return "TILDE";
        case CHTLJSTokenType::CARET: return "CARET";
        case CHTLJSTokenType::ARROW: return "ARROW";
        case CHTLJSTokenType::FILELOADER: return "FILELOADER";
        case CHTLJSTokenType::LISTEN: return "LISTEN";
        case CHTLJSTokenType::DELEGATE: return "DELEGATE";
        case CHTLJSTokenType::ANIMATE: return "ANIMATE";
        case CHTLJSTokenType::VIR: return "VIR";
        case CHTLJSTokenType::ROUTER: return "ROUTER";
        case CHTLJSTokenType::UTIL: return "UTIL";
        case CHTLJSTokenType::INEVERAWAY: return "INEVERAWAY";
        case CHTLJSTokenType::AFTER: return "AFTER";
        case CHTLJSTokenType::BEFORE: return "BEFORE";
        case CHTLJSTokenType::REPLACE: return "REPLACE";
        case CHTLJSTokenType::AT_TOP: return "AT_TOP";
        case CHTLJSTokenType::AT_BOTTOM: return "AT_BOTTOM";
        case CHTLJSTokenType::SELECTOR: return "SELECTOR";
        case CHTLJSTokenType::NEWLINE: return "NEWLINE";
        case CHTLJSTokenType::WHITESPACE: return "WHITESPACE";
        case CHTLJSTokenType::EOF_TOKEN: return "EOF_TOKEN";
        case CHTLJSTokenType::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

bool CHTLJSToken::isKeyword(const std::string& value) {
    return KEYWORDS.find(value) != KEYWORDS.end();
}

CHTLJSTokenType CHTLJSToken::getKeywordType(const std::string& value) {
    auto it = KEYWORDS.find(value);
    if (it != KEYWORDS.end()) {
        return it->second;
    }
    return CHTLJSTokenType::IDENTIFIER;
}

bool CHTLJSToken::isOperator(const std::string& value) {
    return OPERATORS.find(value) != OPERATORS.end();
}

CHTLJSTokenType CHTLJSToken::getOperatorType(const std::string& value) {
    auto it = OPERATORS.find(value);
    if (it != KEYWORDS.end()) {
        return it->second;
    }
    return CHTLJSTokenType::ERROR;
}

bool CHTLJSToken::isDelimiter(const std::string& value) {
    return DELIMITERS.find(value) != DELIMITERS.end();
}

CHTLJSTokenType CHTLJSToken::getDelimiterType(const std::string& value) {
    auto it = DELIMITERS.find(value);
    if (it != DELIMITERS.end()) {
        return it->second;
    }
    return CHTLJSTokenType::ERROR;
}

} // namespace CHTL