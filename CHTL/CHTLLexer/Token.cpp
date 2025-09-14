#include "Token.h"
#include <map>
#include <sstream>

namespace CHTL {

bool Token::isKeyword() const {
    return KeywordMap::isKeyword(value);
}

bool Token::isOperator() const {
    switch (type) {
        case TokenType::COLON:
        case TokenType::EQUALS:
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::ASTERISK:
        case TokenType::SLASH:
        case TokenType::PERCENT:
        case TokenType::LESS_THAN:
        case TokenType::GREATER_THAN:
        case TokenType::AMPERSAND:
        case TokenType::PIPE:
        case TokenType::CARET:
        case TokenType::TILDE:
        case TokenType::EXCLAMATION:
        case TokenType::QUESTION:
            return true;
        default:
            return false;
    }
}

bool Token::isLiteral() const {
    return type == TokenType::LITERAL || type == TokenType::STRING || type == TokenType::NUMBER;
}

bool Token::isIdentifier() const {
    return type == TokenType::IDENTIFIER;
}

std::string Token::toString() const {
    std::stringstream ss;
    ss << "Token(" << static_cast<int>(type) << ", \"" << value << "\", " 
       << line << ":" << column << ")";
    return ss.str();
}

const std::map<std::string, TokenType> KeywordMap::keywordMap_ = {
    // 块关键字
    {"[Template]", TokenType::TEMPLATE},
    {"[Custom]", TokenType::CUSTOM},
    {"[Origin]", TokenType::ORIGIN},
    {"[Import]", TokenType::IMPORT},
    {"[Namespace]", TokenType::NAMESPACE},
    {"[Configuration]", TokenType::CONFIGURATION},
    {"[Info]", TokenType::INFO},
    {"[Export]", TokenType::EXPORT},
    
    // 类型关键字
    {"@Style", TokenType::STYLE},
    {"@Element", TokenType::ELEMENT},
    {"@Var", TokenType::VAR},
    {"@Html", TokenType::HTML},
    {"@JavaScript", TokenType::JAVASCRIPT},
    {"@Chtl", TokenType::CHTL},
    {"@CJmod", TokenType::CJMOD},
    
    // 操作关键字
    {"inherit", TokenType::INHERIT},
    {"delete", TokenType::DELETE},
    {"insert", TokenType::INSERT},
    {"after", TokenType::AFTER},
    {"before", TokenType::BEFORE},
    {"replace", TokenType::REPLACE},
    {"at top", TokenType::AT_TOP},
    {"at bottom", TokenType::AT_BOTTOM},
    {"from", TokenType::FROM},
    {"as", TokenType::AS},
    {"except", TokenType::EXCEPT},
    {"use", TokenType::USE},
    {"html5", TokenType::HTML5},
    {"text", TokenType::TEXT},
    {"script", TokenType::SCRIPT}
};

TokenType KeywordMap::getKeywordType(const std::string& keyword) {
    auto it = keywordMap_.find(keyword);
    if (it != keywordMap_.end()) {
        return it->second;
    }
    return TokenType::IDENTIFIER;
}

bool KeywordMap::isKeyword(const std::string& word) {
    return keywordMap_.find(word) != keywordMap_.end();
}

std::vector<std::string> KeywordMap::getAllKeywords() {
    std::vector<std::string> keywords;
    for (const auto& pair : keywordMap_) {
        keywords.push_back(pair.first);
    }
    return keywords;
}

} // namespace CHTL