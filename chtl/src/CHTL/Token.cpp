#include "CHTL/Token.h"
#include <sstream>

namespace CHTL {

Token::Token(TokenType type, const std::string& value, size_t line, size_t column)
    : type_(type), value_(value), line_(line), column_(column) {
}

bool Token::isKeyword() const {
    switch (type_) {
        case TokenType::TEXT:
        case TokenType::STYLE:
        case TokenType::SCRIPT:
        case TokenType::TEMPLATE:
        case TokenType::CUSTOM:
        case TokenType::ORIGIN:
        case TokenType::IMPORT:
        case TokenType::NAMESPACE:
        case TokenType::CONFIGURATION:
        case TokenType::USE:
        case TokenType::EXCEPT:
        case TokenType::DELETE:
        case TokenType::INSERT:
        case TokenType::INHERIT_VALUE:
        case TokenType::FROM:
        case TokenType::AS:
        case TokenType::AT:
        case TokenType::TOP_POS:
        case TokenType::BOTTOM_POS:
        case TokenType::AFTER:
        case TokenType::BEFORE:
        case TokenType::REPLACE:
        case TokenType::HTML:
        case TokenType::HEAD:
        case TokenType::BODY:
        case TokenType::DIV:
        case TokenType::SPAN:
        case TokenType::P:
        case TokenType::A:
        case TokenType::IMG:
        case TokenType::BR:
        case TokenType::HR:
        case TokenType::H1:
        case TokenType::H2:
        case TokenType::H3:
        case TokenType::H4:
        case TokenType::H5:
        case TokenType::H6:
        case TokenType::UL:
        case TokenType::OL:
        case TokenType::LI:
        case TokenType::TABLE:
        case TokenType::TR:
        case TokenType::TD:
        case TokenType::TH:
        case TokenType::FORM:
        case TokenType::INPUT:
        case TokenType::BUTTON:
        case TokenType::CLASS:
        case TokenType::ID:
        case TokenType::WIDTH:
        case TokenType::HEIGHT:
        case TokenType::COLOR:
        case TokenType::BACKGROUND:
        case TokenType::MARGIN:
        case TokenType::PADDING:
        case TokenType::BORDER:
        case TokenType::FONT:
        case TokenType::TEXT_ALIGN:
        case TokenType::DISPLAY:
        case TokenType::POSITION:
        case TokenType::FLOAT:
        case TokenType::CLEAR:
        case TokenType::OVERFLOW:
        case TokenType::Z_INDEX:
        case TokenType::OPACITY:
        case TokenType::TRANSFORM:
        case TokenType::TRANSITION:
        case TokenType::ANIMATION:
        case TokenType::PX:
        case TokenType::EM:
        case TokenType::REM:
        case TokenType::VW:
        case TokenType::VH:
        case TokenType::RED:
        case TokenType::GREEN:
        case TokenType::BLUE:
        case TokenType::WHITE:
        case TokenType::BLACK:
        case TokenType::YELLOW:
        case TokenType::ORANGE:
        case TokenType::PURPLE:
        case TokenType::PINK:
        case TokenType::GRAY:
        case TokenType::TRANSPARENT:
        case TokenType::LEFT:
        case TokenType::RIGHT:
        case TokenType::CENTER:
        case TokenType::MIDDLE:
        case TokenType::BLOCK:
        case TokenType::INLINE:
        case TokenType::INLINE_BLOCK:
        case TokenType::FLEX:
        case TokenType::GRID:
        case TokenType::NONE:
        case TokenType::AUTO:
        case TokenType::INITIAL:
        case TokenType::UNSET:
        case TokenType::IMPORTANT:
            return true;
        default:
            return false;
    }
}

bool Token::isOperator() const {
    switch (type_) {
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::ASTERISK:
        case TokenType::SLASH:
        case TokenType::PERCENT:
        case TokenType::EQUALS:
        case TokenType::LESS:
        case TokenType::GREATER:
        case TokenType::AMPERSAND:
        case TokenType::PIPE:
        case TokenType::EXCLAMATION:
        case TokenType::QUESTION:
            return true;
        default:
            return false;
    }
}

bool Token::isLiteral() const {
    return type_ == TokenType::LITERAL || type_ == TokenType::STRING || type_ == TokenType::NUMBER;
}

std::string Token::toString() const {
    std::stringstream ss;
    ss << "Token(" << static_cast<int>(type_) << ", \"" << value_ << "\", " 
       << line_ << ":" << column_ << ")";
    return ss.str();
}

} // namespace CHTL