#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    // Single-character tokens
    OpenBrace,      // {
    CloseBrace,     // }
    OpenParen,      // (
    CloseParen,     // )
    OpenBracket,    // [
    CloseBracket,   // ]
    Colon,          // :
    Semicolon,      // ;
    Equal,          // =
    At,             // @
    Hash,           // #
    Dot,            // .
    Comma,          // ,
    Question,       // ?
    GreaterThan,    // >
    LessThan,       // <

    // Two-character tokens (for future expansion)
    // Eq, NotEq, GtEq, LtEq, And, Or

    // Literals
    Identifier(String),
    StringLiteral(String),
    NumberLiteral(f64),

    // Keywords - will be identified from Identifier in the parser
    // For now, we just tokenize identifiers generically.

    // Comments
    LineComment(String),         // // a comment
    BlockComment(String),        // /* a comment */
    GeneratorComment(String),    // -- a comment

    // Keywords
    TemplateKeyword, // [Template]
    ImportKeyword,   // [Import]

    // Special
    EndOfFile,
    Illegal(String),
}
