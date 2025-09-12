#include "Token.h"
#include <map>

namespace CHTL {

// Using a map for easy conversion of TokenType to string for debugging
static const std::map<TokenType, std::string> tokenTypeToStringMap = {
    {TokenType::LEFT_BRACE, "LEFT_BRACE"}, {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
    {TokenType::LEFT_PAREN, "LEFT_PAREN"}, {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
    {TokenType::LEFT_BRACKET, "LEFT_BRACKET"}, {TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
    {TokenType::COMMA, "COMMA"}, {TokenType::DOT, "DOT"}, {TokenType::MINUS, "MINUS"},
    {TokenType::PLUS, "PLUS"}, {TokenType::STAR, "STAR"}, {TokenType::SEMICOLON, "SEMICOLON"},
    {TokenType::SLASH, "SLASH"}, {TokenType::PERCENT, "PERCENT"}, {TokenType::COLON, "COLON"},
    {TokenType::EQUAL, "EQUAL"}, {TokenType::QUESTION_MARK, "QUESTION_MARK"},
    {TokenType::BANG, "BANG"}, {TokenType::BANG_EQUAL, "BANG_EQUAL"},
    {TokenType::EQUAL_EQUAL, "EQUAL_EQUAL"}, {TokenType::GREATER, "GREATER"},
    {TokenType::GREATER_EQUAL, "GREATER_EQUAL"}, {TokenType::LESS, "LESS"},
    {TokenType::LESS_EQUAL, "LESS_EQUAL"}, {TokenType::STAR_STAR, "STAR_STAR"},
    {TokenType::CHTL_COMMENT, "CHTL_COMMENT"}, {TokenType::ARROW, "ARROW"},
    {TokenType::IDENTIFIER, "IDENTIFIER"}, {TokenType::STRING, "STRING"},
    {TokenType::NUMBER, "NUMBER"}, {TokenType::UNQUOTED_LITERAL, "UNQUOTED_LITERAL"},
    {TokenType::KEYWORD_TEXT, "KEYWORD_TEXT"}, {TokenType::KEYWORD_STYLE, "KEYWORD_STYLE"},
    {TokenType::KEYWORD_SCRIPT, "KEYWORD_SCRIPT"}, {TokenType::KEYWORD_INHERIT, "KEYWORD_INHERIT"},
    {TokenType::KEYWORD_DELETE, "KEYWORD_DELETE"}, {TokenType::KEYWORD_INSERT, "KEYWORD_INSERT"},
    {TokenType::KEYWORD_AFTER, "KEYWORD_AFTER"}, {TokenType::KEYWORD_BEFORE, "KEYWORD_BEFORE"},
    {TokenType::KEYWORD_REPLACE, "KEYWORD_REPLACE"}, {TokenType::KEYWORD_ATTOP, "KEYWORD_ATTOP"},
    {TokenType::KEYWORD_ATBOTTOM, "KEYWORD_ATBOTTOM"}, {TokenType::KEYWORD_FROM, "KEYWORD_FROM"},
    {TokenType::KEYWORD_AS, "KEYWORD_AS"}, {TokenType::KEYWORD_EXCEPT, "KEYWORD_EXCEPT"},
    {TokenType::KEYWORD_USE, "KEYWORD_USE"}, {TokenType::KEYWORD_HTML5, "KEYWORD_HTML5"},
    {TokenType::KEYWORD_CUSTOM, "KEYWORD_CUSTOM"}, {TokenType::KEYWORD_TEMPLATE, "KEYWORD_TEMPLATE"},
    {TokenType::KEYWORD_ORIGIN, "KEYWORD_ORIGIN"}, {TokenType::KEYWORD_IMPORT, "KEYWORD_IMPORT"},
    {TokenType::KEYWORD_NAMESPACE, "KEYWORD_NAMESPACE"}, {TokenType::KEYWORD_CONFIGURATION, "KEYWORD_CONFIGURATION"},
    {TokenType::KEYWORD_INFO, "KEYWORD_INFO"}, {TokenType::KEYWORD_EXPORT, "KEYWORD_EXPORT"},
    {TokenType::KEYWORD_ORIGIN_TYPE, "KEYWORD_ORIGIN_TYPE"}, {TokenType::KEYWORD_NAME, "KEYWORD_NAME"},
    {TokenType::KEYWORD_AT_STYLE, "KEYWORD_AT_STYLE"}, {TokenType::KEYWORD_AT_ELEMENT, "KEYWORD_AT_ELEMENT"},
    {TokenType::KEYWORD_AT_VAR, "KEYWORD_AT_VAR"}, {TokenType::KEYWORD_AT_HTML, "KEYWORD_AT_HTML"},
    {TokenType::KEYWORD_AT_JAVASCRIPT, "KEYWORD_AT_JAVASCRIPT"}, {TokenType::KEYWORD_AT_CHTL, "KEYWORD_AT_CHTL"},
    {TokenType::KEYWORD_AT_CJMOD, "KEYWORD_AT_CJMOD"}, {TokenType::KEYWORD_AT_CONFIG, "KEYWORD_AT_CONFIG"},
    {TokenType::KEYWORD_AT_CUSTOM, "KEYWORD_AT_CUSTOM"},
    {TokenType::UNKNOWN, "UNKNOWN"}, {TokenType::END_OF_FILE, "END_OF_FILE"}
};

Token::Token(TokenType type, const std::string& lexeme, int line, int column)
    : type(type), lexeme(lexeme), line(line), column(column) {}

std::string Token::toString() const {
    auto it = tokenTypeToStringMap.find(type);
    std::string typeStr = (it != tokenTypeToStringMap.end()) ? it->second : "INVALID_TOKEN_TYPE";
    return "Token(" + typeStr + ", '" + lexeme + "', " + std::to_string(line) + ":" + std::to_string(column) + ")";
}

} // namespace CHTL
