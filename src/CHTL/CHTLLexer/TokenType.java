package CHTL.CHTLLexer;

public enum TokenType {
    // Literals
    IDENTIFIER,         // e.g., div, my-class
    STRING,             // e.g., "hello world", 'hello world', helloworld
    NUMBER,             // e.g., 100, 1.6

    // Symbols
    LBRACE, RBRACE,     // { }
    LBRACKET, RBRACKET, // [ ]
    COLON,              // :
    EQUALS,             // =
    SEMICOLON,          // ;
    COMMA,              // ,
    DOT,                // .
    AT,                 // @
    HASH,               // #
    SLASH,              // /
    AMPERSAND,          // &

    // Comments
    SINGLE_LINE_COMMENT,    // //
    MULTI_LINE_COMMENT_START, // /*
    MULTI_LINE_COMMENT_END,   // */
    GENERATOR_COMMENT,      // --

    // Keywords (will be expanded)
    TEXT,               // text
    STYLE,              // style
    TEMPLATE,           // Template
    CUSTOM,             // Custom
    IMPORT,             // Import
    ORIGIN,             // Origin
    CONFIGURATION,      // Configuration
    NAME,               // Name
    USE,                // use
    INHERIT,            // inherit
    DELETE,             // delete
    INSERT,             // insert
    AFTER,              // after
    BEFORE,             // before
    REPLACE,            // replace
    ATTOP,              // at top
    ATBOTTOM,           // at bottom
    FROM,               // from
    AS,                 // as

    // End of File
    EOF
}
