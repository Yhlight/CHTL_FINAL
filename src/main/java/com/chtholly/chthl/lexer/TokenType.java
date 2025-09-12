package com.chtholly.chthl.lexer;

public enum TokenType {
    // For unrecognized characters
    ILLEGAL,

    // End of file
    EOF,

    // --- Identifiers & Literals ---
    IDENTIFIER, // div, body, my_var
    STRING,     // "Hello World" or 'Hello World'

    // --- Operators ---
    EQUAL,      // =
    COLON,      // :

    // --- Delimiters ---
    SEMICOLON,  // ;
    LEFT_BRACE, // {
    RIGHT_BRACE,// }

    // --- Keywords ---
    // At this stage, we will treat keywords like 'text' as identifiers.
    // The parser will be responsible for distinguishing them.

    // --- Comments ---
    COMMENT     // // or /**/
}
