package CHTL.Expression;

public enum ExpressionTokenType {
    // Literals
    NUMBER,             // e.g., 100, 1.6
    STRING,             // e.g., "hello world"
    IDENTIFIER,         // e.g., width, px, red

    // Operators
    PLUS, MINUS,        // + -
    STAR, SLASH,        // * /
    PERCENT,            // %
    POWER,              // **

    // Logical Operators
    AND, OR,            // && ||

    // Comparison Operators
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    // Ternary
    QUESTION, COLON,    // ? :

    // Selectors & References
    SELECTOR,           // e.g., #box, .container, div[0]
    DOT,                // .

    // Misc
    LPAREN, RPAREN,     // ( )
    COMMA,              // ,
    EOF
}
