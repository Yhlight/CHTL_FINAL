#include "CJMODArg.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>

namespace CHTL_JS {

// CJMODArg 实现
CJMODArg::CJMODArg() 
    : type(ArgType::UNKNOWN), required(false), optional(false), position(-1), index(-1) {
}

CJMODArg::CJMODArg(const std::string& name, const std::any& value) 
    : name(name), value(value), type(ArgType::UNKNOWN), required(false), optional(false), position(-1), index(-1) {
}

CJMODArg::CJMODArg(const std::string& name, const std::any& value, ArgType type) 
    : name(name), value(value), type(type), required(false), optional(false), position(-1), index(-1) {
}

CJMODArg::~CJMODArg() = default;

// 基本属性实现
void CJMODArg::setName(const std::string& name) {
    this->name = name;
}

std::string CJMODArg::getName() const {
    return name;
}

void CJMODArg::setValue(const std::any& value) {
    this->value = value;
}

std::any CJMODArg::getValue() const {
    return value;
}

void CJMODArg::setType(ArgType type) {
    this->type = type;
}

ArgType CJMODArg::getType() const {
    return type;
}

void CJMODArg::setRequired(bool required) {
    this->required = required;
    if (required) {
        this->optional = false;
    }
}

bool CJMODArg::isRequired() const {
    return required;
}

void CJMODArg::setOptional(bool optional) {
    this->optional = optional;
    if (optional) {
        this->required = false;
    }
}

bool CJMODArg::isOptional() const {
    return optional;
}

void CJMODArg::setDefault(const std::any& defaultValue) {
    this->defaultValue = defaultValue;
}

std::any CJMODArg::getDefault() const {
    return defaultValue;
}

bool CJMODArg::hasDefault() const {
    return defaultValue.has_value();
}

// 参数信息实现
void CJMODArg::setDescription(const std::string& description) {
    this->description = description;
}

std::string CJMODArg::getDescription() const {
    return description;
}

void CJMODArg::setTypeName(const std::string& typeName) {
    this->typeName = typeName;
}

std::string CJMODArg::getTypeName() const {
    return typeName;
}

void CJMODArg::setPosition(int position) {
    this->position = position;
}

int CJMODArg::getPosition() const {
    return position;
}

void CJMODArg::setIndex(int index) {
    this->index = index;
}

int CJMODArg::getIndex() const {
    return index;
}

// 验证实现
ArgValidationResult CJMODArg::validate() const {
    ArgValidationResult result;
    result.valid = validateArg();
    
    if (!result.valid) {
        result.errors.push_back("Argument validation failed");
    }
    
    return result;
}

ArgValidationResult CJMODArg::validate(const std::any& value) const {
    ArgValidationResult result;
    result.valid = validateValue(value);
    
    if (!result.valid) {
        result.errors.push_back("Value validation failed");
    }
    
    return result;
}

bool CJMODArg::isValid() const {
    return validateArg();
}

bool CJMODArg::isValid(const std::any& value) const {
    return validateValue(value);
}

// 绑定实现
std::any CJMODArg::bind(const std::any& value, const ArgBindOptions& options) const {
    return bindValue(value, options);
}

std::any CJMODArg::bind(const std::vector<std::any>& values, const ArgBindOptions& options) const {
    return bindArray(values, options);
}

std::any CJMODArg::bind(const std::map<std::string, std::any>& object, const ArgBindOptions& options) const {
    return bindObject(object, options);
}

// 转换实现
std::any CJMODArg::transform(const std::any& value, const ArgTransformOptions& options) const {
    return transformValue(value, options);
}

std::string CJMODArg::transformToString(const std::any& value, const ArgTransformOptions& options) const {
    return transformToStringValue(value, options);
}

std::vector<std::any> CJMODArg::transformToArray(const std::any& value, const ArgTransformOptions& options) const {
    return transformToArrayValue(value, options);
}

std::map<std::string, std::any> CJMODArg::transformToObject(const std::any& value, const ArgTransformOptions& options) const {
    return transformToObjectValue(value, options);
}

// 填充实现
std::any CJMODArg::fillValue(const std::any& value) const {
    return fillValueFromValue(value);
}

std::any CJMODArg::fillValue(const std::vector<std::any>& values) const {
    return fillValueFromArray(values);
}

std::any CJMODArg::fillValue(const std::map<std::string, std::any>& object) const {
    return fillValueFromObject(object);
}

// 检查实现
bool CJMODArg::checkType(const std::any& value) const {
    return checkTypeValue(value);
}

bool CJMODArg::checkValue(const std::any& value) const {
    return checkValueValue(value);
}

bool CJMODArg::checkRequired(const std::any& value) const {
    return checkRequiredValue(value);
}

bool CJMODArg::checkOptional(const std::any& value) const {
    return checkOptionalValue(value);
}

// 比较实现
bool CJMODArg::equals(const CJMODArg& other) const {
    return name == other.name &&
           type == other.type &&
           required == other.required &&
           optional == other.optional &&
           description == other.description &&
           typeName == other.typeName &&
           position == other.position &&
           index == other.index;
}

bool CJMODArg::equals(const std::any& value) const {
    return this->value.has_value() && value.has_value() && this->value.type() == value.type();
}

// 克隆实现
std::shared_ptr<CJMODArg> CJMODArg::clone() const {
    auto cloned = std::make_shared<CJMODArg>(name, value, type);
    cloned->required = required;
    cloned->optional = optional;
    cloned->defaultValue = defaultValue;
    cloned->description = description;
    cloned->typeName = typeName;
    cloned->position = position;
    cloned->index = index;
    return cloned;
}

std::shared_ptr<CJMODArg> CJMODArg::deepClone() const {
    return clone();
}

// 转换实现
std::string CJMODArg::toString() const {
    return name + ":" + getValueTypeName();
}

std::string CJMODArg::toDebugString() const {
    std::ostringstream oss;
    oss << "CJMODArg{name='" << name
        << "', type=" << static_cast<int>(type)
        << ", required=" << required
        << ", optional=" << optional
        << ", position=" << position
        << ", index=" << index << "}";
    return oss.str();
}

std::string CJMODArg::toJSON() const {
    return generateJSON();
}

std::string CJMODArg::toXML() const {
    return generateXML();
}

std::string CJMODArg::toYAML() const {
    return generateYAML();
}

// 格式化实现
std::string CJMODArg::format() const {
    return formatArg();
}

std::string CJMODArg::minify() const {
    return minifyArg();
}

std::string CJMODArg::beautify() const {
    return beautifyArg();
}

// 统计实现
size_t CJMODArg::getValueSize() const {
    return getValueSizeValue();
}

std::string CJMODArg::getValueType() const {
    return getValueTypeName();
}

bool CJMODArg::hasValue() const {
    return value.has_value();
}

// 重置实现
void CJMODArg::reset() {
    name.clear();
    value = std::any{};
    type = ArgType::UNKNOWN;
    required = false;
    optional = false;
    defaultValue = std::any{};
    description.clear();
    typeName.clear();
    position = -1;
    index = -1;
}

// 辅助方法实现
bool CJMODArg::validateArg() const {
    return !name.empty() && type != ArgType::UNKNOWN;
}

bool CJMODArg::validateValue(const std::any& value) const {
    return validateType(value) && validateRequired(value) && validateOptional(value);
}

bool CJMODArg::validateType(const std::any& value) const {
    return checkTypeValue(value);
}

bool CJMODArg::validateRequired(const std::any& value) const {
    return checkRequiredValue(value);
}

bool CJMODArg::validateOptional(const std::any& value) const {
    return checkOptionalValue(value);
}

std::any CJMODArg::bindValue(const std::any& value, const ArgBindOptions& options) const {
    if (options.validate && !validateValue(value)) {
        return std::any{};
    }
    
    if (options.transform) {
        return transformValue(value, ArgTransformOptions{});
    }
    
    return value;
}

std::any CJMODArg::bindArray(const std::vector<std::any>& values, const ArgBindOptions& options) const {
    if (values.empty()) {
        return hasDefault() ? defaultValue : std::any{};
    }
    
    if (position >= 0 && position < static_cast<int>(values.size())) {
        return bindValue(values[position], options);
    }
    
    return hasDefault() ? defaultValue : std::any{};
}

std::any CJMODArg::bindObject(const std::map<std::string, std::any>& object, const ArgBindOptions& options) const {
    auto it = object.find(name);
    if (it != object.end()) {
        return bindValue(it->second, options);
    }
    
    return hasDefault() ? defaultValue : std::any{};
}

std::any CJMODArg::transformValue(const std::any& value, const ArgTransformOptions& options) const {
    if (options.validateInput && !validateValue(value)) {
        return std::any{};
    }
    
    // 简化的转换实现
    return value;
}

std::string CJMODArg::transformToStringValue(const std::any& value, const ArgTransformOptions& options) const {
    if (options.validateInput && !validateValue(value)) {
        return "";
    }
    
    if (value.type() == typeid(std::string)) {
        return std::any_cast<std::string>(value);
    } else if (value.type() == typeid(int)) {
        return std::to_string(std::any_cast<int>(value));
    } else if (value.type() == typeid(double)) {
        return std::to_string(std::any_cast<double>(value));
    } else if (value.type() == typeid(float)) {
        return std::to_string(std::any_cast<float>(value));
    } else if (value.type() == typeid(bool)) {
        return std::any_cast<bool>(value) ? "true" : "false";
    } else {
        return "unknown";
    }
}

std::vector<std::any> CJMODArg::transformToArrayValue(const std::any& value, const ArgTransformOptions& options) const {
    if (options.validateInput && !validateValue(value)) {
        return {};
    }
    
    if (value.type() == typeid(std::vector<std::any>)) {
        return std::any_cast<std::vector<std::any>>(value);
    }
    
    return {value};
}

std::map<std::string, std::any> CJMODArg::transformToObjectValue(const std::any& value, const ArgTransformOptions& options) const {
    if (options.validateInput && !validateValue(value)) {
        return {};
    }
    
    if (value.type() == typeid(std::map<std::string, std::any>)) {
        return std::any_cast<std::map<std::string, std::any>>(value);
    }
    
    return {{name, value}};
}

std::any CJMODArg::fillValueFromValue(const std::any& value) const {
    if (value.has_value()) {
        return value;
    }
    
    return hasDefault() ? defaultValue : std::any{};
}

std::any CJMODArg::fillValueFromArray(const std::vector<std::any>& values) const {
    if (position >= 0 && position < static_cast<int>(values.size())) {
        return values[position];
    }
    
    return hasDefault() ? defaultValue : std::any{};
}

std::any CJMODArg::fillValueFromObject(const std::map<std::string, std::any>& object) const {
    auto it = object.find(name);
    if (it != object.end()) {
        return it->second;
    }
    
    return hasDefault() ? defaultValue : std::any{};
}

bool CJMODArg::checkTypeValue(const std::any& value) const {
    if (!value.has_value()) {
        return optional || hasDefault();
    }
    
    if (!typeName.empty()) {
        // 简化的类型检查
        return true;
    }
    
    return true;
}

bool CJMODArg::checkValueValue(const std::any& value) const {
    return value.has_value();
}

bool CJMODArg::checkRequiredValue(const std::any& value) const {
    if (required) {
        return value.has_value();
    }
    return true;
}

bool CJMODArg::checkOptionalValue(const std::any& value) const {
    if (optional) {
        return true; // 可选参数总是有效
    }
    return value.has_value();
}

std::string CJMODArg::generateJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"name\": \"" << name << "\",\n";
    oss << "  \"type\": " << static_cast<int>(type) << ",\n";
    oss << "  \"required\": " << (required ? "true" : "false") << ",\n";
    oss << "  \"optional\": " << (optional ? "true" : "false") << ",\n";
    oss << "  \"description\": \"" << description << "\",\n";
    oss << "  \"typeName\": \"" << typeName << "\",\n";
    oss << "  \"position\": " << position << ",\n";
    oss << "  \"index\": " << index << "\n";
    oss << "}";
    return oss.str();
}

std::string CJMODArg::generateXML() const {
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    oss << "<arg>\n";
    oss << "  <name>" << name << "</name>\n";
    oss << "  <type>" << static_cast<int>(type) << "</type>\n";
    oss << "  <required>" << (required ? "true" : "false") << "</required>\n";
    oss << "  <optional>" << (optional ? "true" : "false") << "</optional>\n";
    oss << "  <description>" << description << "</description>\n";
    oss << "  <typeName>" << typeName << "</typeName>\n";
    oss << "  <position>" << position << "</position>\n";
    oss << "  <index>" << index << "</index>\n";
    oss << "</arg>";
    return oss.str();
}

std::string CJMODArg::generateYAML() const {
    std::ostringstream oss;
    oss << "name: " << name << "\n";
    oss << "type: " << static_cast<int>(type) << "\n";
    oss << "required: " << (required ? "true" : "false") << "\n";
    oss << "optional: " << (optional ? "true" : "false") << "\n";
    oss << "description: " << description << "\n";
    oss << "typeName: " << typeName << "\n";
    oss << "position: " << position << "\n";
    oss << "index: " << index << "\n";
    return oss.str();
}

std::string CJMODArg::formatArg() const {
    return toString();
}

std::string CJMODArg::minifyArg() const {
    return toString();
}

std::string CJMODArg::beautifyArg() const {
    return toString();
}

std::string CJMODArg::getValueTypeName() const {
    if (!value.has_value()) {
        return "undefined";
    }
    
    if (value.type() == typeid(std::string)) {
        return "string";
    } else if (value.type() == typeid(int)) {
        return "number";
    } else if (value.type() == typeid(double)) {
        return "number";
    } else if (value.type() == typeid(float)) {
        return "number";
    } else if (value.type() == typeid(bool)) {
        return "boolean";
    } else if (value.type() == typeid(std::vector<std::any>)) {
        return "array";
    } else if (value.type() == typeid(std::map<std::string, std::any>)) {
        return "object";
    } else {
        return "unknown";
    }
}

size_t CJMODArg::getValueSizeValue() const {
    if (!value.has_value()) {
        return 0;
    }
    
    if (value.type() == typeid(std::string)) {
        return std::any_cast<std::string>(value).length();
    } else if (value.type() == typeid(std::vector<std::any>)) {
        return std::any_cast<std::vector<std::any>>(value).size();
    } else if (value.type() == typeid(std::map<std::string, std::any>)) {
        return std::any_cast<std::map<std::string, std::any>>(value).size();
    } else {
        return 1;
    }
}

} // namespace CHTL_JS