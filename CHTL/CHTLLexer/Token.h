#pragma once
#include <string>
namespace CHTL {
enum class TokenType {
    ILLEGAL, END_OF_FILE, IDENTIFIER, STRING_LITERAL,
    LEFT_BRACE, RIGHT_BRACE, COLON, EQUALS, SEMICOLON,
    DOT, HASH, AMPERSAND
};
struct Token { TokenType type; std::string literal; };
}
