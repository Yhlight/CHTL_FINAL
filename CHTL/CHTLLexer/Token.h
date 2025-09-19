#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>

namespace CHTL {

enum class TokenType {
    // Single-character tokens.
    LEFT_BRACE, RIGHT_BRACE,     // { }
    LEFT_BRACKET, RIGHT_BRACKET, // [ ]
    LEFT_PAREN, RIGHT_PAREN,     // ( )
    AT,                          // @
    COLON, SEMICOLON,            // : ;
    DOT,                         // .
    QUESTION,                    // ?
    COMMA,                       // ,
    PLUS, MINUS, STAR, SLASH, PERCENT,
    GREATER, LESS,

    // One or two character tokens
    BANG, BANG_EQUAL,            // ! !=
    EQUAL, EQUAL_EQUAL,          // = ==
    GREATER_EQUAL, LESS_EQUAL,    // >= <=
    AMPERSAND, AMPERSAND_AMPERSAND, // & &&
    PIPE, PIPE_PIPE,             // | ||

    // Two-character tokens
    STAR_STAR,

    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords
    STYLE,
    TEXT,
    INHERIT,
    FROM,
    AS,
    DELETE,
    INSERT,
    AFTER,
    BEFORE,
    REPLACE,
	AT_TOP,
	AT_BOTTOM,
	USE,
	EXCEPT,

	// Brackets
	KEYWORD_CUSTOM,
	KEYWORD_TEMPLATE,
	KEYWORD_ORIGIN,
	KEYWORD_IMPORT,
	KEYWORD_NAMESPACE,
	KEYWORD_CONFIGURATION,
	KEYWORD_INFO,
	KEYWORD_EXPORT,
	KEYWORD_NAME,
	KEYWORD_ORIGIN_TYPE,


	// CHTL JS
	KEYWORD_SCRIPT,
	KEYWORD_SCRIPT_LOADER,
	KEYWORD_LISTEN,
	KEYWORD_DELEGATE,
	KEYWORD_ANIMATE,
	KEYWORD_VIR,
	KEYWORD_ROUTER,
	KEYWORD_I_NEVER_AWAY,
	KEYWORD_PRINT_MY_LOVE,
	KEYWORD_UTIL,
	KEYWORD_THEN,
	KEYWORD_CHANGE,

    // Special
    SYMBOL,
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int position;
};

std::string tokenTypeToString(TokenType type);

} // namespace CHTL

#endif // TOKEN_H
