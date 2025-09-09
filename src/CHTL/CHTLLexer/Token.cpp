#include "Token.h"

std::string Token::toString() const {
    std::string typeStr;
    switch (type) {
        case TokenType::EndOfFile: typeStr = "EndOfFile"; break;
        case TokenType::Unknown: typeStr = "Unknown"; break;
        case TokenType::Identifier: typeStr = "Identifier"; break;
        case TokenType::UnquotedLiteral: typeStr = "UnquotedLiteral"; break;
        case TokenType::StringLiteral: typeStr = "StringLiteral"; break;
        case TokenType::LeftBrace: typeStr = "LeftBrace"; break;
        case TokenType::RightBrace: typeStr = "RightBrace"; break;
        case TokenType::LeftParen: typeStr = "LeftParen"; break;
        case TokenType::RightParen: typeStr = "RightParen"; break;
        case TokenType::LeftBracket: typeStr = "LeftBracket"; break;
        case TokenType::RightBracket: typeStr = "RightBracket"; break;
        case TokenType::Colon: typeStr = "Colon"; break;
        case TokenType::Semicolon: typeStr = "Semicolon"; break;
        case TokenType::At: typeStr = "At"; break;
        case TokenType::Hash: typeStr = "Hash"; break;
        case TokenType::Dot: typeStr = "Dot"; break;
        case TokenType::Comma: typeStr = "Comma"; break;
        case TokenType::Ampersand: typeStr = "Ampersand"; break;
        case TokenType::GreaterThan: typeStr = "GreaterThan"; break;
        case TokenType::LessThan: typeStr = "LessThan"; break;
        case TokenType::Text: typeStr = "Text"; break;
        case TokenType::Style: typeStr = "Style"; break;
        case TokenType::TemplateKeyword: typeStr = "TemplateKeyword"; break;
        case TokenType::NamespaceKeyword: typeStr = "NamespaceKeyword"; break;
        case TokenType::ImportKeyword: typeStr = "ImportKeyword"; break;
        case TokenType::FromKeyword: typeStr = "FromKeyword"; break;
        default: typeStr = "Unknown"; break;
    }
    return "Token(" + typeStr + ", '" + lexeme + "', " + std::to_string(line) + ":" + std::to_string(column) + " @ " + std::to_string(offset) + ")";
}
