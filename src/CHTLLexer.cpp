#include "CHTL/CHTLLexer.h"
#include <iostream>
#include <cctype>
#include <algorithm>
#include <sstream>

namespace CHTL {

CHTLLexer::CHTLLexer(std::shared_ptr<CHTLContext> context)
    : m_context(context)
    , m_debugMode(false)
{
    // 初始化关键字映射
    m_keywords = {
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
        {"inherit", TokenType::INHERIT},
        {"delete", TokenType::DELETE},
        {"insert", TokenType::INSERT},
        {"after", TokenType::AFTER},
        {"before", TokenType::BEFORE},
        {"replace", TokenType::REPLACE},
        {"at", TokenType::AT_TOP},
        {"top", TokenType::AT_TOP},
        {"bottom", TokenType::AT_BOTTOM},
        {"from", TokenType::FROM},
        {"as", TokenType::AS},
        {"except", TokenType::EXCEPT},
        {"html5", TokenType::HTML5}
    };
    
    // 初始化符号映射
    m_symbols = {
        {"{", TokenType::LEFT_BRACE},
        {"}", TokenType::RIGHT_BRACE},
        {"(", TokenType::LEFT_PAREN},
        {")", TokenType::RIGHT_PAREN},
        {"[", TokenType::LEFT_BRACKET},
        {"]", TokenType::RIGHT_BRACKET},
        {";", TokenType::SEMICOLON},
        {":", TokenType::COLON},
        {"=", TokenType::EQUALS},
        {",", TokenType::COMMA},
        {".", TokenType::DOT},
        {"#", TokenType::HASH},
        {"@", TokenType::AT},
        {"$", TokenType::DOLLAR},
        {"&", TokenType::AMPERSAND},
        {"|", TokenType::PIPE},
        {"?", TokenType::QUESTION},
        {"!", TokenType::EXCLAMATION},
        {"~", TokenType::TILDE},
        {"^", TokenType::CARET},
        {"<", TokenType::LESS_THAN},
        {">", TokenType::GREATER_THAN},
        {"+", TokenType::PLUS},
        {"-", TokenType::MINUS},
        {"*", TokenType::ASTERISK},
        {"/", TokenType::SLASH},
        {"%", TokenType::PERCENT},
        {"\\", TokenType::BACKSLASH}
    };
}

CHTLLexer::~CHTLLexer() {
}

TokenList CHTLLexer::tokenize(const std::string& sourceCode) {
    TokenList tokens;
    
    if (sourceCode.empty()) {
        return tokens;
    }
    
    size_t pos = 0;
    size_t line = 1, column = 1;
    
    while (pos < sourceCode.length()) {
        // 跳过空白字符
        size_t newPos = skipWhitespace(sourceCode, pos);
        if (newPos != pos) {
            pos = newPos;
            auto [newLine, newColumn] = getLineColumn(sourceCode, pos);
            line = newLine;
            column = newColumn;
            continue;
        }
        
        // 检查注释
        if (pos + 1 < sourceCode.length() && 
            sourceCode[pos] == '/' && sourceCode[pos + 1] == '/') {
            pos = skipSingleLineComment(sourceCode, pos);
            continue;
        }
        
        if (pos + 1 < sourceCode.length() && 
            sourceCode[pos] == '/' && sourceCode[pos + 1] == '*') {
            pos = skipMultiLineComment(sourceCode, pos);
            continue;
        }
        
        if (pos + 1 < sourceCode.length() && 
            sourceCode[pos] == '-' && sourceCode[pos + 1] == '-') {
            pos = skipGeneratorComment(sourceCode, pos);
            continue;
        }
        
        // 检查字符串
        if (sourceCode[pos] == '"' || sourceCode[pos] == '\'') {
            size_t startPos = pos;
            pos = skipString(sourceCode, pos, sourceCode[pos]);
            std::string value = sourceCode.substr(startPos, pos - startPos);
            auto [tokenLine, tokenColumn] = getLineColumn(sourceCode, startPos);
            tokens.addToken(TokenType::STRING, value, tokenLine, tokenColumn);
            continue;
        }
        
        // 检查数字
        if (std::isdigit(sourceCode[pos])) {
            size_t startPos = pos;
            pos = readNumber(sourceCode, pos);
            std::string value = sourceCode.substr(startPos, pos - startPos);
            auto [tokenLine, tokenColumn] = getLineColumn(sourceCode, startPos);
            tokens.addToken(TokenType::NUMBER, value, tokenLine, tokenColumn);
            continue;
        }
        
        // 检查标识符或关键字
        if (std::isalpha(sourceCode[pos]) || sourceCode[pos] == '_') {
            size_t startPos = pos;
            pos = readIdentifier(sourceCode, pos);
            std::string word = sourceCode.substr(startPos, pos - startPos);
            auto [tokenLine, tokenColumn] = getLineColumn(sourceCode, startPos);
            
            // 检查是否为关键字
            TokenType type = identifyKeyword(word);
            if (type == TokenType::UNKNOWN) {
                type = TokenType::IDENTIFIER;
            }
            
            tokens.addToken(type, word, tokenLine, tokenColumn);
            continue;
        }
        
        // 检查无修饰字面量
        if (isalnum(sourceCode[pos]) || sourceCode[pos] == '_' || 
            sourceCode[pos] == '-' || sourceCode[pos] == '.') {
            size_t startPos = pos;
            pos = readLiteral(sourceCode, pos);
            std::string value = sourceCode.substr(startPos, pos - startPos);
            auto [tokenLine, tokenColumn] = getLineColumn(sourceCode, startPos);
            tokens.addToken(TokenType::IDENTIFIER, value, tokenLine, tokenColumn);
            continue;
        }
        
        // 检查符号
        TokenType symbolType = identifySymbol(sourceCode, pos);
        if (symbolType != TokenType::UNKNOWN) {
            std::string symbol(1, sourceCode[pos]);
            
            // 检查多字符符号
            if (pos + 1 < sourceCode.length()) {
                std::string twoChar = sourceCode.substr(pos, 2);
                if (m_symbols.find(twoChar) != m_symbols.end()) {
                    symbol = twoChar;
                    pos++;
                }
            }
            
            auto [tokenLine, tokenColumn] = getLineColumn(sourceCode, pos);
            tokens.addToken(symbolType, symbol, tokenLine, tokenColumn);
            pos++;
            continue;
        }
        
        // 未知字符
        addError("Unknown character: " + std::string(1, sourceCode[pos]), line, column);
        pos++;
    }
    
    // 添加文件结束标记
    auto [endLine, endColumn] = getLineColumn(sourceCode, sourceCode.length());
    tokens.addToken(TokenType::END_OF_FILE, "", endLine, endColumn);
    
    if (m_debugMode) {
        std::cout << "[CHTLLexer] Generated " << tokens.size() << " tokens" << std::endl;
        for (size_t i = 0; i < tokens.size(); ++i) {
            std::cout << "  " << i << ": " << tokens[i].toString() << std::endl;
        }
    }
    
    return tokens;
}

void CHTLLexer::reset() {
    // 重置内部状态
}

void CHTLLexer::setDebugMode(bool debug) {
    m_debugMode = debug;
}

bool CHTLLexer::isDebugMode() const {
    return m_debugMode;
}

size_t CHTLLexer::skipWhitespace(const std::string& sourceCode, size_t pos) const {
    while (pos < sourceCode.length() && std::isspace(sourceCode[pos])) {
        pos++;
    }
    return pos;
}

size_t CHTLLexer::skipComment(const std::string& sourceCode, size_t pos) const {
    if (pos + 1 < sourceCode.length() && 
        sourceCode[pos] == '/' && sourceCode[pos + 1] == '/') {
        return skipSingleLineComment(sourceCode, pos);
    } else if (pos + 1 < sourceCode.length() && 
               sourceCode[pos] == '/' && sourceCode[pos + 1] == '*') {
        return skipMultiLineComment(sourceCode, pos);
    }
    return pos;
}

size_t CHTLLexer::skipSingleLineComment(const std::string& sourceCode, size_t pos) const {
    if (pos + 1 < sourceCode.length() && 
        sourceCode[pos] == '/' && sourceCode[pos + 1] == '/') {
        pos += 2;
        while (pos < sourceCode.length() && sourceCode[pos] != '\n') {
            pos++;
        }
    }
    return pos;
}

size_t CHTLLexer::skipMultiLineComment(const std::string& sourceCode, size_t pos) const {
    if (pos + 1 < sourceCode.length() && 
        sourceCode[pos] == '/' && sourceCode[pos + 1] == '*') {
        pos += 2;
        while (pos + 1 < sourceCode.length()) {
            if (sourceCode[pos] == '*' && sourceCode[pos + 1] == '/') {
                pos += 2;
                break;
            }
            pos++;
        }
    }
    return pos;
}

size_t CHTLLexer::skipGeneratorComment(const std::string& sourceCode, size_t pos) const {
    if (pos + 1 < sourceCode.length() && 
        sourceCode[pos] == '-' && sourceCode[pos + 1] == '-') {
        pos += 2;
        while (pos < sourceCode.length() && sourceCode[pos] != '\n') {
            pos++;
        }
    }
    return pos;
}

size_t CHTLLexer::skipString(const std::string& sourceCode, size_t pos, char quote) const {
    if (pos >= sourceCode.length() || sourceCode[pos] != quote) {
        return pos;
    }
    
    pos++; // 跳过开始引号
    while (pos < sourceCode.length()) {
        if (sourceCode[pos] == quote && (pos == 0 || sourceCode[pos - 1] != '\\')) {
            pos++; // 跳过结束引号
            break;
        }
        pos++;
    }
    
    return pos;
}

size_t CHTLLexer::readIdentifier(const std::string& sourceCode, size_t pos) const {
    while (pos < sourceCode.length() && 
           (std::isalnum(sourceCode[pos]) || sourceCode[pos] == '_')) {
        pos++;
    }
    return pos;
}

size_t CHTLLexer::readNumber(const std::string& sourceCode, size_t pos) const {
    while (pos < sourceCode.length() && 
           (std::isdigit(sourceCode[pos]) || sourceCode[pos] == '.')) {
        pos++;
    }
    return pos;
}

size_t CHTLLexer::readString(const std::string& sourceCode, size_t pos) const {
    if (pos >= sourceCode.length() || 
        (sourceCode[pos] != '"' && sourceCode[pos] != '\'')) {
        return pos;
    }
    
    return skipString(sourceCode, pos, sourceCode[pos]);
}

size_t CHTLLexer::readLiteral(const std::string& sourceCode, size_t pos) const {
    while (pos < sourceCode.length() && 
           (std::isalnum(sourceCode[pos]) || sourceCode[pos] == '_' || 
            sourceCode[pos] == '-' || sourceCode[pos] == '.')) {
        pos++;
    }
    return pos;
}

TokenType CHTLLexer::identifyKeyword(const std::string& word) const {
    auto it = m_keywords.find(word);
    if (it != m_keywords.end()) {
        return it->second;
    }
    return TokenType::UNKNOWN;
}

TokenType CHTLLexer::identifySymbol(const std::string& sourceCode, size_t pos) const {
    if (pos >= sourceCode.length()) {
        return TokenType::UNKNOWN;
    }
    
    // 检查双字符符号
    if (pos + 1 < sourceCode.length()) {
        std::string twoChar = sourceCode.substr(pos, 2);
        auto it = m_symbols.find(twoChar);
        if (it != m_symbols.end()) {
            return it->second;
        }
    }
    
    // 检查单字符符号
    std::string oneChar(1, sourceCode[pos]);
    auto it = m_symbols.find(oneChar);
    if (it != m_symbols.end()) {
        return it->second;
    }
    
    return TokenType::UNKNOWN;
}

std::pair<size_t, size_t> CHTLLexer::getLineColumn(const std::string& sourceCode, size_t pos) const {
    size_t line = 1;
    size_t column = 1;
    
    for (size_t i = 0; i < pos && i < sourceCode.length(); ++i) {
        if (sourceCode[i] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
    }
    
    return {line, column};
}

void CHTLLexer::addError(const std::string& message, size_t line, size_t column) {
    if (m_context) {
        std::ostringstream oss;
        oss << "Lexer Error at " << line << ":" << column << ": " << message;
        m_context->addError(oss.str());
    }
}

} // namespace CHTL