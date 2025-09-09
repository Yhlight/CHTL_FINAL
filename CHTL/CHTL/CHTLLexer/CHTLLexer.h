#pragma once

#include "Token.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class CHTLLexer {
private:
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    
    // 状态管理
    bool inString;
    bool inComment;
    bool inMultiLineComment;
    char stringDelimiter;
    
    // 辅助方法
    char currentChar() const;
    char peekChar(size_t offset = 1) const;
    void advance();
    void skipWhitespace();
    void skipNewline();
    
    // 词法分析方法
    Token scanIdentifier();
    Token scanString();
    Token scanNumber();
    Token scanUnquotedLiteral();
    Token scanComment();
    Token scanOperator();
    Token scanDelimiter();
    Token scanSpecial();
    
    // 工具方法
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    bool isStringDelimiter(char c) const;
    bool isCommentStart() const;
    bool isMultiLineCommentStart() const;
    bool isMultiLineCommentEnd() const;
    bool isInUnquotedLiteralContext() const;
    
    // 关键字和运算符识别
    TokenType identifyKeyword(const std::string& value) const;
    TokenType identifyOperator(const std::string& value) const;
    TokenType identifyDelimiter(const std::string& value) const;
    
public:
    CHTLLexer(const std::string& source);
    ~CHTLLexer() = default;
    
    // 主要接口
    TokenStream tokenize();
    std::vector<Token> getAllTokens();
    
    // 状态查询
    bool isAtEnd() const;
    size_t getCurrentLine() const;
    size_t getCurrentColumn() const;
    size_t getCurrentPosition() const;
    
    // 错误处理
    void throwError(const std::string& message) const;
};

} // namespace CHTL