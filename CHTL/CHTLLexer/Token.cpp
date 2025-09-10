#include "Token.h"

std::string Token::toString() const {
    std::string typeStr;
    switch (type) {
        case TokenType::LEFT_BRACE: typeStr = "LEFT_BRACE"; break;
        case TokenType::RIGHT_BRACE: typeStr = "RIGHT_BRACE"; break;
        case TokenType::LEFT_BRACKET: typeStr = "LEFT_BRACKET"; break;
        case TokenType::RIGHT_BRACKET: typeStr = "RIGHT_BRACKET"; break;
        case TokenType::COLON: typeStr = "COLON"; break;
        case TokenType::SEMICOLON: typeStr = "SEMICOLON"; break;
        case TokenType::AT: typeStr = "AT"; break;
        case TokenType::EQUAL: typeStr = "EQUAL"; break;
        case TokenType::SLASH: typeStr = "SLASH"; break;
        case TokenType::STAR: typeStr = "STAR"; break;
        case TokenType::LINE_COMMENT: typeStr = "LINE_COMMENT"; break;
        case TokenType::BLOCK_COMMENT_START: typeStr = "BLOCK_COMMENT_START"; break;
        case TokenType::BLOCK_COMMENT_END: typeStr = "BLOCK_COMMENT_END"; break;
        case TokenType::GEN_COMMENT: typeStr = "GEN_COMMENT"; break;
        case TokenType::IDENTIFIER: typeStr = "IDENTIFIER"; break;
        case TokenType::STRING: typeStr = "STRING"; break;
        case TokenType::UNQUOTED_LITERAL: typeStr = "UNQUOTED_LITERAL"; break;
        case TokenType::TEXT: typeStr = "TEXT"; break;
        case TokenType::STYLE: typeStr = "STYLE"; break;
        case TokenType::SCRIPT: typeStr = "SCRIPT"; break;
        case TokenType::UNKNOWN: typeStr = "UNKNOWN"; break;
        case TokenType::END_OF_FILE: typeStr = "END_OF_FILE"; break;
        default: typeStr = "ERROR_UNKNOWN_TOKEN"; break;
    }
    return "Token(" + typeStr + ", '" + lexeme + "', " + std::to_string(line) + ")";
}
