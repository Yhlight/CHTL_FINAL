#include "AtomArg.h"
#include <iostream>
#include <sstream>
#include <regex>

namespace CHTL {
namespace CJMOD {

AtomArg::AtomArg() 
    : type(PlaceholderType::NON_PLACEHOLDER), isPlaceholder(false), matchCount(0) {}

AtomArg::AtomArg(const std::string& token) 
    : token(token), type(PlaceholderType::NON_PLACEHOLDER), isPlaceholder(false), matchCount(0) {
    parseToken(token);
}

AtomArg::AtomArg(const std::string& token, const std::string& value) 
    : token(token), value(value), type(PlaceholderType::NON_PLACEHOLDER), isPlaceholder(false), matchCount(0) {
    parseToken(token);
}

AtomArg::AtomArg(const std::string& token, PlaceholderType type) 
    : token(token), type(type), isPlaceholder(true), matchCount(0) {}

std::string AtomArg::getToken() const {
    return token;
}

void AtomArg::setToken(const std::string& token) {
    this->token = token;
    parseToken(token);
}

std::string AtomArg::getValue() const {
    return value;
}

void AtomArg::setValue(const std::string& value) {
    this->value = value;
}

PlaceholderType AtomArg::getType() const {
    return type;
}

void AtomArg::setType(PlaceholderType type) {
    this->type = type;
    this->isPlaceholder = (type != PlaceholderType::NON_PLACEHOLDER);
}

bool AtomArg::isPlaceholder() const {
    return isPlaceholder;
}

bool AtomArg::isNormalPlaceholder() const {
    return type == PlaceholderType::NORMAL;
}

bool AtomArg::isOptional() const {
    return type == PlaceholderType::OPTIONAL || type == PlaceholderType::OPTIONAL_UNORDERED;
}

bool AtomArg::isRequired() const {
    return type == PlaceholderType::REQUIRED || type == PlaceholderType::REQUIRED_UNORDERED;
}

bool AtomArg::isUnordered() const {
    return type == PlaceholderType::UNORDERED || 
           type == PlaceholderType::OPTIONAL_UNORDERED || 
           type == PlaceholderType::REQUIRED_UNORDERED;
}

bool AtomArg::isVariadic() const {
    return type == PlaceholderType::VARIADIC;
}

PlaceholderType AtomArg::parsePlaceholderType(const std::string& token) {
    if (token == "$") {
        return PlaceholderType::NORMAL;
    } else if (token == "$?") {
        return PlaceholderType::OPTIONAL;
    } else if (token == "$!") {
        return PlaceholderType::REQUIRED;
    } else if (token == "$_") {
        return PlaceholderType::UNORDERED;
    } else if (token == "$?_") {
        return PlaceholderType::OPTIONAL_UNORDERED;
    } else if (token == "$!_") {
        return PlaceholderType::REQUIRED_UNORDERED;
    } else if (token == "...") {
        return PlaceholderType::VARIADIC;
    } else {
        return PlaceholderType::NON_PLACEHOLDER;
    }
}

std::string AtomArg::placeholderTypeToString(PlaceholderType type) {
    switch (type) {
        case PlaceholderType::NORMAL: return "NORMAL";
        case PlaceholderType::OPTIONAL: return "OPTIONAL";
        case PlaceholderType::REQUIRED: return "REQUIRED";
        case PlaceholderType::UNORDERED: return "UNORDERED";
        case PlaceholderType::OPTIONAL_UNORDERED: return "OPTIONAL_UNORDERED";
        case PlaceholderType::REQUIRED_UNORDERED: return "REQUIRED_UNORDERED";
        case PlaceholderType::VARIADIC: return "VARIADIC";
        case PlaceholderType::NON_PLACEHOLDER: return "NON_PLACEHOLDER";
        default: return "UNKNOWN";
    }
}

std::string AtomArg::getPlaceholderPrefix(PlaceholderType type) {
    switch (type) {
        case PlaceholderType::NORMAL: return "$";
        case PlaceholderType::OPTIONAL: return "$?";
        case PlaceholderType::REQUIRED: return "$!";
        case PlaceholderType::UNORDERED: return "$_";
        case PlaceholderType::OPTIONAL_UNORDERED: return "$?_";
        case PlaceholderType::REQUIRED_UNORDERED: return "$!_";
        case PlaceholderType::VARIADIC: return "...";
        case PlaceholderType::NON_PLACEHOLDER: return "";
        default: return "";
    }
}

void AtomArg::bind(std::function<std::string(const std::string&)> func) {
    bindingFunction = func;
}

void AtomArg::bind(const std::string& value) {
    bindingFunction = [value](const std::string&) { return value; };
}

void AtomArg::unbind() {
    bindingFunction = nullptr;
}

bool AtomArg::hasBinding() const {
    return bindingFunction != nullptr;
}

std::string AtomArg::applyBinding(const std::string& input) const {
    if (bindingFunction) {
        return bindingFunction(input);
    }
    return input;
}

void AtomArg::fillValue(const std::string& value) {
    this->value = value;
}

void AtomArg::fillValue(int value) {
    this->value = std::to_string(value);
}

void AtomArg::fillValue(double value) {
    this->value = std::to_string(value);
}

void AtomArg::fillValue(bool value) {
    this->value = value ? "true" : "false";
}

void AtomArg::clearValue() {
    value.clear();
}

int AtomArg::getMatchCount() const {
    return matchCount;
}

void AtomArg::setMatchCount(int count) {
    matchCount = count;
}

void AtomArg::incrementMatchCount() {
    matchCount++;
}

void AtomArg::resetMatchCount() {
    matchCount = 0;
}

bool AtomArg::validate() const {
    // 基本验证
    if (token.empty()) {
        return false;
    }
    
    // 占位符验证
    if (isPlaceholder) {
        // 检查占位符类型是否有效
        if (type == PlaceholderType::NON_PLACEHOLDER) {
            return false;
        }
        
        // 检查占位符格式
        std::string expectedPrefix = getPlaceholderPrefix(type);
        if (!expectedPrefix.empty() && token != expectedPrefix) {
            return false;
        }
    }
    
    return true;
}

bool AtomArg::hasValue() const {
    return !value.empty();
}

bool AtomArg::isEmpty() const {
    return token.empty() && value.empty();
}

std::string AtomArg::toString() const {
    if (isPlaceholder) {
        return token + "(" + value + ")";
    } else {
        return token;
    }
}

std::string AtomArg::toFormattedString() const {
    if (isPlaceholder) {
        std::ostringstream oss;
        oss << token << "[" << placeholderTypeToString(type) << "]";
        if (!value.empty()) {
            oss << "=" << value;
        }
        return oss.str();
    } else {
        return token;
    }
}

std::string AtomArg::toDebugString() const {
    std::ostringstream oss;
    oss << "AtomArg{";
    oss << "token='" << token << "', ";
    oss << "value='" << value << "', ";
    oss << "type=" << placeholderTypeToString(type) << ", ";
    oss << "isPlaceholder=" << (isPlaceholder ? "true" : "false") << ", ";
    oss << "matchCount=" << matchCount << ", ";
    oss << "hasBinding=" << (hasBinding() ? "true" : "false");
    oss << "}";
    return oss.str();
}

bool AtomArg::operator==(const AtomArg& other) const {
    return token == other.token && 
           value == other.value && 
           type == other.type && 
           isPlaceholder == other.isPlaceholder;
}

bool AtomArg::operator!=(const AtomArg& other) const {
    return !(*this == other);
}

bool AtomArg::operator<(const AtomArg& other) const {
    if (token != other.token) {
        return token < other.token;
    }
    if (value != other.value) {
        return value < other.value;
    }
    return static_cast<int>(type) < static_cast<int>(other.type);
}

AtomArg::AtomArg(const AtomArg& other) 
    : token(other.token), value(other.value), type(other.type), 
      isPlaceholder(other.isPlaceholder), bindingFunction(other.bindingFunction), 
      matchCount(other.matchCount) {}

AtomArg& AtomArg::operator=(const AtomArg& other) {
    if (this != &other) {
        token = other.token;
        value = other.value;
        type = other.type;
        isPlaceholder = other.isPlaceholder;
        bindingFunction = other.bindingFunction;
        matchCount = other.matchCount;
    }
    return *this;
}

AtomArg::AtomArg(AtomArg&& other) noexcept 
    : token(std::move(other.token)), value(std::move(other.value)), type(other.type), 
      isPlaceholder(other.isPlaceholder), bindingFunction(std::move(other.bindingFunction)), 
      matchCount(other.matchCount) {
    other.type = PlaceholderType::NON_PLACEHOLDER;
    other.isPlaceholder = false;
    other.matchCount = 0;
}

AtomArg& AtomArg::operator=(AtomArg&& other) noexcept {
    if (this != &other) {
        token = std::move(other.token);
        value = std::move(other.value);
        type = other.type;
        isPlaceholder = other.isPlaceholder;
        bindingFunction = std::move(other.bindingFunction);
        matchCount = other.matchCount;
        
        other.type = PlaceholderType::NON_PLACEHOLDER;
        other.isPlaceholder = false;
        other.matchCount = 0;
    }
    return *this;
}

void AtomArg::parseToken(const std::string& token) {
    this->token = token;
    
    if (isValidPlaceholderToken(token)) {
        type = parsePlaceholderType(token);
        isPlaceholder = true;
    } else if (isVariadicToken(token)) {
        type = PlaceholderType::VARIADIC;
        isPlaceholder = true;
    } else {
        type = PlaceholderType::NON_PLACEHOLDER;
        isPlaceholder = false;
    }
}

bool AtomArg::isValidPlaceholderToken(const std::string& token) const {
    return token == "$" || token == "$?" || token == "$!" || 
           token == "$_" || token == "$?_" || token == "$!_";
}

bool AtomArg::isVariadicToken(const std::string& token) const {
    return token == "...";
}

} // namespace CJMOD
} // namespace CHTL