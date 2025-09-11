#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // Special Tokens
    EndOfFile,
    Unexpected,

    // Literals
    Identifier,
    StringLiteral,
    UnquotedLiteral,

    // Symbols
    OpenBrace, CloseBrace, OpenParen, CloseParen, OpenBracket, CloseBracket,
    Colon, Semicolon, Equals, At, Hash, Dot, Ampersand, Comma,

    // Operators
    Plus, Minus, Asterisk, Slash, Percent, DoubleAsterisk,
    LogicalAnd, LogicalOr, QuestionMark, GreaterThan, LessThan,

    // Keywords
    TemplateKeyword, CustomKeyword, ImportKeyword, DeleteKeyword, InsertKeyword,
    FromKeyword, UseKeyword, ExceptKeyword, ConfigurationKeyword, NamespaceKeyword,
    ExportKeyword,      // [Export]

    // Comments
    SingleLineComment, MultiLineComment, GeneratorComment,
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

// Helper for debugging
std::string tokenTypeToString(TokenType type);
