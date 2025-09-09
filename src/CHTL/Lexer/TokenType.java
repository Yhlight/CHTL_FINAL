package CHTL.Lexer;

public enum TokenType {
    // Literals
    IDENTIFIER,
    STRING,
    NUMBER,

    // Symbols
    LBRACE, RBRACE,
    COLON,
    EQUALS,
    SEMICOLON,

    // Keywords from the spec
    TEXT,
    STYLE,
    SCRIPT,

    EOF
}
