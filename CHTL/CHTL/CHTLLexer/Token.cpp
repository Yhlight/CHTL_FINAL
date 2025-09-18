#include "Token.h"
#include <unordered_map>

namespace CHTL {

// 静态成员定义
std::unordered_map<std::string, TokenType> KeywordMap::s_keywords;
bool KeywordMap::s_initialized = false;

void KeywordMap::initialize() {
    if (s_initialized) return;
    
    // 模板相关关键字
    s_keywords["[Template]"] = TokenType::TEMPLATE;
    s_keywords["[Custom]"] = TokenType::CUSTOM;
    s_keywords["[Origin]"] = TokenType::ORIGIN;
    s_keywords["[Import]"] = TokenType::IMPORT;
    s_keywords["[Namespace]"] = TokenType::NAMESPACE;
    s_keywords["[Configuration]"] = TokenType::CONFIGURATION;
    s_keywords["[Info]"] = TokenType::INFO;
    s_keywords["[Export]"] = TokenType::EXPORT;
    
    // 样式相关
    s_keywords["style"] = TokenType::STYLE;
    s_keywords["script"] = TokenType::SCRIPT;
    s_keywords["text"] = TokenType::TEXT;
    
    // 模板类型
    s_keywords["@Style"] = TokenType::AT_STYLE;
    s_keywords["@Element"] = TokenType::AT_ELEMENT;
    s_keywords["@Var"] = TokenType::AT_VAR;
    s_keywords["@Html"] = TokenType::AT_HTML;
    s_keywords["@JavaScript"] = TokenType::AT_JAVASCRIPT;
    s_keywords["@Chtl"] = TokenType::AT_CHTL;
    s_keywords["@Config"] = TokenType::AT_CONFIG;
    
    // 操作符
    s_keywords["inherit"] = TokenType::INHERIT;
    s_keywords["delete"] = TokenType::DELETE;
    s_keywords["insert"] = TokenType::INSERT;
    s_keywords["after"] = TokenType::AFTER;
    s_keywords["before"] = TokenType::BEFORE;
    s_keywords["replace"] = TokenType::REPLACE;
    s_keywords["at top"] = TokenType::AT_TOP;
    s_keywords["at bottom"] = TokenType::AT_BOTTOM;
    s_keywords["from"] = TokenType::FROM;
    s_keywords["as"] = TokenType::AS;
    s_keywords["except"] = TokenType::EXCEPT;
    s_keywords["use"] = TokenType::USE;
    
    s_initialized = true;
}

TokenType KeywordMap::getKeywordType(const std::string& keyword) {
    if (!s_initialized) {
        initialize();
    }
    
    auto it = s_keywords.find(keyword);
    if (it != s_keywords.end()) {
        return it->second;
    }
    return TokenType::UNKNOWN;
}

bool KeywordMap::isKeyword(const std::string& keyword) {
    return getKeywordType(keyword) != TokenType::UNKNOWN;
}

std::string KeywordMap::getKeywordName(TokenType type) {
    if (!s_initialized) {
        initialize();
    }
    
    for (const auto& pair : s_keywords) {
        if (pair.second == type) {
            return pair.first;
        }
    }
    return "";
}

bool TokenUtils::isIdentifier(const std::string& str) {
    if (str.empty()) return false;
    
    // 第一个字符必须是字母或下划线
    if (!std::isalpha(str[0]) && str[0] != '_') {
        return false;
    }
    
    // 其余字符必须是字母、数字或下划线
    for (size_t i = 1; i < str.length(); i++) {
        if (!std::isalnum(str[i]) && str[i] != '_') {
            return false;
        }
    }
    
    return true;
}

bool TokenUtils::isString(const std::string& str) {
    if (str.length() < 2) return false;
    
    char first = str[0];
    char last = str[str.length() - 1];
    
    return (first == '"' && last == '"') || (first == '\'' && last == '\'');
}

bool TokenUtils::isNumber(const std::string& str) {
    if (str.empty()) return false;
    
    bool hasDot = false;
    for (char c : str) {
        if (c == '.') {
            if (hasDot) return false; // 多个小数点
            hasDot = true;
        } else if (!std::isdigit(c)) {
            return false;
        }
    }
    
    return true;
}

bool TokenUtils::isLiteral(const std::string& str) {
    if (str.empty()) return false;
    
    // 字面量不能以数字开头
    if (std::isdigit(str[0])) return false;
    
    // 字面量不能包含引号
    if (str.find('"') != std::string::npos || str.find('\'') != std::string::npos) {
        return false;
    }
    
    // 字面量不能包含特殊字符（除了连字符和下划线）
    for (char c : str) {
        if (!std::isalnum(c) && c != '-' && c != '_' && c != '.') {
            return false;
        }
    }
    
    return true;
}

bool TokenUtils::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

bool TokenUtils::isNewline(char c) {
    return c == '\n' || c == '\r';
}

bool TokenUtils::isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || 
           c == '^' || c == '=' || c == '!' || c == '<' || c == '>' || 
           c == '&' || c == '|' || c == '?';
}

bool TokenUtils::isDelimiter(char c) {
    return c == '{' || c == '}' || c == '[' || c == ']' || 
           c == '(' || c == ')' || c == ';' || c == ':' || 
           c == ',' || c == '.' || c == '#';
}

std::string TokenUtils::escapeString(const std::string& str) {
    std::string result;
    result.reserve(str.length() * 2);
    
    for (char c : str) {
        switch (c) {
            case '"':
                result += "\\\"";
                break;
            case '\'':
                result += "\\'";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\t':
                result += "\\t";
                break;
            default:
                result += c;
                break;
        }
    }
    
    return result;
}

std::string TokenUtils::unescapeString(const std::string& str) {
    std::string result;
    result.reserve(str.length());
    
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '\\' && i + 1 < str.length()) {
            switch (str[i + 1]) {
                case '"':
                    result += '"';
                    i++; // 跳过下一个字符
                    break;
                case '\'':
                    result += '\'';
                    i++; // 跳过下一个字符
                    break;
                case '\\':
                    result += '\\';
                    i++; // 跳过下一个字符
                    break;
                case 'n':
                    result += '\n';
                    i++; // 跳过下一个字符
                    break;
                case 'r':
                    result += '\r';
                    i++; // 跳过下一个字符
                    break;
                case 't':
                    result += '\t';
                    i++; // 跳过下一个字符
                    break;
                default:
                    result += str[i];
                    break;
            }
        } else {
            result += str[i];
        }
    }
    
    return result;
}

} // namespace CHTL