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
        case TokenType::PLUS: typeStr = "PLUS"; break;
        case TokenType::MINUS: typeStr = "MINUS"; break;
        case TokenType::STAR: typeStr = "STAR"; break;
        case TokenType::SLASH: typeStr = "SLASH"; break;
        case TokenType::PERCENT: typeStr = "PERCENT"; break;
        case TokenType::GREATER: typeStr = "GREATER"; break;
        case TokenType::LESS: typeStr = "LESS"; break;
        case TokenType::AMPERSAND: typeStr = "AMPERSAND"; break;
        case TokenType::DOT: typeStr = "DOT"; break;
        case TokenType::HASH: typeStr = "HASH"; break;
        case TokenType::QUESTION: typeStr = "QUESTION"; break;
        case TokenType::STAR_STAR: typeStr = "STAR_STAR"; break;
        case TokenType::GEN_COMMENT: typeStr = "GEN_COMMENT"; break;
        case TokenType::IDENTIFIER: typeStr = "IDENTIFIER"; break;
        case TokenType::STRING: typeStr = "STRING"; break;
        case TokenType::NUMBER: typeStr = "NUMBER"; break;
        case TokenType::TEXT: typeStr = "TEXT"; break;
        case TokenType::STYLE: typeStr = "STYLE"; break;
        case TokenType::SCRIPT: typeStr = "SCRIPT"; break;
        case TokenType::KEYWORD_ORIGIN: typeStr = "KEYWORD_ORIGIN"; break;
        case TokenType::KEYWORD_IMPORT: typeStr = "KEYWORD_IMPORT"; break;
        case TokenType::KEYWORD_FROM: typeStr = "KEYWORD_FROM"; break;
        case TokenType::KEYWORD_AS: typeStr = "KEYWORD_AS"; break;
        case TokenType::UNKNOWN: typeStr = "UNKNOWN"; break;
        case TokenType::END_OF_FILE: typeStr = "END_OF_FILE"; break;
        default: typeStr = "ERROR_UNKNOWN_TOKEN"; break;
    }
    return "Token(" + typeStr + ", '" + lexeme + "', content='" + content + "', " + std::to_string(line) + ")";
}
