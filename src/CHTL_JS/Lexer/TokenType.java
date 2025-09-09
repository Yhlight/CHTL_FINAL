package CHTL_JS.Lexer;

public enum TokenType {
    // Keywords
    LISTEN,
    ANIMATE,
    DELEGATE,
    TARGET,
    ROUTER,

    // Symbols
    LBRACE, RBRACE,         // { }
    COLON,                  // :
    COMMA,                  // ,
    SEMICOLON,              // ;
    ARROW,                  // ->

    // Selector Syntax
    L_CURLY_BRACE,          // {{
    R_CURLY_BRACE,          // }}
    SELECTOR,               // e.g., #box, .class, div[0]

    // Literals
    IDENTIFIER,
    STRING,
    NUMBER,

    EOF
}
