#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace CHTL {

class CHTLLexer {
private:
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    
    // 关键字映射
    std::unordered_map<std::string, TokenType> keywords;
    
    // 初始化关键字映射
    void initializeKeywords();
    
    // 辅助方法
    char currentChar() const;
    char peekChar(size_t offset = 1) const;
    void advance();
    void skipWhitespace();
    void skipLine();
    
    // 词法分析辅助方法
    Token scanIdentifier();
    Token scanString();
    Token scanNumber();
    Token scanLiteral();
    Token scanComment();
    Token scanOperator();
    Token scanSymbol();
    
    // CHTL特有词法分析方法
    Token scanElementName();
    Token scanAttributeName();
    Token scanAttributeValue();
    Token scanTextContent();
    Token scanUnquotedLiteral();
    Token scanQuotedLiteral();
    Token scanColonEqual();
    
    // 局部样式块词法分析方法
    Token scanClassSelector();
    Token scanIdSelector();
    Token scanContextReference();
    Token scanConditionalExpression();
    Token scanPropertyReference();
    
    // 模板系统词法分析方法
    Token scanTemplateSyntax();
    Token scanTemplateType();
    Token scanResponsiveValue();
    
    // 检查是否为字母
    bool isAlpha(char c) const;
    
    // 检查是否为数字
    bool isDigit(char c) const;
    
    // 检查是否为字母或数字
    bool isAlphaNumeric(char c) const;
    
    // 检查是否为空白字符
    bool isWhitespace(char c) const;
    
    // 检查是否为换行符
    bool isNewline(char c) const;
    
    // 检查是否为引号
    bool isQuote(char c) const;
    
    // 检查是否为运算符字符
    bool isOperatorChar(char c) const;
    
    // 检查是否为符号字符
    bool isSymbolChar(char c) const;
    
    // 检查是否为模板关键字
    bool isTemplateKeyword(const std::string& str) const;
    
    // 检查是否为自定义关键字
    bool isCustomKeyword(const std::string& str) const;
    
    // 检查是否为原始嵌入关键字
    bool isOriginKeyword(const std::string& str) const;
    
    // 检查是否为导入关键字
    bool isImportKeyword(const std::string& str) const;
    
    // 检查是否为命名空间关键字
    bool isNamespaceKeyword(const std::string& str) const;
    
    // 检查是否为配置关键字
    bool isConfigurationKeyword(const std::string& str) const;
    
    // 检查是否为use关键字
    bool isUseKeyword(const std::string& str) const;
    
    // 检查是否为HTML5关键字
    bool isHTML5Keyword(const std::string& str) const;
    
    // CHTL特有检查方法
    bool isElementNameChar(char c) const;
    bool isAttributeNameChar(char c) const;
    bool isTextContentChar(char c) const;
    bool isLiteralChar(char c) const;
    bool isColonEqual() const;
    bool isElementStart() const;
    bool isAttributeStart() const;
    bool isTextStart() const;

public:
    CHTLLexer();
    explicit CHTLLexer(const std::string& source);
    
    // 设置源代码
    void setSource(const std::string& source);
    
    // 词法分析
    TokenStream tokenize();
    
    // 重置词法分析器状态
    void reset();
    
    // 获取当前位置信息
    size_t getPosition() const { return position; }
    size_t getLine() const { return line; }
    size_t getColumn() const { return column; }
};

} // namespace CHTL

#endif // CHTL_LEXER_H