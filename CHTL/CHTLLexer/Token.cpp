#include "Token.h"
#include <map>

// Using a map for easy lookup. Could be a switch or array for performance if needed.
std::string tokenTypeToString(TokenType type) {
    static const std::map<TokenType, std::string> typeMap = {
        {TokenType::ERROR, "ERROR"},
        {TokenType::END_OF_FILE, "END_OF_FILE"},
        {TokenType::LEFT_BRACE, "LEFT_BRACE"},
        {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
        {TokenType::LEFT_PAREN, "LEFT_PAREN"},
        {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
        {TokenType::LEFT_BRACKET, "LEFT_BRACKET"},
        {TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
        {TokenType::COLON, "COLON"},
        {TokenType::SEMICOLON, "SEMICOLON"},
        {TokenType::COMMA, "COMMA"},
        {TokenType::DOT, "DOT"},
        {TokenType::PLUS, "PLUS"},
        {TokenType::MINUS, "MINUS"},
        {TokenType::SLASH, "SLASH"},
        {TokenType::STAR, "STAR"},
        {TokenType::PERCENT, "PERCENT"},
        {TokenType::AMPERSAND, "AMPERSAND"},
        {TokenType::PIPE, "PIPE"},
        {TokenType::QUESTION, "QUESTION"},
        {TokenType::EQUAL, "EQUAL"},
        {TokenType::LESS, "LESS"},
        {TokenType::GREATER, "GREATER"},
        {TokenType::BANG, "BANG"},
        {TokenType::HASH, "HASH"},
        {TokenType::AT, "AT"},
        {TokenType::STAR_STAR, "STAR_STAR"},
        {TokenType::BANG_EQUAL, "BANG_EQUAL"},
        {TokenType::EQUAL_EQUAL, "EQUAL_EQUAL"},
        {TokenType::LESS_EQUAL, "LESS_EQUAL"},
        {TokenType::GREATER_EQUAL, "GREATER_EQUAL"},
        {TokenType::AMPERSAND_AMPERSAND, "AMPERSAND_AMPERSAND"},
        {TokenType::PIPE_PIPE, "PIPE_PIPE"},
        {TokenType::MINUS_GREATER, "MINUS_GREATER"},
        {TokenType::IDENTIFIER, "IDENTIFIER"},
        {TokenType::STRING, "STRING"},
        {TokenType::NUMBER, "NUMBER"},
        {TokenType::UNQUOTED_LITERAL, "UNQUOTED_LITERAL"},
        {TokenType::TEXT, "TEXT"},
        {TokenType::STYLE, "STYLE"},
        {TokenType::SCRIPT, "SCRIPT"},
        {TokenType::INHERIT, "INHERIT"},
        {TokenType::DELETE, "DELETE"},
        {TokenType::INSERT, "INSERT"},
        {TokenType::AFTER, "AFTER"},
        {TokenType::BEFORE, "BEFORE"},
        {TokenType::REPLACE, "REPLACE"},
        {TokenType::AT_TOP, "AT_TOP"},
        {TokenType::AT_BOTTOM, "AT_BOTTOM"},
        {TokenType::FROM, "FROM"},
        {TokenType::AS, "AS"},
        {TokenType::EXCEPT, "EXCEPT"},
        {TokenType::USE, "USE"},
        {TokenType::HTML5, "HTML5"},
        {TokenType::KEYWORD_CUSTOM, "KEYWORD_CUSTOM"},
        {TokenType::KEYWORD_TEMPLATE, "KEYWORD_TEMPLATE"},
        {TokenType::KEYWORD_ORIGIN, "KEYWORD_ORIGIN"},
        {TokenType::KEYWORD_IMPORT, "KEYWORD_IMPORT"},
        {TokenType::KEYWORD_NAMESPACE, "KEYWORD_NAMESPACE"},
        {TokenType::KEYWORD_CONFIG, "KEYWORD_CONFIG"},
        {TokenType::KEYWORD_INFO, "KEYWORD_INFO"},
        {TokenType::KEYWORD_EXPORT, "KEYWORD_EXPORT"},
        {TokenType::KEYWORD_NAME, "KEYWORD_NAME"},
        {TokenType::KEYWORD_ORIGIN_TYPE, "KEYWORD_ORIGIN_TYPE"},
        {TokenType::COMMENT, "COMMENT"}
    };

    auto it = typeMap.find(type);
    if (it != typeMap.end()) {
        return it->second;
    }
    return "UNKNOWN_TOKEN";
}

std::string Token::toString() const {
    return "Token(" + tokenTypeToString(type) + ", '" + lexeme + "', " +
           std::to_string(line) + ":" + std::to_string(column) + ")";
}
