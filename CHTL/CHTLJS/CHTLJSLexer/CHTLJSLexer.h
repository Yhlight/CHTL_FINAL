#ifndef CHTLJS_LEXER_H
#define CHTLJS_LEXER_H

#include "Token.h"
#include <string>
#include <vector>

namespace CHTLJS {

class CHTLJSLexer {
private:
    std::string source;
    size_t currentPos;
    size_t currentLine;
    size_t currentColumn;
    bool debugMode;
    bool minifyOutput;
    std::vector<std::string> errors;
    
    // 内部方法
    char getCurrentChar() const;
    char peekChar(size_t offset = 1) const;
    void advance();
    void skipWhitespace();
    void skipComment();
    
    // Token识别方法
    Token scanIdentifier();
    Token scanString();
    Token scanNumber();
    Token scanOperator();
    Token scanPunctuation();
    Token scanEnhancedSelector();
    Token scanResponsiveValue();
    Token scanEventBinding();
    Token scanArrowAccess();
    
    // CHTL JS特有语法识别
    bool isVirKeyword(const std::string& text) const;
    bool isListenKeyword(const std::string& text) const;
    bool isAnimateKeyword(const std::string& text) const;
    bool isDelegateKeyword(const std::string& text) const;
    bool isScriptLoaderKeyword(const std::string& text) const;
    bool isEnhancedSelectorStart() const;
    bool isResponsiveValueStart() const;
    bool isEventBindingStart() const;
    bool isArrowAccessStart() const;
    
    // 工具方法
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    bool isQuote(char c) const;
    bool isOperatorChar(char c) const;
    bool isPunctuationChar(char c) const;
    
    void addError(const std::string& message);
    void logDebug(const std::string& message) const;
    
public:
    CHTLJSLexer();
    CHTLJSLexer(const std::string& source);
    ~CHTLJSLexer() = default;
    
    // 主要方法
    TokenList tokenize();
    TokenList tokenize(const std::string& source);
    
    // 配置方法
    void setDebugMode(bool debug);
    bool isDebugMode() const;
    void setMinifyOutput(bool minify);
    bool isMinifyOutput() const;
    void setSource(const std::string& source);
    
    // 状态查询
    bool hasError() const;
    std::string getLastError() const;
    std::vector<std::string> getErrors() const;
    
    // 工具方法
    void reset();
    void clear();
    
    // 调试方法
    void printTokens() const;
    void printStatistics() const;
};

} // namespace CHTLJS

#endif // CHTLJS_LEXER_H