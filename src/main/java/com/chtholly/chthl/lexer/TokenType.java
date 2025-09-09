package com.chtholly.chthl.lexer;

public enum TokenType {
    // Single-character tokens.
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACKET, RIGHT_BRACKET,
    COLON, SEMICOLON, EQUAL,
    DOT, COMMA, PLUS, MINUS, STAR, SLASH, PERCENT,

    // Two-character tokens
    STAR_STAR, // for power operator
    SLASH_SLASH, // for single line comment
    SLASH_STAR, // for multi-line comment start
    STAR_SLASH, // for multi-line comment end
    MINUS_MINUS, // for generator comment

    // Literals.
    IDENTIFIER, // div, id, class, etc.
    STRING,     // "a string" or 'a string'
    UNQUOTED_LITERAL, // some text

    // Keywords.
    TEXT, STYLE, SCRIPT,
    TEMPLATE, CUSTOM, ORIGIN, IMPORT, NAMESPACE,
    FROM, AS, USE, EXCEPT,
    DELETE, INSERT, AFTER, BEFORE, REPLACE, AT, TOP, BOTTOM,
    INHERIT,

    // End of file.
    EOF
}
