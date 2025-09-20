#include "Token.h"

namespace CHTL {

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        // Single-character tokens
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::AT: return "AT";
        case TokenType::COLON: return "COLON";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::QUESTION: return "QUESTION";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::SLASH: return "SLASH";
        case TokenType::PERCENT: return "PERCENT";
        case TokenType::GREATER: return "GREATER";
        case TokenType::LESS: return "LESS";

        // One or two character tokens
        case TokenType::BANG: return "BANG";
        case TokenType::BANG_EQUAL: return "BANG_EQUAL";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::AMPERSAND: return "AMPERSAND";
        case TokenType::AMPERSAND_AMPERSAND: return "AMPERSAND_AMPERSAND";
        case TokenType::PIPE: return "PIPE";
        case TokenType::PIPE_PIPE: return "PIPE_PIPE";

        // Two-character tokens
        case TokenType::STAR_STAR: return "STAR_STAR";

        // Literals
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING: return "STRING";
        case TokenType::NUMBER: return "NUMBER";

        // Keywords
        case TokenType::STYLE: return "STYLE";
        case TokenType::TEXT: return "TEXT";
        case TokenType::INHERIT: return "INHERIT";
        case TokenType::TEMPLATE: return "TEMPLATE";
        case TokenType::CUSTOM: return "CUSTOM";
        case TokenType::ORIGIN: return "ORIGIN";
        case TokenType::IMPORT: return "IMPORT";
        case TokenType::NAMESPACE: return "NAMESPACE";
        case TokenType::CONFIGURATION: return "CONFIGURATION";
        case TokenType::ELEMENT: return "ELEMENT";
        case TokenType::VAR: return "VAR";
        case TokenType::HTML: return "HTML";
        case TokenType::JAVASCRIPT: return "JAVASCRIPT";
        case TokenType::CHTL: return "CHTL";
        case TokenType::FROM: return "FROM";
        case TokenType::AS: return "AS";
        case TokenType::DELETE: return "DELETE";
        case TokenType::INSERT: return "INSERT";
        case TokenType::AFTER: return "AFTER";
        case TokenType::BEFORE: return "BEFORE";
        case TokenType::REPLACE: return "REPLACE";
        case TokenType::TOP: return "TOP";
        case TokenType::BOTTOM: return "BOTTOM";
        case TokenType::EXCEPT: return "EXCEPT";
        case TokenType::USE: return "USE";
        case TokenType::HTML5: return "HTML5";
        case TokenType::SCRIPT: return "SCRIPT";

        // Special
        case TokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        case TokenType::SYMBOL: return "SYMBOL";
        case TokenType::DOLLAR: return "DOLLAR";
        case TokenType::END_OF_FILE: return "EOF";
        case TokenType::UNKNOWN: return "UNKNOWN";

        default: return "ERROR_UNKNOWN_TOKEN";
    }
}

} // namespace CHTL
