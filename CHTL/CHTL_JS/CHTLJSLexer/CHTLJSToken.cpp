#include "CHTLJSToken.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace CHTL_JS {

CHTLJSToken::CHTLJSToken() 
    : type(CHTLJSTokenType::UNKNOWN), line(0), column(0), position(0) {
}

CHTLJSToken::CHTLJSToken(CHTLJSTokenType type, const std::string& value, int line, int column, int position)
    : type(type), value(value), line(line), column(column), position(position) {
}

void CHTLJSToken::setAttribute(const std::string& key, const std::string& value) {
    attributes[key] = value;
}

std::string CHTLJSToken::getAttribute(const std::string& key) const {
    auto it = attributes.find(key);
    return it != attributes.end() ? it->second : "";
}

bool CHTLJSToken::hasAttribute(const std::string& key) const {
    return attributes.find(key) != attributes.end();
}

void CHTLJSToken::removeAttribute(const std::string& key) {
    attributes.erase(key);
}

bool CHTLJSToken::isOperator() const {
    return isArithmeticOperator() || isComparisonOperator() || 
           isLogicalOperator() || isBitwiseOperator() || 
           isAssignmentOperator() || isUnaryOperator() || 
           isBinaryOperator() || isTernaryOperator();
}

bool CHTLJSToken::isKeyword() const {
    switch (type) {
        case CHTLJSTokenType::IF:
        case CHTLJSTokenType::ELSE:
        case CHTLJSTokenType::FOR:
        case CHTLJSTokenType::WHILE:
        case CHTLJSTokenType::DO:
        case CHTLJSTokenType::SWITCH:
        case CHTLJSTokenType::CASE:
        case CHTLJSTokenType::DEFAULT:
        case CHTLJSTokenType::BREAK:
        case CHTLJSTokenType::CONTINUE:
        case CHTLJSTokenType::RETURN:
        case CHTLJSTokenType::FUNCTION:
        case CHTLJSTokenType::CONST:
        case CHTLJSTokenType::LET:
        case CHTLJSTokenType::VAR:
        case CHTLJSTokenType::TRUE:
        case CHTLJSTokenType::FALSE:
        case CHTLJSTokenType::NULL:
        case CHTLJSTokenType::UNDEFINED:
        case CHTLJSTokenType::THIS:
        case CHTLJSTokenType::NEW:
        case CHTLJSTokenType::TYPEOF:
        case CHTLJSTokenType::INSTANCEOF:
        case CHTLJSTokenType::IN:
        case CHTLJSTokenType::OF:
        case CHTLJSTokenType::WITH:
        case CHTLJSTokenType::TRY:
        case CHTLJSTokenType::CATCH:
        case CHTLJSTokenType::FINALLY:
        case CHTLJSTokenType::THROW:
            return true;
        default:
            return false;
    }
}

bool CHTLJSToken::isPunctuation() const {
    switch (type) {
        case CHTLJSTokenType::LEFT_PAREN:
        case CHTLJSTokenType::RIGHT_PAREN:
        case CHTLJSTokenType::LEFT_BRACKET:
        case CHTLJSTokenType::RIGHT_BRACKET:
        case CHTLJSTokenType::LEFT_BRACE:
        case CHTLJSTokenType::RIGHT_BRACE:
        case CHTLJSTokenType::SEMICOLON:
        case CHTLJSTokenType::COMMA:
        case CHTLJSTokenType::DOT:
        case CHTLJSTokenType::COLON:
        case CHTLJSTokenType::QUESTION:
            return true;
        default:
            return false;
    }
}

bool CHTLJSToken::isComment() const {
    return type == CHTLJSTokenType::SINGLE_COMMENT || 
           type == CHTLJSTokenType::MULTI_COMMENT || 
           type == CHTLJSTokenType::GENERATOR_COMMENT;
}

std::string CHTLJSToken::getTypeName() const {
    switch (type) {
        case CHTLJSTokenType::IDENTIFIER: return "IDENTIFIER";
        case CHTLJSTokenType::LITERAL: return "LITERAL";
        case CHTLJSTokenType::STRING: return "STRING";
        case CHTLJSTokenType::NUMBER: return "NUMBER";
        case CHTLJSTokenType::BOOLEAN: return "BOOLEAN";
        case CHTLJSTokenType::ASSIGN: return "ASSIGN";
        case CHTLJSTokenType::ARROW: return "ARROW";
        case CHTLJSTokenType::DOT: return "DOT";
        case CHTLJSTokenType::COLON: return "COLON";
        case CHTLJSTokenType::SEMICOLON: return "SEMICOLON";
        case CHTLJSTokenType::COMMA: return "COMMA";
        case CHTLJSTokenType::PLUS: return "PLUS";
        case CHTLJSTokenType::MINUS: return "MINUS";
        case CHTLJSTokenType::MULTIPLY: return "MULTIPLY";
        case CHTLJSTokenType::DIVIDE: return "DIVIDE";
        case CHTLJSTokenType::MODULO: return "MODULO";
        case CHTLJSTokenType::POWER: return "POWER";
        case CHTLJSTokenType::EQUAL: return "EQUAL";
        case CHTLJSTokenType::NOT_EQUAL: return "NOT_EQUAL";
        case CHTLJSTokenType::LESS_THAN: return "LESS_THAN";
        case CHTLJSTokenType::GREATER_THAN: return "GREATER_THAN";
        case CHTLJSTokenType::LESS_EQUAL: return "LESS_EQUAL";
        case CHTLJSTokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case CHTLJSTokenType::AND: return "AND";
        case CHTLJSTokenType::OR: return "OR";
        case CHTLJSTokenType::NOT: return "NOT";
        case CHTLJSTokenType::BITWISE_AND: return "BITWISE_AND";
        case CHTLJSTokenType::BITWISE_OR: return "BITWISE_OR";
        case CHTLJSTokenType::BITWISE_XOR: return "BITWISE_XOR";
        case CHTLJSTokenType::BITWISE_NOT: return "BITWISE_NOT";
        case CHTLJSTokenType::LEFT_SHIFT: return "LEFT_SHIFT";
        case CHTLJSTokenType::RIGHT_SHIFT: return "RIGHT_SHIFT";
        case CHTLJSTokenType::LEFT_PAREN: return "LEFT_PAREN";
        case CHTLJSTokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case CHTLJSTokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case CHTLJSTokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case CHTLJSTokenType::LEFT_BRACE: return "LEFT_BRACE";
        case CHTLJSTokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case CHTLJSTokenType::QUESTION: return "QUESTION";
        case CHTLJSTokenType::TERNARY: return "TERNARY";
        case CHTLJSTokenType::NULLISH: return "NULLISH";
        case CHTLJSTokenType::OPTIONAL: return "OPTIONAL";
        case CHTLJSTokenType::ENHANCED_SELECTOR: return "ENHANCED_SELECTOR";
        case CHTLJSTokenType::VIRTUAL_OBJECT: return "VIRTUAL_OBJECT";
        case CHTLJSTokenType::LISTEN: return "LISTEN";
        case CHTLJSTokenType::DELEGATE: return "DELEGATE";
        case CHTLJSTokenType::ANIMATE: return "ANIMATE";
        case CHTLJSTokenType::ROUTER: return "ROUTER";
        case CHTLJSTokenType::FILELOADER: return "FILELOADER";
        case CHTLJSTokenType::UTIL: return "UTIL";
        case CHTLJSTokenType::IF: return "IF";
        case CHTLJSTokenType::ELSE: return "ELSE";
        case CHTLJSTokenType::FOR: return "FOR";
        case CHTLJSTokenType::WHILE: return "WHILE";
        case CHTLJSTokenType::DO: return "DO";
        case CHTLJSTokenType::SWITCH: return "SWITCH";
        case CHTLJSTokenType::CASE: return "CASE";
        case CHTLJSTokenType::DEFAULT: return "DEFAULT";
        case CHTLJSTokenType::BREAK: return "BREAK";
        case CHTLJSTokenType::CONTINUE: return "CONTINUE";
        case CHTLJSTokenType::RETURN: return "RETURN";
        case CHTLJSTokenType::FUNCTION: return "FUNCTION";
        case CHTLJSTokenType::CONST: return "CONST";
        case CHTLJSTokenType::LET: return "LET";
        case CHTLJSTokenType::VAR: return "VAR";
        case CHTLJSTokenType::TRUE: return "TRUE";
        case CHTLJSTokenType::FALSE: return "FALSE";
        case CHTLJSTokenType::NULL: return "NULL";
        case CHTLJSTokenType::UNDEFINED: return "UNDEFINED";
        case CHTLJSTokenType::THIS: return "THIS";
        case CHTLJSTokenType::NEW: return "NEW";
        case CHTLJSTokenType::TYPEOF: return "TYPEOF";
        case CHTLJSTokenType::INSTANCEOF: return "INSTANCEOF";
        case CHTLJSTokenType::IN: return "IN";
        case CHTLJSTokenType::OF: return "OF";
        case CHTLJSTokenType::WITH: return "WITH";
        case CHTLJSTokenType::TRY: return "TRY";
        case CHTLJSTokenType::CATCH: return "CATCH";
        case CHTLJSTokenType::FINALLY: return "FINALLY";
        case CHTLJSTokenType::THROW: return "THROW";
        case CHTLJSTokenType::SINGLE_COMMENT: return "SINGLE_COMMENT";
        case CHTLJSTokenType::MULTI_COMMENT: return "MULTI_COMMENT";
        case CHTLJSTokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        case CHTLJSTokenType::END_OF_FILE: return "END_OF_FILE";
        case CHTLJSTokenType::ERROR: return "ERROR";
        case CHTLJSTokenType::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

std::string CHTLJSToken::toString() const {
    return value;
}

std::string CHTLJSToken::toDebugString() const {
    std::ostringstream oss;
    oss << "Token{type=" << getTypeName() 
        << ", value='" << value 
        << "', line=" << line 
        << ", column=" << column 
        << ", position=" << position << "}";
    return oss.str();
}

bool CHTLJSToken::operator==(const CHTLJSToken& other) const {
    return type == other.type && value == other.value;
}

bool CHTLJSToken::operator!=(const CHTLJSToken& other) const {
    return !(*this == other);
}

bool CHTLJSToken::operator==(CHTLJSTokenType type) const {
    return this->type == type;
}

bool CHTLJSToken::operator!=(CHTLJSTokenType type) const {
    return this->type != type;
}

CHTLJSToken CHTLJSToken::clone() const {
    CHTLJSToken token(type, value, line, column, position);
    token.attributes = attributes;
    return token;
}

bool CHTLJSToken::isValid() const {
    return type != CHTLJSTokenType::ERROR && type != CHTLJSTokenType::UNKNOWN;
}

bool CHTLJSToken::isComplete() const {
    return isValid() && !value.empty();
}

std::string CHTLJSToken::toJavaScript() const {
    return convertToJavaScript();
}

std::string CHTLJSToken::toCHTLJS() const {
    return convertToCHTLJS();
}

std::string CHTLJSToken::toHTML() const {
    return convertToHTML();
}

std::string CHTLJSToken::toCSS() const {
    return convertToCSS();
}

std::string CHTLJSToken::format() const {
    return formatToken();
}

std::string CHTLJSToken::minify() const {
    return minifyToken();
}

std::string CHTLJSToken::beautify() const {
    return beautifyToken();
}

std::string CHTLJSToken::compress() const {
    return compressToken();
}

std::string CHTLJSToken::decompress() const {
    return decompressToken();
}

std::string CHTLJSToken::encode() const {
    return encodeToken();
}

std::string CHTLJSToken::decode() const {
    return decodeToken();
}

// 辅助方法实现
bool CHTLJSToken::isArithmeticOperator() const {
    switch (type) {
        case CHTLJSTokenType::PLUS:
        case CHTLJSTokenType::MINUS:
        case CHTLJSTokenType::MULTIPLY:
        case CHTLJSTokenType::DIVIDE:
        case CHTLJSTokenType::MODULO:
        case CHTLJSTokenType::POWER:
            return true;
        default:
            return false;
    }
}

bool CHTLJSToken::isComparisonOperator() const {
    switch (type) {
        case CHTLJSTokenType::EQUAL:
        case CHTLJSTokenType::NOT_EQUAL:
        case CHTLJSTokenType::LESS_THAN:
        case CHTLJSTokenType::GREATER_THAN:
        case CHTLJSTokenType::LESS_EQUAL:
        case CHTLJSTokenType::GREATER_EQUAL:
            return true;
        default:
            return false;
    }
}

bool CHTLJSToken::isLogicalOperator() const {
    switch (type) {
        case CHTLJSTokenType::AND:
        case CHTLJSTokenType::OR:
        case CHTLJSTokenType::NOT:
            return true;
        default:
            return false;
    }
}

bool CHTLJSToken::isBitwiseOperator() const {
    switch (type) {
        case CHTLJSTokenType::BITWISE_AND:
        case CHTLJSTokenType::BITWISE_OR:
        case CHTLJSTokenType::BITWISE_XOR:
        case CHTLJSTokenType::BITWISE_NOT:
        case CHTLJSTokenType::LEFT_SHIFT:
        case CHTLJSTokenType::RIGHT_SHIFT:
            return true;
        default:
            return false;
    }
}

bool CHTLJSToken::isAssignmentOperator() const {
    return type == CHTLJSTokenType::ASSIGN;
}

bool CHTLJSToken::isUnaryOperator() const {
    switch (type) {
        case CHTLJSTokenType::NOT:
        case CHTLJSTokenType::BITWISE_NOT:
        case CHTLJSTokenType::MINUS:
        case CHTLJSTokenType::PLUS:
            return true;
        default:
            return false;
    }
}

bool CHTLJSToken::isBinaryOperator() const {
    return isArithmeticOperator() || isComparisonOperator() || 
           isLogicalOperator() || isBitwiseOperator();
}

bool CHTLJSToken::isTernaryOperator() const {
    return type == CHTLJSTokenType::TERNARY || type == CHTLJSTokenType::QUESTION;
}

// 类型转换实现
std::string CHTLJSToken::convertToJavaScript() const {
    switch (type) {
        case CHTLJSTokenType::ENHANCED_SELECTOR:
            // 将{{选择器}}转换为JavaScript选择器代码
            return "document.querySelector('" + value + "')";
        case CHTLJSTokenType::ARROW:
            return "=>";
        case CHTLJSTokenType::POWER:
            return "**";
        case CHTLJSTokenType::NULLISH:
            return "??";
        case CHTLJSTokenType::OPTIONAL:
            return "?.";
        default:
            return value;
    }
}

std::string CHTLJSToken::convertToCHTLJS() const {
    // CHTL JS特有的转换逻辑
    return value;
}

std::string CHTLJSToken::convertToHTML() const {
    // 转换为HTML实体
    std::string result = value;
    std::replace(result.begin(), result.end(), '<', '&lt;');
    std::replace(result.begin(), result.end(), '>', '&gt;');
    std::replace(result.begin(), result.end(), '&', '&amp;');
    std::replace(result.begin(), result.end(), '"', '&quot;');
    std::replace(result.begin(), result.end(), '\'', '&#39;');
    return result;
}

std::string CHTLJSToken::convertToCSS() const {
    // CSS转换逻辑
    return value;
}

// 格式化实现
std::string CHTLJSToken::formatToken() const {
    return value;
}

std::string CHTLJSToken::minifyToken() const {
    std::string result = value;
    // 移除多余空格
    result.erase(std::remove_if(result.begin(), result.end(), ::isspace), result.end());
    return result;
}

std::string CHTLJSToken::beautifyToken() const {
    return value;
}

// 压缩实现
std::string CHTLJSToken::compressToken() const {
    return value;
}

std::string CHTLJSToken::decompressToken() const {
    return value;
}

// 编码实现
std::string CHTLJSToken::encodeToken() const {
    return value;
}

std::string CHTLJSToken::decodeToken() const {
    return value;
}

} // namespace CHTL_JS