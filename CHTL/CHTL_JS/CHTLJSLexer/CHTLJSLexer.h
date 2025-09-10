#ifndef CHTL_JS_LEXER_H
#define CHTL_JS_LEXER_H

#include "CHTLJSToken.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace CHTL_JS {

class CHTLJSLexer {
private:
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    bool debugMode;
    bool strictMode;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, CHTLJSTokenType> keywords;
    std::map<std::string, CHTLJSTokenType> operators;
    std::map<char, CHTLJSTokenType> singleCharTokens;
    std::map<std::string, CHTLJSTokenType> multiCharTokens;
    
    // 统一扫描器相关
    std::vector<std::string> placeholders;
    std::map<std::string, std::string> placeholderMap;
    bool isUnifiedMode;
    std::string currentFragment;
    std::string fragmentType; // "chtl", "chtljs", "js", "css"
    
    // 语法边界识别
    struct SyntaxBoundary {
        std::string startPattern;
        std::string endPattern;
        std::string placeholder;
        std::string type;
        bool isNested;
        int priority;
    };
    
    std::vector<SyntaxBoundary> boundaries;
    std::map<std::string, std::string> boundaryMap;

public:
    CHTLJSLexer();
    virtual ~CHTLJSLexer() = default;
    
    // 基本配置
    void setSource(const std::string& source);
    void setDebugMode(bool debug) { debugMode = debug; }
    void setStrictMode(bool strict) { strictMode = strict; }
    void setUnifiedMode(bool unified) { isUnifiedMode = unified; }
    
    // 词法分析
    std::vector<CHTLJSToken> tokenize();
    std::vector<CHTLJSToken> tokenizeFragment(const std::string& fragment, const std::string& type);
    CHTLJSToken getNextToken();
    CHTLJSToken peekToken();
    CHTLJSToken peekToken(int offset);
    
    // 统一扫描器功能
    std::vector<std::string> separateCodeFragments(const std::string& source);
    std::string createPlaceholder(const std::string& content, const std::string& type);
    std::string restoreFromPlaceholder(const std::string& placeholder);
    std::string processUnifiedCode(const std::string& source);
    
    // 语法边界识别
    void addSyntaxBoundary(const std::string& start, const std::string& end, 
                          const std::string& type, bool nested = false, int priority = 0);
    std::vector<SyntaxBoundary> detectBoundaries(const std::string& code);
    std::string replaceBoundariesWithPlaceholders(const std::string& code);
    
    // 错误处理
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    void clearErrors();
    void clearWarnings();
    const std::vector<std::string>& getErrors() const { return errors; }
    const std::vector<std::string>& getWarnings() const { return warnings; }
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    
    // 状态管理
    void reset();
    void resetPosition();
    size_t getPosition() const { return position; }
    size_t getLine() const { return line; }
    size_t getColumn() const { return column; }
    bool isAtEnd() const;
    
    // 调试信息
    std::string getDebugInfo() const;
    std::string getTokenDebugInfo(const std::vector<CHTLJSToken>& tokens) const;
    void printTokens(const std::vector<CHTLJSToken>& tokens) const;
    
    // 验证
    bool validateTokens(const std::vector<CHTLJSToken>& tokens) const;
    bool validateSyntax(const std::vector<CHTLJSToken>& tokens) const;
    
    // 优化
    std::vector<CHTLJSToken> optimizeTokens(const std::vector<CHTLJSToken>& tokens);
    std::vector<CHTLJSToken> compressTokens(const std::vector<CHTLJSToken>& tokens);
    std::vector<CHTLJSToken> decompressTokens(const std::vector<CHTLJSToken>& tokens);

private:
    // 初始化
    void initializeKeywords();
    void initializeOperators();
    void initializeSingleCharTokens();
    void initializeMultiCharTokens();
    void initializeSyntaxBoundaries();
    
    // 字符处理
    char getCurrentChar() const;
    char getNextChar();
    char peekChar() const;
    char peekChar(int offset) const;
    void advance();
    void advance(int count);
    void skipWhitespace();
    void skipLine();
    void skipBlockComment();
    
    // 词法分析辅助
    CHTLJSToken readIdentifier();
    CHTLJSToken readNumber();
    CHTLJSToken readString();
    CHTLJSToken readString(char delimiter);
    CHTLJSToken readTemplateLiteral();
    CHTLJSToken readComment();
    CHTLJSToken readSingleLineComment();
    CHTLJSToken readMultiLineComment();
    CHTLJSToken readGeneratorComment();
    CHTLJSToken readOperator();
    CHTLJSToken readPunctuation();
    CHTLJSToken readEnhancedSelector();
    CHTLJSToken readVirtualObject();
    CHTLJSToken readCHTLJSKeyword();
    CHTLJSToken readUnquotedLiteral();
    
    // 类型判断
    bool isLetter(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    bool isQuote(char c) const;
    bool isOperatorChar(char c) const;
    bool isPunctuationChar(char c) const;
    bool isKeyword(const std::string& word) const;
    bool isOperator(const std::string& op) const;
    bool isSingleCharToken(char c) const;
    bool isMultiCharToken(const std::string& str) const;
    
    // 统一扫描器辅助
    std::string extractCHTLFragment(const std::string& source, size_t& pos);
    std::string extractCHTLJSFragment(const std::string& source, size_t& pos);
    std::string extractJSFragment(const std::string& source, size_t& pos);
    std::string extractCSSFragment(const std::string& source, size_t& pos);
    std::string extractMixedFragment(const std::string& source, size_t& pos);
    
    // 占位符管理
    std::string generatePlaceholder(const std::string& type);
    void registerPlaceholder(const std::string& placeholder, const std::string& content);
    std::string getPlaceholderContent(const std::string& placeholder);
    void clearPlaceholders();
    
    // 语法边界处理
    bool isBoundaryStart(const std::string& code, size_t pos) const;
    bool isBoundaryEnd(const std::string& code, size_t pos, const std::string& startPattern) const;
    std::string findMatchingBoundary(const std::string& code, size_t startPos, const std::string& startPattern) const;
    int calculateBoundaryPriority(const std::string& startPattern) const;
    
    // 错误处理辅助
    void reportError(const std::string& message, size_t line, size_t column);
    void reportWarning(const std::string& message, size_t line, size_t column);
    std::string formatError(const std::string& message, size_t line, size_t column) const;
    
    // 调试辅助
    std::string getContext(size_t pos, int contextSize = 20) const;
    std::string getLineContext(size_t line) const;
    void printContext(size_t pos) const;
    
    // 验证辅助
    bool isValidIdentifier(const std::string& identifier) const;
    bool isValidNumber(const std::string& number) const;
    bool isValidString(const std::string& str) const;
    bool isValidOperator(const std::string& op) const;
    bool isValidPunctuation(const std::string& punct) const;
    
    // 优化辅助
    bool canMergeTokens(const CHTLJSToken& token1, const CHTLJSToken& token2) const;
    CHTLJSToken mergeTokens(const CHTLJSToken& token1, const CHTLJSToken& token2) const;
    bool canCompressToken(const CHTLJSToken& token) const;
    CHTLJSToken compressToken(const CHTLJSToken& token) const;
    CHTLJSToken decompressToken(const CHTLJSToken& token) const;
    
    // 特殊语法处理
    bool isCHTLJSSyntax(const std::string& code, size_t pos) const;
    bool isEnhancedSelectorStart(const std::string& code, size_t pos) const;
    bool isVirtualObjectStart(const std::string& code, size_t pos) const;
    bool isListenStart(const std::string& code, size_t pos) const;
    bool isDelegateStart(const std::string& code, size_t pos) const;
    bool isAnimateStart(const std::string& code, size_t pos) const;
    bool isRouterStart(const std::string& code, size_t pos) const;
    bool isFileloaderStart(const std::string& code, size_t pos) const;
    bool isUnquotedLiteralStart(char c) const;
    bool isUnquotedLiteralChar(char c) const;
    bool isUtilStart(const std::string& code, size_t pos) const;
    
    // 声明式语法处理
    bool isDeclarationSyntax(const std::string& code, size_t pos) const;
    CHTLJSToken readDeclaration(const std::string& keyword);
    std::vector<CHTLJSToken> parseKeyValuePairs(const std::string& content);
    std::vector<CHTLJSToken> parseUnorderedPairs(const std::string& content);
    std::vector<CHTLJSToken> parseOptionalPairs(const std::string& content);
    
    // 无修饰字面量处理
    bool isUnquotedLiteral(const std::string& code, size_t pos) const;
    CHTLJSToken readUnquotedLiteral();
    bool isValidUnquotedLiteral(const std::string& literal) const;
    
    // 链式语法处理
    bool isChainSyntax(const std::string& code, size_t pos) const;
    CHTLJSToken readChainExpression();
    std::vector<CHTLJSToken> parseChainElements(const std::string& chain);
};

} // namespace CHTL_JS

#endif // CHTL_JS_LEXER_H