#include "Token.h"

namespace CHTL
{
    std::string tokenTypeToString(TokenType type)
    {
        switch (type)
        {
            case TokenType::LEFT_BRACE: return "LEFT_BRACE";
            case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
            case TokenType::LEFT_PAREN: return "LEFT_PAREN";
            case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
            case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
            case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
            case TokenType::COLON: return "COLON";
            case TokenType::SEMICOLON: return "SEMICOLON";
            case TokenType::COMMA: return "COMMA";
            case TokenType::DOT: return "DOT";
            case TokenType::EQUAL: return "EQUAL";
            case TokenType::PLUS: return "PLUS";
            case TokenType::MINUS: return "MINUS";
            case TokenType::STAR: return "STAR";
            case TokenType::SLASH: return "SLASH";
            case TokenType::PERCENT: return "PERCENT";
            case TokenType::POWER: return "POWER";
            case TokenType::IDENTIFIER: return "IDENTIFIER";
            case TokenType::STRING: return "STRING";
            case TokenType::NUMBER: return "NUMBER";
            case TokenType::SINGLE_LINE_COMMENT: return "SINGLE_LINE_COMMENT";
            case TokenType::MULTI_LINE_COMMENT: return "MULTI_LINE_COMMENT";
            case TokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
            case TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
            case TokenType::AMPERSAND: return "AMPERSAND";
            case TokenType::END_OF_FILE: return "END_OF_FILE";
            case TokenType::ILLEGAL: return "ILLEGAL";
            default: return "UNKNOWN";
        }
    }
}
