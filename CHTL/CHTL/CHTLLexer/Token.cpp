#include "Token.hpp"
#include <sstream>
#include <algorithm>

namespace CHTL {

bool Token::isKeyword() const {
    return KeywordMap::isKeyword(value_);
}

bool Token::isOperator() const {
    return OperatorMap::isOperator(value_);
}

bool Token::isLiteral() const {
    return type_ == TokenType::STRING_LITERAL || 
           type_ == TokenType::UNQUOTED_LITERAL ||
           type_ == TokenType::NUMBER;
}

bool Token::isComment() const {
    return type_ == TokenType::SINGLE_COMMENT ||
           type_ == TokenType::MULTI_COMMENT ||
           type_ == TokenType::GENERATOR_COMMENT;
}

std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token(" << static_cast<int>(type_) << ", \"" << value_ 
        << "\", " << line_ << ":" << column_ << ")";
    return oss.str();
}

// 关键字映射表实现
const std::unordered_map<std::string, TokenType>& KeywordMap::getKeywords() {
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"text", TokenType::TEXT},
        {"style", TokenType::STYLE},
        {"script", TokenType::SCRIPT},
        {"[Template]", TokenType::TEMPLATE},
        {"[Custom]", TokenType::CUSTOM},
        {"[Origin]", TokenType::ORIGIN},
        {"[Import]", TokenType::IMPORT},
        {"[Namespace]", TokenType::NAMESPACE},
        {"[Configuration]", TokenType::CONFIGURATION},
        {"[Info]", TokenType::INFO},
        {"[Export]", TokenType::EXPORT},
        {"use", TokenType::USE},
        {"except", TokenType::EXCEPT},
        {"delete", TokenType::DELETE},
        {"insert", TokenType::INSERT},
        {"inherit", TokenType::INHERIT},
        {"from", TokenType::FROM},
        {"as", TokenType::AS},
        {"after", TokenType::AFTER},
        {"before", TokenType::BEFORE},
        {"replace", TokenType::REPLACE},
        {"at", TokenType::AT_TOP}, // 需要特殊处理 "at top"
        {"top", TokenType::AT_TOP},
        {"bottom", TokenType::AT_BOTTOM},
        {"html5", TokenType::HTML5},
        {"@Style", TokenType::TEMPLATE_STYLE},
        {"Style", TokenType::STYLE},
        {"@Element", TokenType::TEMPLATE_ELEMENT},
        {"@Var", TokenType::TEMPLATE_VAR},
        {"@Html", TokenType::ORIGIN_HTML},
        {"@JavaScript", TokenType::ORIGIN_JAVASCRIPT},
        {"@Chtl", TokenType::ORIGIN_CHTL},
        {"@CJmod", TokenType::ORIGIN_CJMOD}
    };
    return keywords;
}

TokenType KeywordMap::getKeywordType(const std::string& keyword) {
    const auto& keywords = getKeywords();
    auto it = keywords.find(keyword);
    return (it != keywords.end()) ? it->second : TokenType::IDENTIFIER;
}

bool KeywordMap::isKeyword(const std::string& word) {
    const auto& keywords = getKeywords();
    return keywords.find(word) != keywords.end();
}

// 操作符映射表实现
const std::unordered_map<std::string, TokenType>& OperatorMap::getOperators() {
    static const std::unordered_map<std::string, TokenType> operators = {
        {"{", TokenType::LEFT_BRACE},
        {"}", TokenType::RIGHT_BRACE},
        {"[", TokenType::LEFT_BRACKET},
        {"]", TokenType::RIGHT_BRACKET},
        {"(", TokenType::LEFT_PAREN},
        {")", TokenType::RIGHT_PAREN},
        {";", TokenType::SEMICOLON},
        {":", TokenType::COLON},
        {"=", TokenType::EQUAL},
        {",", TokenType::COMMA},
        {".", TokenType::DOT},
        {"#", TokenType::HASH},
        {"@", TokenType::AT},
        {"&", TokenType::AMPERSAND},
        {"?", TokenType::QUESTION},
        {"!", TokenType::EXCLAMATION},
        {"_", TokenType::UNDERSCORE},
        {"/", TokenType::SLASH},
        {"\\", TokenType::BACKSLASH},
        {"|", TokenType::PIPE},
        {"~", TokenType::TILDE},
        {"^", TokenType::CARET},
        {"$", TokenType::DOLLAR},
        {"%", TokenType::PERCENT},
        {"->", TokenType::ARROW},
        {"::", TokenType::DOUBLE_COLON},
        {"...", TokenType::TRIPLE_DOT},
        
        // 算术运算符
        {"+", TokenType::PLUS},
        {"-", TokenType::MINUS},
        {"*", TokenType::MULTIPLY},
        {"/", TokenType::DIVIDE},
        {"%", TokenType::MODULO},
        {"**", TokenType::POWER},
        
        // 比较运算符
        {"<", TokenType::LESS_THAN},
        {">", TokenType::GREATER_THAN},
        {"<=", TokenType::LESS_EQUAL},
        {">=", TokenType::GREATER_EQUAL},
        {"==", TokenType::EQUAL_EQUAL},
        {"!=", TokenType::NOT_EQUAL},
        
        // 逻辑运算符
        {"&&", TokenType::LOGICAL_AND},
        {"||", TokenType::LOGICAL_OR},
        {"!", TokenType::LOGICAL_NOT}
    };
    return operators;
}

TokenType OperatorMap::getOperatorType(const std::string& op) {
    const auto& operators = getOperators();
    auto it = operators.find(op);
    return (it != operators.end()) ? it->second : TokenType::ERROR;
}

bool OperatorMap::isOperator(const std::string& word) {
    const auto& operators = getOperators();
    return operators.find(word) != operators.end();
}

} // namespace CHTL