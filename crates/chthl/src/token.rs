//! This module defines the tokens that make up the CHTL language.

#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    // Special tokens
    Illegal(String),
    Eof,

    // Identifiers and literals
    Ident(String),
    String(String),
    Number(String),

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

    // Operators
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
    GeneratorComment(String),
}
