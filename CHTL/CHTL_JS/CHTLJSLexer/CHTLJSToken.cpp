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
    return isAssignmentOperator() || isArrowOperator();
}

bool CHTLJSToken::isKeyword() const {
    switch (type) {
        // 基本关键字（仅CHTL JS需要的）
        case CHTLJSTokenType::TRUE:
        case CHTLJSTokenType::FALSE:
        case CHTLJSTokenType::NULL_VALUE:
        case CHTLJSTokenType::UNDEFINED:
        
        // CHTL JS特有关键字
        case CHTLJSTokenType::VIRTUAL_OBJECT:
        case CHTLJSTokenType::LISTEN:
        case CHTLJSTokenType::DELEGATE:
        case CHTLJSTokenType::ANIMATE:
        case CHTLJSTokenType::ROUTER:
        case CHTLJSTokenType::FILELOADER:
        case CHTLJSTokenType::INEVERAWAY:
        case CHTLJSTokenType::UTIL:
        case CHTLJSTokenType::THEN:
        case CHTLJSTokenType::CHANGE:
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
        case CHTLJSTokenType::COLON:
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
        case CHTLJSTokenType::COLON: return "COLON";
        case CHTLJSTokenType::SEMICOLON: return "SEMICOLON";
        case CHTLJSTokenType::COMMA: return "COMMA";
        case CHTLJSTokenType::LEFT_PAREN: return "LEFT_PAREN";
        case CHTLJSTokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case CHTLJSTokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case CHTLJSTokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case CHTLJSTokenType::LEFT_BRACE: return "LEFT_BRACE";
        case CHTLJSTokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case CHTLJSTokenType::ENHANCED_SELECTOR: return "ENHANCED_SELECTOR";
        case CHTLJSTokenType::VIRTUAL_OBJECT: return "VIRTUAL_OBJECT";
        case CHTLJSTokenType::LISTEN: return "LISTEN";
        case CHTLJSTokenType::DELEGATE: return "DELEGATE";
        case CHTLJSTokenType::ANIMATE: return "ANIMATE";
        case CHTLJSTokenType::ROUTER: return "ROUTER";
        case CHTLJSTokenType::FILELOADER: return "FILELOADER";
        case CHTLJSTokenType::INEVERAWAY: return "INEVERAWAY";
        case CHTLJSTokenType::UTIL: return "UTIL";
        case CHTLJSTokenType::THEN: return "THEN";
        case CHTLJSTokenType::CHANGE: return "CHANGE";
        case CHTLJSTokenType::TRUE: return "TRUE";
        case CHTLJSTokenType::FALSE: return "FALSE";
        case CHTLJSTokenType::NULL_VALUE: return "NULL";
        case CHTLJSTokenType::UNDEFINED: return "UNDEFINED";
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
bool CHTLJSToken::isAssignmentOperator() const {
    return type == CHTLJSTokenType::ASSIGN;
}

bool CHTLJSToken::isArrowOperator() const {
    return type == CHTLJSTokenType::ARROW;
}

// 类型转换实现
std::string CHTLJSToken::convertToJavaScript() const {
    switch (type) {
        case CHTLJSTokenType::ENHANCED_SELECTOR:
            // 将{{选择器}}转换为JavaScript选择器代码
            return "document.querySelector('" + value + "')";
        case CHTLJSTokenType::ARROW:
            return "->";
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
    
    // 替换HTML特殊字符
    size_t pos = 0;
    while ((pos = result.find('<', pos)) != std::string::npos) {
        result.replace(pos, 1, "&lt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = result.find('>', pos)) != std::string::npos) {
        result.replace(pos, 1, "&gt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = result.find('&', pos)) != std::string::npos) {
        result.replace(pos, 1, "&amp;");
        pos += 5;
    }
    
    pos = 0;
    while ((pos = result.find('"', pos)) != std::string::npos) {
        result.replace(pos, 1, "&quot;");
        pos += 6;
    }
    
    pos = 0;
    while ((pos = result.find('\'', pos)) != std::string::npos) {
        result.replace(pos, 1, "&#39;");
        pos += 5;
    }
    
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