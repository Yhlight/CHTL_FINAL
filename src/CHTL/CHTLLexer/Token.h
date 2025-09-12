#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <variant>

namespace CHTL {

enum class TokenType {
    // Special Tokens
    EndOfFile,
    Unexpected,

    // Literals
    Identifier,
    StringLiteral,
    UnquotedLiteral,

    // Punctuation
    OpenBrace,      // {
    CloseBrace,     // }
    OpenParen,      // (
    CloseParen,     // )
    OpenBracket,    // [
    CloseBracket,   // ]
    Colon,          // :
    Semicolon,      // ;
    Equals,         // =
    Comma,          // ,
    Dot,            // .

    // Operators
    Plus,           // +
    Minus,          // -
    Asterisk,       // *
    Slash,          // /
    Percent,        // %
    DoubleAsterisk, // **
    QuestionMark,   // ?
    Ampersand,      // &
    DoubleAmpersand,// &&
    Pipe,           // |
    DoublePipe,     // ||
    GreaterThan,    // >
    LessThan,       // <
    Arrow,          // ->

    // Comments
    SingleLineComment, // // (skipped)
    MultiLineComment,  // /* */ (skipped)
    GeneratorComment,  // --

    // Keywords that are standalone identifiers
    Text,
    Style,
    Script,
    Inherit,
    Delete,
    Insert,
    After,
    Before,
    Replace,
    From,
    As,
    Except,
    Use,
    Html5,

    // Keywords with special characters
    AtStyle,        // @Style
    AtElement,      // @Element
    AtVar,          // @Var
    AtHtml,         // @Html
    AtJavaScript,   // @JavaScript
    AtChtl,         // @Chtl
    AtCJmod,        // @CJmod
    AtConfig,       // @Config
    AtTop,          // at top
    AtBottom,       // at bottom

    // Keywords that are part of a construct
    Template,       // [Template]
    Custom,         // [Custom]
    Origin,         // [Origin]
    Import,         // [Import]
    Namespace,      // [Namespace]
    Configuration,  // [Configuration]
    Info,           // [Info]
    Export,         // [Export]
    Name,           // [Name]
    OriginType,     // [OriginType]
};

struct Token {
    TokenType type;
    std::string lexeme;
    size_t line = 1;
    size_t column = 1;
};

} // namespace CHTL

#endif // CHTL_TOKEN_H
