#include "Token.h"
#include <sstream>
#include <unordered_map>

namespace CHTL {

bool Token::isKeyword() const {
    return KeywordMap::isKeyword(value_);
}

bool Token::isOperator() const {
    switch (type_) {
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::MULTIPLY:
        case TokenType::DIVIDE:
        case TokenType::MODULO:
        case TokenType::LESS:
        case TokenType::GREATER:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER_EQUAL:
        case TokenType::EQUAL:
        case TokenType::NOT_EQUAL:
        case TokenType::AND:
        case TokenType::OR:
        case TokenType::NOT:
        case TokenType::QUESTION:
            return true;
        default:
            return false;
    }
}

bool Token::isSeparator() const {
    switch (type_) {
        case TokenType::LEFT_BRACE:
        case TokenType::RIGHT_BRACE:
        case TokenType::LEFT_BRACKET:
        case TokenType::RIGHT_BRACKET:
        case TokenType::LEFT_PAREN:
        case TokenType::RIGHT_PAREN:
        case TokenType::SEMICOLON:
        case TokenType::COLON:
        case TokenType::EQUALS:
        case TokenType::COMMA:
        case TokenType::DOT:
        case TokenType::HASH:
        case TokenType::AMPERSAND:
        case TokenType::ARROW:
            return true;
        default:
            return false;
    }
}

std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token(" << static_cast<int>(type_) << ", \"" << value_ << "\", " 
        << line_ << ":" << column_ << ")";
    return oss.str();
}

const std::unordered_map<std::string, TokenType>& KeywordMap::getKeywords() {
    static const std::unordered_map<std::string, TokenType> keywords = {
        // 模板和自定义关键字
        {"[Template]", TokenType::TEMPLATE},
        {"[Custom]", TokenType::CUSTOM},
        {"[Origin]", TokenType::ORIGIN},
        {"[Import]", TokenType::IMPORT},
        {"[Namespace]", TokenType::NAMESPACE},
        {"[Configuration]", TokenType::CONFIGURATION},
        {"[Info]", TokenType::INFO},
        {"[Export]", TokenType::EXPORT},
        
        // 类型关键字
        {"@Style", TokenType::AT_STYLE},
        {"@style", TokenType::AT_STYLE},
        {"@CSS", TokenType::AT_STYLE},
        {"@Css", TokenType::AT_STYLE},
        {"@css", TokenType::AT_STYLE},
        {"@Element", TokenType::AT_ELEMENT},
        {"@Var", TokenType::AT_VAR},
        {"@Html", TokenType::AT_HTML},
        {"@JavaScript", TokenType::AT_JAVASCRIPT},
        {"@Chtl", TokenType::AT_CHTL},
        {"@Config", TokenType::AT_CONFIG},
        
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
        {"style", TokenType::STYLE},
        {"script", TokenType::SCRIPT}
    };
    return keywords;
}

TokenType KeywordMap::getKeywordType(const std::string& keyword) {
    const auto& keywords = getKeywords();
    auto it = keywords.find(keyword);
    return (it != keywords.end()) ? it->second : TokenType::UNKNOWN;
}

bool KeywordMap::isKeyword(const std::string& word) {
    return getKeywordType(word) != TokenType::UNKNOWN;
}

} // namespace CHTL