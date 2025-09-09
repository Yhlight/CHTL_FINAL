#include "CHTLJSToken.hpp"
#include <sstream>

namespace CHTL {

CHTLJSToken::CHTLJSToken(CHTLJSTokenType type, const std::string& value, size_t line, size_t column)
    : type_(type), value_(value), line_(line), column_(column) {}

CHTLJSToken::CHTLJSToken() 
    : type_(CHTLJSTokenType::END_OF_FILE), value_(""), line_(0), column_(0) {}

std::string CHTLJSToken::toString() const {
    std::ostringstream oss;
    oss << "CHTLJSToken(type=" << static_cast<int>(type_)
        << ", value=\"" << value_ << "\""
        << ", line=" << line_ << ", column=" << column_ << ")";
    return oss.str();
}

} // namespace CHTL