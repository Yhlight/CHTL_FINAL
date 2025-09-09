#include "CJMOD/Arg.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CJMOD {

// AtomArg 实现
AtomArg::AtomArg(const std::string& value)
    : m_value(value)
    , m_isPlaceholder(false)
    , m_isOptional(false)
    , m_isRequired(false)
    , m_isUnordered(false)
    , m_isVariadic(false)
{
    parsePlaceholderType();
}

void AtomArg::bind(std::function<std::string(const std::string&)> func) {
    m_bindFunction = func;
}

void AtomArg::fillValue(const std::string& value) {
    m_value = value;
}

void AtomArg::fillValue(int value) {
    m_value = std::to_string(value);
}

bool AtomArg::isPlaceholder() const {
    return m_isPlaceholder;
}

bool AtomArg::isOptional() const {
    return m_isOptional;
}

bool AtomArg::isRequired() const {
    return m_isRequired;
}

bool AtomArg::isUnordered() const {
    return m_isUnordered;
}

bool AtomArg::isVariadic() const {
    return m_isVariadic;
}

std::string AtomArg::execute() const {
    if (m_bindFunction) {
        return m_bindFunction(m_value);
    }
    return m_value;
}

void AtomArg::parsePlaceholderType() {
    if (m_value.empty()) {
        return;
    }
    
    if (m_value[0] == '$') {
        m_isPlaceholder = true;
        
        for (size_t i = 1; i < m_value.length(); i++) {
            switch (m_value[i]) {
                case '?':
                    m_isOptional = true;
                    break;
                case '!':
                    m_isRequired = true;
                    break;
                case '_':
                    m_isUnordered = true;
                    break;
                default:
                    break;
            }
        }
    } else if (m_value == "...") {
        m_isPlaceholder = true;
        m_isVariadic = true;
    }
}

// Arg 实现
Arg::Arg() = default;

Arg::Arg(const std::vector<std::string>& values) {
    for (const auto& value : values) {
        add(value);
    }
}

void Arg::add(const std::string& value) {
    m_args.emplace_back(value);
}

AtomArg& Arg::operator[](size_t index) {
    if (index >= m_args.size()) {
        throw std::out_of_range("Index out of range");
    }
    return m_args[index];
}

const AtomArg& Arg::operator[](size_t index) const {
    if (index >= m_args.size()) {
        throw std::out_of_range("Index out of range");
    }
    return m_args[index];
}

void Arg::bind(const std::string& placeholder, std::function<std::string(const std::string&)> func) {
    for (auto& arg : m_args) {
        if (arg.getValue() == placeholder) {
            arg.bind(func);
        }
    }
}

void Arg::fillValue(const std::vector<std::string>& values) {
    size_t valueIndex = 0;
    for (auto& arg : m_args) {
        if (arg.isPlaceholder() && valueIndex < values.size()) {
            arg.fillValue(values[valueIndex]);
            valueIndex++;
        }
    }
}

void Arg::fillValue(const Arg& other) {
    if (other.size() != m_args.size()) {
        return;
    }
    
    for (size_t i = 0; i < m_args.size(); i++) {
        if (m_args[i].isPlaceholder()) {
            m_args[i].fillValue(other[i].getValue());
        }
    }
}

void Arg::transform(const std::string& templateStr) {
    m_result = processTemplate(templateStr);
}

const std::string& Arg::getResult() const {
    return m_result;
}

void Arg::print() const {
    std::cout << "[";
    for (size_t i = 0; i < m_args.size(); i++) {
        if (i > 0) {
            std::cout << ", ";
        }
        std::cout << "\"" << m_args[i].getValue() << "\"";
    }
    std::cout << "]" << std::endl;
}

void Arg::clear() {
    m_args.clear();
    m_result.clear();
}

int Arg::findPlaceholder(const std::string& placeholder) const {
    for (size_t i = 0; i < m_args.size(); i++) {
        if (m_args[i].getValue() == placeholder) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void Arg::replacePlaceholder(const std::string& placeholder, const std::string& value) {
    for (auto& arg : m_args) {
        if (arg.getValue() == placeholder) {
            arg.fillValue(value);
        }
    }
}

bool Arg::containsPlaceholder(const std::string& placeholder) const {
    return findPlaceholder(placeholder) != -1;
}

std::vector<std::string> Arg::getPlaceholders() const {
    std::vector<std::string> placeholders;
    for (const auto& arg : m_args) {
        if (arg.isPlaceholder()) {
            placeholders.push_back(arg.getValue());
        }
    }
    return placeholders;
}

bool Arg::validate() const {
    // 检查所有必须的占位符是否都有值
    for (const auto& arg : m_args) {
        if (arg.isRequired() && arg.getValue().empty()) {
            return false;
        }
    }
    return true;
}

std::string Arg::processTemplate(const std::string& templateStr) {
    std::string result = templateStr;
    
    // 替换所有占位符
    for (size_t i = 0; i < m_args.size(); i++) {
        if (m_args[i].isPlaceholder()) {
            std::string placeholder = m_args[i].getValue();
            std::string value = m_args[i].execute();
            result = replaceAll(result, placeholder, value);
        }
    }
    
    return result;
}

std::string Arg::replaceAll(const std::string& str, const std::string& placeholder, const std::string& value) {
    std::string result = str;
    size_t pos = 0;
    
    while ((pos = result.find(placeholder, pos)) != std::string::npos) {
        result.replace(pos, placeholder.length(), value);
        pos += value.length();
    }
    
    return result;
}

} // namespace CJMOD