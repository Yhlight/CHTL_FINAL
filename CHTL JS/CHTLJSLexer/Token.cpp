#include "Token.h"
#include <sstream>
#include <map>

namespace CHTLJS {

std::string CHTLJS_Token::toString() const {
    std::stringstream ss;
    ss << "CHTLJS_Token(" << static_cast<int>(type) << ", \"" << value << "\", " 
       << line << ", " << column << ")";
    return ss.str();
}

bool CHTLJS_Token::isKeyword() const {
    return type == CHTLJS_TokenType::SCRIPT_LOADER ||
           type == CHTLJS_TokenType::LISTEN ||
           type == CHTLJS_TokenType::ANIMATE ||
           type == CHTLJS_TokenType::ROUTER ||
           type == CHTLJS_TokenType::VIR ||
           type == CHTLJS_TokenType::UTIL ||
           type == CHTLJS_TokenType::CHANGE ||
           type == CHTLJS_TokenType::THEN ||
           type == CHTLJS_TokenType::PRINTMYLOVE ||
           type == CHTLJS_TokenType::INEVERAWAY;
}

bool CHTLJS_Token::isOperator() const {
    return type == CHTLJS_TokenType::BIND_OPERATOR ||
           type == CHTLJS_TokenType::ARROW_OPERATOR ||
           type == CHTLJS_TokenType::ASSIGN ||
           type == CHTLJS_TokenType::EQUAL ||
           type == CHTLJS_TokenType::NOT_EQUAL ||
           type == CHTLJS_TokenType::LESS ||
           type == CHTLJS_TokenType::GREATER ||
           type == CHTLJS_TokenType::LESS_EQUAL ||
           type == CHTLJS_TokenType::GREATER_EQUAL ||
           type == CHTLJS_TokenType::AND ||
           type == CHTLJS_TokenType::OR ||
           type == CHTLJS_TokenType::NOT ||
           type == CHTLJS_TokenType::PLUS ||
           type == CHTLJS_TokenType::MINUS ||
           type == CHTLJS_TokenType::MULTIPLY ||
           type == CHTLJS_TokenType::DIVIDE ||
           type == CHTLJS_TokenType::MODULO ||
           type == CHTLJS_TokenType::POWER;
}

bool CHTLJS_Token::isLiteral() const {
    return type == CHTLJS_TokenType::STRING_LITERAL ||
           type == CHTLJS_TokenType::NUMBER_LITERAL ||
           type == CHTLJS_TokenType::BOOLEAN_LITERAL;
}

bool CHTLJS_Token::isCHTLJSSpecific() const {
    return type == CHTLJS_TokenType::SCRIPT_LOADER ||
           type == CHTLJS_TokenType::LISTEN ||
           type == CHTLJS_TokenType::ANIMATE ||
           type == CHTLJS_TokenType::ROUTER ||
           type == CHTLJS_TokenType::VIR ||
           type == CHTLJS_TokenType::UTIL ||
           type == CHTLJS_TokenType::CHANGE ||
           type == CHTLJS_TokenType::THEN ||
           type == CHTLJS_TokenType::PRINTMYLOVE ||
           type == CHTLJS_TokenType::INEVERAWAY ||
           type == CHTLJS_TokenType::BIND_OPERATOR ||
           type == CHTLJS_TokenType::ARROW_OPERATOR ||
           type == CHTLJS_TokenType::RESPONSIVE_GET ||
           type == CHTLJS_TokenType::RESPONSIVE_SET ||
           type == CHTLJS_TokenType::CHTL_SELECTOR ||
           type == CHTLJS_TokenType::CHTL_RESPONSIVE ||
           type == CHTLJS_TokenType::CHTL_FUNCTION;
}

} // namespace CHTLJS