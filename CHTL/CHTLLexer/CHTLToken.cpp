#include "CHTLToken.h"
#include <sstream>

namespace CHTL {

CHTLToken::CHTLToken(CHTLTokenType type, const std::string& value, size_t line, size_t column)
    : type(type), value(value), line(line), column(column) {
}

CHTLTokenType CHTLToken::getType() const {
    return type;
}

std::string CHTLToken::getValue() const {
    return value;
}

size_t CHTLToken::getLine() const {
    return line;
}

size_t CHTLToken::getColumn() const {
    return column;
}

std::string CHTLToken::toString() const {
    std::ostringstream oss;
    oss << "CHTLToken(" << static_cast<int>(type) << ", \"" << value << "\", " 
        << line << ", " << column << ")";
    return oss.str();
}

bool CHTLToken::isOperator() const {
    return type == CHTLTokenType::COLON ||
           type == CHTLTokenType::EQUAL ||
           type == CHTLTokenType::SEMICOLON ||
           type == CHTLTokenType::COMMA ||
           type == CHTLTokenType::DOT ||
           type == CHTLTokenType::HASH ||
           type == CHTLTokenType::PERCENT ||
           type == CHTLTokenType::AMPERSAND ||
           type == CHTLTokenType::PIPE ||
           type == CHTLTokenType::QUESTION ||
           type == CHTLTokenType::EXCLAMATION ||
           type == CHTLTokenType::UNDERSCORE;
}

bool CHTLToken::isKeyword() const {
    return type == CHTLTokenType::TEXT ||
           type == CHTLTokenType::TEMPLATE ||
           type == CHTLTokenType::CUSTOM ||
           type == CHTLTokenType::ORIGIN ||
           type == CHTLTokenType::IMPORT ||
           type == CHTLTokenType::CONFIGURATION ||
           type == CHTLTokenType::NAMESPACE ||
           type == CHTLTokenType::INFO ||
           type == CHTLTokenType::EXPORT ||
           type == CHTLTokenType::INHERIT ||
           type == CHTLTokenType::DELETE ||
           type == CHTLTokenType::INSERT ||
           type == CHTLTokenType::AFTER ||
           type == CHTLTokenType::BEFORE ||
           type == CHTLTokenType::REPLACE ||
           type == CHTLTokenType::AT_TOP ||
           type == CHTLTokenType::AT_BOTTOM ||
           type == CHTLTokenType::FROM ||
           type == CHTLTokenType::AS ||
           type == CHTLTokenType::EXCEPT ||
           type == CHTLTokenType::USE ||
           type == CHTLTokenType::HTML5;
}

bool CHTLToken::isLiteral() const {
    return type == CHTLTokenType::STRING ||
           type == CHTLTokenType::NUMBER ||
           type == CHTLTokenType::BOOLEAN ||
           type == CHTLTokenType::IDENTIFIER;
}

bool CHTLToken::isPunctuation() const {
    return type == CHTLTokenType::LEFT_PAREN ||
           type == CHTLTokenType::RIGHT_PAREN ||
           type == CHTLTokenType::LEFT_BRACE ||
           type == CHTLTokenType::RIGHT_BRACE ||
           type == CHTLTokenType::LEFT_BRACKET ||
           type == CHTLTokenType::RIGHT_BRACKET ||
           type == CHTLTokenType::LEFT_ANGLE ||
           type == CHTLTokenType::RIGHT_ANGLE;
}

bool CHTLToken::isHTMLTag() const {
    return type == CHTLTokenType::HTML ||
           type == CHTLTokenType::HEAD ||
           type == CHTLTokenType::BODY ||
           type == CHTLTokenType::TITLE ||
           type == CHTLTokenType::DIV ||
           type == CHTLTokenType::SPAN ||
           type == CHTLTokenType::P ||
           type == CHTLTokenType::A ||
           type == CHTLTokenType::IMG ||
           type == CHTLTokenType::BR ||
           type == CHTLTokenType::HR ||
           type == CHTLTokenType::UL ||
           type == CHTLTokenType::OL ||
           type == CHTLTokenType::LI ||
           type == CHTLTokenType::TABLE ||
           type == CHTLTokenType::TR ||
           type == CHTLTokenType::TD ||
           type == CHTLTokenType::TH ||
           type == CHTLTokenType::FORM ||
           type == CHTLTokenType::INPUT ||
           type == CHTLTokenType::BUTTON ||
           type == CHTLTokenType::TEXTAREA ||
           type == CHTLTokenType::SELECT ||
           type == CHTLTokenType::OPTION ||
           type == CHTLTokenType::SCRIPT ||
           type == CHTLTokenType::STYLE ||
           type == CHTLTokenType::LINK ||
           type == CHTLTokenType::META;
}

bool CHTLToken::isCHTLKeyword() const {
    return type == CHTLTokenType::TEXT ||
           type == CHTLTokenType::TEMPLATE ||
           type == CHTLTokenType::CUSTOM ||
           type == CHTLTokenType::ORIGIN ||
           type == CHTLTokenType::IMPORT ||
           type == CHTLTokenType::CONFIGURATION ||
           type == CHTLTokenType::NAMESPACE ||
           type == CHTLTokenType::INFO ||
           type == CHTLTokenType::EXPORT;
}

} // namespace CHTL