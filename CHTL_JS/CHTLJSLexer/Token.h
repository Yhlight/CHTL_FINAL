#pragma once

#include <string>
#include <vector>

enum class CHTLJSTokenType {
    // Special Tokens
    EndOfFile,
    Unexpected,

    // Literals
    Identifier,
    StringLiteral,
    NumberLiteral,

    // Symbols
    OpenBrace,          // {
    CloseBrace,         // }
    OpenParen,          // (
    CloseParen,         // )
    OpenBracket,        // [
    CloseBracket,       // ]
    Colon,              // :
    Semicolon,          // ;
    Comma,              // ,
    Dot,                // .
    DoubleCurlyOpen,    // {{
    DoubleCurlyClose,   // }}
    Equals,             // =
    Arrow,              // ->
    FatArrow,           // =>

    // Keywords
    FileLoader,         // fileloader
    Listen,             // listen
    Delegate,           // delegate
    Animate,            // animate
    Vir,                // vir
    Router,             // router
    Util,               // util
    Then,               // then
    INeverAway,         // iNeverAway
    PrintMyLove,        // printMylove

    // Raw JS code block (to be handled by JS parser)
    JavaScript,
};

struct CHTLJSToken {
    CHTLJSTokenType type;
    std::string value;
    int line;
    int column;
};
