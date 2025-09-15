#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

/**
 * CHTL词法分析器
 * 负责将CHTL代码分解为令牌序列
 */
class CHTLLexer {
public:
    CHTLLexer(const std::string& source);
    ~CHTLLexer();
    
    // 主要方法
    std::vector<Token> tokenize();
    Token nextToken();
    bool hasMoreTokens() const;
    
    // 状态查询
    int getCurrentLine() const;
    int getCurrentColumn() const;
    const std::string& getSource() const;

private:
    std::string source_;
    size_t position_;
    int line_;
    int column_;
    
    // 辅助方法
    char currentChar() const;
    char peekChar() const;
    void advance();
    void skipWhitespace();
    void skipComment();
    
    Token parseString();
    Token parseNumber();
    Token parseIdentifier();
    Token parseHTML();
    Token parseTemplate();
    Token parseCustom();
    Token parseOrigin();
    Token parseImport();
    Token parseNamespace();
    Token parseConfiguration();
    Token parseSelector();
    Token parseResponsiveValue();
    Token parseGeneratorComment();
    
    bool isLetter(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    
    TokenType getKeywordType(const std::string& keyword) const;
    TokenType getHTMLType(const std::string& element) const;
    TokenType getTemplateType(const std::string& type) const;
    TokenType getCustomType(const std::string& type) const;
    TokenType getOriginType(const std::string& type) const;
    TokenType getImportType(const std::string& type) const;
    
    // 关键字映射
    static const std::map<std::string, TokenType> keywords_;
    static const std::map<std::string, TokenType> html_elements_;
    static const std::map<std::string, TokenType> template_types_;
    static const std::map<std::string, TokenType> custom_types_;
    static const std::map<std::string, TokenType> origin_types_;
    static const std::map<std::string, TokenType> import_types_;
};

} // namespace CHTL

#endif // CHTL_LEXER_H