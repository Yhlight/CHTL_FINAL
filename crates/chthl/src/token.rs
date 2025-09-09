//! This module defines the tokens that make up the CHTL language.

#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    // Special tokens
    Illegal(String), // Represents a token that is not valid
    Eof,             // End of file

    // Identifiers and literals
    Ident(String),  // e.g., div, color, my_variable. Also used for unquoted literals.
    String(String), // "a string" or 'a string'

    // Delimiters
    LBrace,     // {
    RBrace,     // }
    LParen,     // (
    RParen,     // )
    LBracket,   // [
    RBracket,   // ]
    Semicolon,  // ;
    Comma,      // ,
    Colon,      // :
    Assign,     // =

    // Special symbols
    At,         // @
    Hash,       // #
    Dot,        // .
    Ampersand,  // &
    Question,   // ?
    Arrow,      // ->

    // Operators for conditional expressions
    Gt,             // >
    Lt,             // <
    LogicalAnd,     // &&
    LogicalOr,      // ||

    // Comments
    GeneratorComment(String), // -- a comment
}
