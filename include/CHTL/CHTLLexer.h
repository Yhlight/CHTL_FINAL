#pragma once

#include "Token.h"
#include "Common.h"
#include <string_view>
#include <memory>

namespace CHTL {

// 词法分析器类
class CHTLLexer {
private:
    StringView source_;
    size_t position_;
    size_t line_;
    size_t column_;
    TokenList tokens_;
    std::vector<Error> errors_;
    
    // 关键字映射
    static const std::unordered_map<String, TokenType> keywords_;
    static const std::unordered_map<String, TokenType> templateKeywords_;
    static const std::unordered_map<String, TokenType> customKeywords_;
    static const std::unordered_map<String, TokenType> importKeywords_;
    static const std::unordered_map<String, TokenType> originKeywords_;
    static const std::unordered_map<String, TokenType> configurationKeywords_;
    static const std::unordered_map<String, TokenType> operatorKeywords_;
    
    // 字符检查
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    bool isQuote(char c) const;
    bool isDelimiter(char c) const;
    bool isOperator(char c) const;
    bool isSpecial(char c) const;
    
    // 位置管理
    Position getCurrentPosition() const;
    void advance();
    void advance(int count);
    char peek(int offset = 0) const;
    char current() const;
    bool isAtEnd() const;
    
    // Token识别
    Token scanToken();
    Token scanIdentifier();
    Token scanString();
    Token scanNumber();
    Token scanComment();
    Token scanOperator();
    Token scanSpecial();
    Token scanDelimiter();
    Token scanKeyword();
    Token scanTemplateKeyword();
    Token scanCustomKeyword();
    Token scanImportKeyword();
    Token scanOriginKeyword();
    Token scanConfigurationKeyword();
    Token scanOperatorKeyword();
    
    // 字符串处理
    String scanStringLiteral(char quote);
    String scanRawString();
    String scanTemplateString();
    String scanInterpolatedString();
    
    // 数字处理
    String scanInteger();
    String scanFloat();
    String scanHex();
    String scanOctal();
    String scanBinary();
    
    // 注释处理
    Token scanLineComment();
    Token scanBlockComment();
    Token scanGeneratorComment();
    
    // 操作符处理
    Token scanSingleOperator();
    Token scanDoubleOperator();
    Token scanTripleOperator();
    
    // 特殊字符处理
    Token scanAtSymbol();
    Token scanDollarSymbol();
    Token scanHashSymbol();
    Token scanAmpersand();
    Token scanQuestionMark();
    Token scanExclamation();
    Token scanUnderscore();
    Token scanPipe();
    Token scanTilde();
    Token scanCaret();
    
    // 选择器处理
    Token scanClassSelector();
    Token scanIdSelector();
    Token scanAttributeSelector();
    Token scanPseudoClass();
    Token scanPseudoElement();
    Token scanIndexSelector();
    
    // 单位处理
    Token scanUnit();
    
    // 错误处理
    void addError(const String& message, const SourceLocation& location);
    void addError(const String& message, const Position& position);
    
    // 跳过空白字符
    void skipWhitespace();
    void skipNewlines();
    
    // 跳过注释
    void skipComment();
    
    // 查找匹配的字符
    size_t findMatching(char open, char close) const;
    size_t findStringEnd(char quote) const;
    size_t findNewline() const;
    
    // 验证Token
    bool validateToken(const Token& token) const;
    
public:
    CHTLLexer(const StringView& source);
    ~CHTLLexer() = default;
    
    // 主要接口
    TokenList tokenize();
    const TokenList& getTokens() const { return tokens_; }
    const std::vector<Error>& getErrors() const { return errors_; }
    bool hasErrors() const { return !errors_.empty(); }
    
    // 重置
    void reset();
    void reset(const StringView& source);
    
    // 状态查询
    size_t getPosition() const { return position_; }
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    bool isComplete() const { return position_ >= source_.length(); }
    
    // 静态方法
    static TokenType getKeywordType(const String& keyword);
    static bool isKeyword(const String& word);
    static bool isTemplateKeyword(const String& word);
    static bool isCustomKeyword(const String& word);
    static bool isImportKeyword(const String& word);
    static bool isOriginKeyword(const String& word);
    static bool isConfigurationKeyword(const String& word);
    static bool isOperatorKeyword(const String& word);
    static bool isUnit(const String& word);
    static bool isHTMLTag(const String& word);
    static bool isVoidElement(const String& word);
    static bool isSelfClosingElement(const String& word);
    static bool isBlockElement(const String& word);
    static bool isInlineElement(const String& word);
    static bool isInlineBlockElement(const String& word);
    
    // 工具方法
    static String escapeString(const String& str);
    static String unescapeString(const String& str);
    static String normalizeWhitespace(const String& str);
    static String removeComments(const String& str);
    static String extractStringContent(const String& str);
    static String extractNumberContent(const String& str);
    static String extractIdentifierContent(const String& str);
    static String extractSelectorContent(const String& str);
    static String extractPropertyContent(const String& str);
    static String extractValueContent(const String& str);
    
    // 调试
    void printTokens() const;
    void printErrors() const;
    String getTokenInfo(const Token& token) const;
    String getSourceSnippet(size_t start, size_t end) const;
};

} // namespace CHTL