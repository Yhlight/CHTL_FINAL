#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    // Delimiters
    OpenBrace,      // {
    CloseBrace,     // }
    OpenParen,      // (
    CloseParen,     // )
    Colon,          // :
    Semicolon,      // ;
    Comma,          // ,
    Dot,            // .

    // CHTL JS specific
    Arrow,              // ->
    FatArrow,           // =>

    // General
    Identifier(String),
    StringLiteral(String),

    // For now, we'll treat the content inside {{...}} as a special kind of literal.
    SelectorContent(String),

    // Keywords (to be added later)
    // Listen, Animate, Vir, Router, etc.

    // Special
    EndOfFile,
    Illegal(String),
}
