//! This module defines the tokens that make up the CHTL language.

#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    // Special tokens
    Illegal(String), // Represents a token that is not valid
    Eof,             // End of file

    // Identifiers and literals
    Ident(String),  // e.g., div, color, 100px, 50. Also used for unquoted literals.
    String(String), // "a string" or 'a string'
    Number(String), // e.g., 50, 1.5

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

    // Operators for conditional expressions (to be expanded)
    Gt,             // >
    Lt,             // <
    LogicalAnd,     // &&
    LogicalOr,      // ||
    Bang,           // !
    Plus,           // +
    Minus,          // -
    Asterisk,       // *
    Slash,          // /

    // Comments
    GeneratorComment(String), // -- a comment
}
