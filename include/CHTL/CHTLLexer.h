#pragma once

#include "CHTLCommon.h"
#include <istream>

namespace CHTL {

// 词法单元类型
enum class TokenType {
    // 基础符号
    LeftBrace,      // {
    RightBrace,     // }
    LeftBracket,    // [
    RightBracket,   // ]
    LeftParen,      // (
    RightParen,     // )
    Semicolon,      // ;
    Colon,          // :
    Equal,          // =
    Comma,          // ,
    Dot,            // .
    Hash,           // #
    At,             // @
    Question,       // ?
    Exclamation,    // !
    Ampersand,      // &
    Pipe,           // |
    Tilde,          // ~
    Caret,          // ^
    Dollar,         // $
    Percent,        // %
    Plus,           // +
    Minus,          // -
    Star,           // *
    Slash,          // /
    Backslash,      // \
    Less,           // <
    Greater,        // >
    Underscore,     // _
    
    // 字符串
    String,         // "string" or 'string'
    RawString,      // unquoted string
    
    // 数字
    Number,         // 123, 123.45
    
    // 标识符
    Identifier,     // variable name, function name, etc.
    
    // 关键字
    Keyword,        // reserved words
    
    // 注释
    Comment,        // // or /* */
    GeneratorComment, // --
    
    // 特殊
    Newline,        // \n
    Whitespace,     // spaces, tabs
    EndOfFile,      // EOF
    
    // 错误
    Error           // invalid token
};

// 词法单元
struct Token {
    TokenType type;
    String value;
    Position position;
    
    Token() : type(TokenType::Error) {}
    Token(TokenType t, const String& v, const Position& p) 
        : type(t), value(v), position(p) {}
    
    bool isKeyword() const { return type == TokenType::Keyword; }
    bool isIdentifier() const { return type == TokenType::Identifier; }
    bool isString() const { return type == TokenType::String; }
    bool isNumber() const { return type == TokenType::Number; }
    bool isOperator() const;
    bool isPunctuation() const;
    
    // 检查是否为特定关键字
    bool isKeyword(const String& keyword) const;
    
    // 检查是否为特定操作符
    bool isOperator(const String& op) const;
};

// 词法分析器
class CHTLLexer {
public:
    CHTLLexer(std::istream& input);
    ~CHTLLexer() = default;
    
    // 获取下一个词法单元
    Token nextToken();
    
    // 预览下一个词法单元（不移动位置）
    Token peekToken();
    
    // 跳过当前词法单元
    void skipToken();
    
    // 检查是否到达文件末尾
    bool isEof() const;
    
    // 获取当前位置
    const Position& getPosition() const;
    
    // 获取错误列表
    const ErrorList& getErrors() const;
    
    // 重置词法分析器
    void reset();

private:
    std::istream& input_;
    Position currentPos_;
    Token nextToken_;
    bool hasNextToken_;
    ErrorList errors_;
    
    // 内部方法
    Token scanToken();
    Token scanIdentifier();
    Token scanString();
    Token scanNumber();
    Token scanComment();
    Token scanOperator();
    Token scanPunctuation();
    
    // 字符处理
    char getChar();
    char peekChar();
    void ungetChar();
    void skipWhitespace();
    void skipNewline();
    
    // 位置更新
    void updatePosition(char c);
    
    // 错误处理
    void addError(const String& message, const Position& pos);
    
    // 关键字检查
    bool isKeyword(const String& word) const;
    bool isOperator(const String& op) const;
    
    // 字符分类
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    bool isQuote(char c) const;
    bool isEscape(char c) const;
    
    // 配置选项
    ConfigOptions config_;
};

} // namespace CHTL