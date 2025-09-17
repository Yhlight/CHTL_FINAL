#include "Token.h"
#include <sstream>
#include <unordered_map>

namespace CHTL {

Token::Token(TokenType type, const std::string& value, size_t line, size_t column)
    : type_(type), value_(value), line_(line), column_(column) {
}

bool Token::isKeyword() const {
    switch (type_) {
        case TokenType::TEMPLATE:
        case TokenType::CUSTOM:
        case TokenType::ORIGIN:
        case TokenType::IMPORT:
        case TokenType::NAMESPACE:
        case TokenType::CONFIGURATION:
        case TokenType::INFO:
        case TokenType::EXPORT:
        case TokenType::STYLE:
        case TokenType::ELEMENT:
        case TokenType::VAR:
        case TokenType::HTML:
        case TokenType::JAVASCRIPT:
        case TokenType::CHTL:
        case TokenType::CJMOD:
        case TokenType::CONFIG:
        case TokenType::INHERIT:
        case TokenType::DELETE:
        case TokenType::INSERT:
        case TokenType::AFTER:
        case TokenType::BEFORE:
        case TokenType::REPLACE:
        case TokenType::AT_TOP:
        case TokenType::AT_BOTTOM:
        case TokenType::FROM:
        case TokenType::AS:
        case TokenType::EXCEPT:
        case TokenType::USE:
        case TokenType::HTML5:
        case TokenType::TEXT:
        case TokenType::SCRIPT:
        case TokenType::VIR:
        case TokenType::LISTEN:
        case TokenType::ANIMATE:
        case TokenType::ROUTER:
        case TokenType::SCRIPT_LOADER:
        case TokenType::UTIL:
        case TokenType::CHANGE:
        case TokenType::THEN:
            return true;
        default:
            return false;
    }
}

bool Token::isOperator() const {
    switch (type_) {
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::MULTIPLY:
        case TokenType::DIVIDE:
        case TokenType::MODULO:
        case TokenType::POWER:
        case TokenType::AND:
        case TokenType::OR:
        case TokenType::NOT:
        case TokenType::LESS:
        case TokenType::GREATER:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER_EQUAL:
        case TokenType::EQUAL:
        case TokenType::NOT_EQUAL:
            return true;
        default:
            return false;
    }
}

bool Token::isSymbol() const {
    switch (type_) {
        case TokenType::LEFT_BRACE:
        case TokenType::RIGHT_BRACE:
        case TokenType::LEFT_PAREN:
        case TokenType::RIGHT_PAREN:
        case TokenType::LEFT_BRACKET:
        case TokenType::RIGHT_BRACKET:
        case TokenType::SEMICOLON:
        case TokenType::COLON:
        case TokenType::EQUALS:
        case TokenType::COMMA:
        case TokenType::DOT:
        case TokenType::HASH:
        case TokenType::AT:
        case TokenType::AMPERSAND:
        case TokenType::ARROW:
        case TokenType::QUESTION:
        case TokenType::EXCLAMATION:
        case TokenType::UNDERSCORE:
        case TokenType::DOLLAR:
            return true;
        default:
            return false;
    }
}

std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token(" << GlobalMap::getTokenTypeName(type_) 
        << ", \"" << value_ << "\", " << line_ << ":" << column_ << ")";
    return oss.str();
}

// 全局映射表实现
std::unordered_map<std::string, TokenType> GlobalMap::keywordMap_;
std::unordered_map<std::string, TokenType> GlobalMap::operatorMap_;

void GlobalMap::initialize() {
    // 初始化关键字映射
    keywordMap_ = {
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
        {"@Config", TokenType::CONFIG},
        
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
        {"script", TokenType::SCRIPT},
        
        // CHTL JS关键字
        {"Vir", TokenType::VIR},
        {"Listen", TokenType::LISTEN},
        {"Animate", TokenType::ANIMATE},
        {"Router", TokenType::ROUTER},
        {"ScriptLoader", TokenType::SCRIPT_LOADER},
        {"util", TokenType::UTIL},
        {"change", TokenType::CHANGE},
        {"then", TokenType::THEN}
    };
    
    // 初始化运算符映射
    operatorMap_ = {
        {"+", TokenType::PLUS},
        {"-", TokenType::MINUS},
        {"*", TokenType::MULTIPLY},
        {"/", TokenType::DIVIDE},
        {"%", TokenType::MODULO},
        {"**", TokenType::POWER},
        {"&&", TokenType::AND},
        {"||", TokenType::OR},
        {"!", TokenType::NOT},
        {"<", TokenType::LESS},
        {">", TokenType::GREATER},
        {"<=", TokenType::LESS_EQUAL},
        {">=", TokenType::GREATER_EQUAL},
        {"==", TokenType::EQUAL},
        {"!=", TokenType::NOT_EQUAL},
        {"->", TokenType::ARROW}
    };
}

TokenType GlobalMap::getKeywordType(const std::string& keyword) {
    auto it = keywordMap_.find(keyword);
    return (it != keywordMap_.end()) ? it->second : TokenType::UNKNOWN;
}

TokenType GlobalMap::getOperatorType(const std::string& op) {
    auto it = operatorMap_.find(op);
    return (it != operatorMap_.end()) ? it->second : TokenType::UNKNOWN;
}

bool GlobalMap::isKeyword(const std::string& word) {
    return keywordMap_.find(word) != keywordMap_.end();
}

bool GlobalMap::isOperator(const std::string& op) {
    return operatorMap_.find(op) != operatorMap_.end();
}

std::string GlobalMap::getTokenTypeName(TokenType type) {
    switch (type) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING: return "STRING";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::LITERAL: return "LITERAL";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COLON: return "COLON";
        case TokenType::EQUALS: return "EQUALS";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::HASH: return "HASH";
        case TokenType::AT: return "AT";
        case TokenType::AMPERSAND: return "AMPERSAND";
        case TokenType::ARROW: return "ARROW";
        case TokenType::QUESTION: return "QUESTION";
        case TokenType::EXCLAMATION: return "EXCLAMATION";
        case TokenType::UNDERSCORE: return "UNDERSCORE";
        case TokenType::DOLLAR: return "DOLLAR";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::MULTIPLY: return "MULTIPLY";
        case TokenType::DIVIDE: return "DIVIDE";
        case TokenType::MODULO: return "MODULO";
        case TokenType::POWER: return "POWER";
        case TokenType::AND: return "AND";
        case TokenType::OR: return "OR";
        case TokenType::NOT: return "NOT";
        case TokenType::LESS: return "LESS";
        case TokenType::GREATER: return "GREATER";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::NOT_EQUAL: return "NOT_EQUAL";
        case TokenType::TEMPLATE: return "TEMPLATE";
        case TokenType::CUSTOM: return "CUSTOM";
        case TokenType::ORIGIN: return "ORIGIN";
        case TokenType::IMPORT: return "IMPORT";
        case TokenType::NAMESPACE: return "NAMESPACE";
        case TokenType::CONFIGURATION: return "CONFIGURATION";
        case TokenType::INFO: return "INFO";
        case TokenType::EXPORT: return "EXPORT";
        case TokenType::STYLE: return "STYLE";
        case TokenType::ELEMENT: return "ELEMENT";
        case TokenType::VAR: return "VAR";
        case TokenType::HTML: return "HTML";
        case TokenType::JAVASCRIPT: return "JAVASCRIPT";
        case TokenType::CHTL: return "CHTL";
        case TokenType::CJMOD: return "CJMOD";
        case TokenType::CONFIG: return "CONFIG";
        case TokenType::INHERIT: return "INHERIT";
        case TokenType::DELETE: return "DELETE";
        case TokenType::INSERT: return "INSERT";
        case TokenType::AFTER: return "AFTER";
        case TokenType::BEFORE: return "BEFORE";
        case TokenType::REPLACE: return "REPLACE";
        case TokenType::AT_TOP: return "AT_TOP";
        case TokenType::AT_BOTTOM: return "AT_BOTTOM";
        case TokenType::FROM: return "FROM";
        case TokenType::AS: return "AS";
        case TokenType::EXCEPT: return "EXCEPT";
        case TokenType::USE: return "USE";
        case TokenType::HTML5: return "HTML5";
        case TokenType::TEXT: return "TEXT";
        case TokenType::SCRIPT: return "SCRIPT";
        case TokenType::VIR: return "VIR";
        case TokenType::LISTEN: return "LISTEN";
        case TokenType::ANIMATE: return "ANIMATE";
        case TokenType::ROUTER: return "ROUTER";
        case TokenType::SCRIPT_LOADER: return "SCRIPT_LOADER";
        case TokenType::UTIL: return "UTIL";
        case TokenType::CHANGE: return "CHANGE";
        case TokenType::THEN: return "THEN";
        case TokenType::COMMENT: return "COMMENT";
        case TokenType::MULTILINE_COMMENT: return "MULTILINE_COMMENT";
        case TokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        case TokenType::PLACEHOLDER: return "PLACEHOLDER";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

} // namespace CHTL