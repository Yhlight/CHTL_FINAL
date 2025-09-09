#ifndef CHTL_JS_LEXER_H
#define CHTL_JS_LEXER_H

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// CHTL JS token types
enum class CHTLJSTokenType {
    // Basic tokens
    Identifier,
    String,
    Number,
    Boolean,
    Null,
    Undefined,
    
    // Operators
    Plus,
    Minus,
    Multiply,
    Divide,
    Modulo,
    Equal,
    NotEqual,
    LessThan,
    GreaterThan,
    LessEqual,
    GreaterEqual,
    And,
    Or,
    Not,
    Assign,
    PlusAssign,
    MinusAssign,
    MultiplyAssign,
    DivideAssign,
    ModuloAssign,
    
    // Punctuation
    LeftParen,
    RightParen,
    LeftBracket,
    RightBracket,
    LeftBrace,
    RightBrace,
    Comma,
    Semicolon,
    Colon,
    Dot,
    QuestionMark,
    Arrow,
    
    // CHTL JS specific
    CHTLJSFunction,
    VirtualObject,
    Animation,
    EventDelegation,
    EnhancedSelector,
    EnhancedListener,
    LocalScript,
    
    // Keywords
    If,
    Else,
    For,
    While,
    Do,
    Switch,
    Case,
    Default,
    Break,
    Continue,
    Return,
    Function,
    Var,
    Let,
    Const,
    Class,
    Extends,
    Import,
    Export,
    From,
    As,
    New,
    This,
    Super,
    Static,
    Async,
    Await,
    Try,
    Catch,
    Finally,
    Throw,
    Typeof,
    Instanceof,
    In,
    Of,
    With,
    Delete,
    Void,
    
    // CHTL JS keywords
    Listen,
    Delegate,
    Animate,
    Vir,
    Router,
    FileLoader,
    Script,
    Selector,
    Listener,
    
    // Special
    EndOfFile,
    Error
};

// CHTL JS token
struct CHTLJSToken {
    CHTLJSTokenType type;
    std::string value;
    size_t line;
    size_t column;
    
    CHTLJSToken(CHTLJSTokenType t, const std::string& v, size_t l, size_t c)
        : type(t), value(v), line(l), column(c) {}
};

// CHTL JS lexer
class CHTLJSLexer {
public:
    CHTLJSLexer(const std::string& source);
    ~CHTLJSLexer();
    
    // Main lexing function
    std::vector<CHTLJSToken> tokenize();
    
    // Individual token parsing
    CHTLJSToken nextToken();
    CHTLJSToken peekToken();
    
    // Utility functions
    bool isAtEnd() const;
    void reset();
    
private:
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    
    // Character utilities
    char currentChar() const;
    char peekChar() const;
    char peekChar(size_t offset) const;
    void advance();
    void advance(size_t count);
    bool match(char expected);
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    
    // Token parsing methods
    CHTLJSToken parseIdentifier();
    CHTLJSToken parseString();
    CHTLJSToken parseNumber();
    CHTLJSToken parseComment();
    CHTLJSToken parseOperator();
    CHTLJSToken parsePunctuation();
    CHTLJSToken parseKeyword();
    CHTLJSToken parseCHTLJSKeyword();
    
    // String parsing
    std::string parseStringLiteral();
    std::string parseTemplateLiteral();
    
    // Number parsing
    std::string parseInteger();
    std::string parseFloat();
    std::string parseHex();
    std::string parseBinary();
    std::string parseOctal();
    
    // Identifier parsing
    std::string parseIdentifierName();
    bool isKeyword(const std::string& word) const;
    bool isCHTLJSKeyword(const std::string& word) const;
    
    // Error handling
    CHTLJSToken createErrorToken(const std::string& message);
    void skipWhitespace();
    void skipComment();
    void skipSingleLineComment();
    void skipMultiLineComment();
    
    // CHTL JS specific parsing
    CHTLJSToken parseCHTLJSFunction();
    CHTLJSToken parseVirtualObject();
    CHTLJSToken parseRouter();
    CHTLJSToken parseAnimation();
    CHTLJSToken parseEventDelegation();
    CHTLJSToken parseEnhancedSelector();
    CHTLJSToken parseEnhancedListener();
    CHTLJSToken parseFileLoader();
    CHTLJSToken parseLocalScript();
    
    // Helper methods
    std::string getTokenValue(CHTLJSTokenType type) const;
    CHTLJSTokenType getTokenType(const std::string& value) const;
    bool isOperatorStart(char c) const;
    bool isPunctuationStart(char c) const;
    bool isStringStart(char c) const;
    bool isNumberStart(char c) const;
    bool isIdentifierStart(char c) const;
    bool isCommentStart(char c) const;
    bool isCHTLJSStart(char c) const;
};

} // namespace CHTL

#endif // CHTL_JS_LEXER_H