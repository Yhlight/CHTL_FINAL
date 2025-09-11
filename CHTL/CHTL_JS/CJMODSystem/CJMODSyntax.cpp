#include "CJMODSyntax.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>

namespace CHTL_JS {

// CJMODSyntax 实现
CJMODSyntax::CJMODSyntax() {
}

CJMODSyntax::~CJMODSyntax() = default;

// 基本分析实现
SyntaxAnalysisResult CJMODSyntax::analyze(const std::any& value) const {
    SyntaxAnalysisResult result;
    
    if (value.has_value()) {
        result.type = inferTypeFromValue(value);
        result.isObject = isObject(value);
        result.isFunction = isFunction(value);
        result.isArray = isArray(value);
        result.isCHTLJSFunction = isCHTLJSFunction(value);
        
        switch (result.type) {
            case SyntaxType::OBJECT:
                result.description = "Object";
                break;
            case SyntaxType::FUNCTION:
                result.description = "Function";
                break;
            case SyntaxType::ARRAY:
                result.description = "Array";
                break;
            case SyntaxType::STRING:
                result.description = "String";
                break;
            case SyntaxType::NUMBER:
                result.description = "Number";
                break;
            case SyntaxType::BOOLEAN:
                result.description = "Boolean";
                break;
            case SyntaxType::NULL_VALUE:
                result.description = "Null";
                break;
            case SyntaxType::UNDEFINED:
                result.description = "Undefined";
                break;
            case SyntaxType::SYMBOL:
                result.description = "Symbol";
                break;
            case SyntaxType::REGEXP:
                result.description = "RegExp";
                break;
            case SyntaxType::DATE:
                result.description = "Date";
                break;
            case SyntaxType::ERROR:
                result.description = "Error";
                break;
            case SyntaxType::PROMISE:
                result.description = "Promise";
                break;
            case SyntaxType::GENERATOR:
                result.description = "Generator";
                break;
            case SyntaxType::ASYNC_FUNCTION:
                result.description = "AsyncFunction";
                break;
            case SyntaxType::CLASS:
                result.description = "Class";
                break;
            case SyntaxType::MODULE:
                result.description = "Module";
                break;
            case SyntaxType::CHTL_JS_FUNCTION:
                result.description = "CHTLJSFunction";
                break;
            default:
                result.description = "Unknown";
                break;
        }
    } else {
        result.type = SyntaxType::UNDEFINED;
        result.description = "Undefined";
    }
    
    return result;
}

SyntaxAnalysisResult CJMODSyntax::analyze(const std::string& code) const {
    SyntaxAnalysisResult result;
    result.type = inferTypeFromCode(code);
    result.isObject = result.type == SyntaxType::OBJECT;
    result.isFunction = result.type == SyntaxType::FUNCTION;
    result.isArray = result.type == SyntaxType::ARRAY;
    result.isCHTLJSFunction = result.type == SyntaxType::CHTL_JS_FUNCTION;
    
    switch (result.type) {
        case SyntaxType::OBJECT:
            result.description = "Object";
            break;
        case SyntaxType::FUNCTION:
            result.description = "Function";
            break;
        case SyntaxType::ARRAY:
            result.description = "Array";
            break;
        case SyntaxType::STRING:
            result.description = "String";
            break;
        case SyntaxType::NUMBER:
            result.description = "Number";
            break;
        case SyntaxType::BOOLEAN:
            result.description = "Boolean";
            break;
        case SyntaxType::NULL_VALUE:
            result.description = "Null";
            break;
        case SyntaxType::UNDEFINED:
            result.description = "Undefined";
            break;
        case SyntaxType::SYMBOL:
            result.description = "Symbol";
            break;
        case SyntaxType::REGEXP:
            result.description = "RegExp";
            break;
        case SyntaxType::DATE:
            result.description = "Date";
            break;
        case SyntaxType::ERROR:
            result.description = "Error";
            break;
        case SyntaxType::PROMISE:
            result.description = "Promise";
            break;
        case SyntaxType::GENERATOR:
            result.description = "Generator";
            break;
        case SyntaxType::ASYNC_FUNCTION:
            result.description = "AsyncFunction";
            break;
        case SyntaxType::CLASS:
            result.description = "Class";
            break;
        case SyntaxType::MODULE:
            result.description = "Module";
            break;
        case SyntaxType::CHTL_JS_FUNCTION:
            result.description = "CHTLJSFunction";
            break;
        default:
            result.description = "Unknown";
            break;
    }
    
    return result;
}

SyntaxAnalysisResult CJMODSyntax::analyze(const std::vector<std::any>& array) const {
    SyntaxAnalysisResult result;
    result.type = SyntaxType::ARRAY;
    result.isObject = false;
    result.isFunction = false;
    result.isArray = true;
    result.isCHTLJSFunction = false;
    result.description = "Array";
    
    return result;
}

SyntaxAnalysisResult CJMODSyntax::analyze(const std::map<std::string, std::any>& object) const {
    SyntaxAnalysisResult result;
    result.type = SyntaxType::OBJECT;
    result.isObject = true;
    result.isFunction = false;
    result.isArray = false;
    result.isCHTLJSFunction = false;
    result.description = "Object";
    
    return result;
}

// 类型检查实现
bool CJMODSyntax::isObject(const std::any& value) const {
    return inferTypeFromValue(value) == SyntaxType::OBJECT;
}

bool CJMODSyntax::isFunction(const std::any& value) const {
    return inferTypeFromValue(value) == SyntaxType::FUNCTION;
}

bool CJMODSyntax::isArray(const std::any& value) const {
    return inferTypeFromValue(value) == SyntaxType::ARRAY;
}

bool CJMODSyntax::isString(const std::any& value) const {
    return inferTypeFromValue(value) == SyntaxType::STRING;
}

bool CJMODSyntax::isNumber(const std::any& value) const {
    return inferTypeFromValue(value) == SyntaxType::NUMBER;
}

bool CJMODSyntax::isBoolean(const std::any& value) const {
    return inferTypeFromValue(value) == SyntaxType::BOOLEAN;
}

bool CJMODSyntax::isNull(const std::any& value) const {
    return inferTypeFromValue(value) == SyntaxType::NULL_VALUE;
}

bool CJMODSyntax::isUndefined(const std::any& value) const {
    return inferTypeFromValue(value) == SyntaxType::UNDEFINED;
}

bool CJMODSyntax::isSymbol(const std::any& value) const {
    return inferTypeFromValue(value) == SyntaxType::SYMBOL;
}

bool CJMODSyntax::isRegExp(const std::any& value) const {
    return inferTypeFromValue(value) == SyntaxType::REGEXP;
}

bool CJMODSyntax::isDate(const std::any& value) const {
    return inferTypeFromValue(value) == SyntaxType::DATE;
}

bool CJMODSyntax::isError(const std::any& value) const {
    return inferTypeFromValue(value) == SyntaxType::ERROR;
}

bool CJMODSyntax::isPromise(const std::any& value) const {
    return inferTypeFromValue(value) == SyntaxType::PROMISE;
}

bool CJMODSyntax::isGenerator(const std::any& value) const {
    return inferTypeFromValue(value) == SyntaxType::GENERATOR;
}

bool CJMODSyntax::isAsyncFunction(const std::any& value) const {
    return inferTypeFromValue(value) == SyntaxType::ASYNC_FUNCTION;
}

bool CJMODSyntax::isClass(const std::any& value) const {
    return inferTypeFromValue(value) == SyntaxType::CLASS;
}

bool CJMODSyntax::isModule(const std::any& value) const {
    return inferTypeFromValue(value) == SyntaxType::MODULE;
}

bool CJMODSyntax::isCHTLJSFunction(const std::any& value) const {
    return inferTypeFromValue(value) == SyntaxType::CHTL_JS_FUNCTION;
}

// 类型推断实现
SyntaxType CJMODSyntax::inferType(const std::any& value) const {
    return inferTypeFromValue(value);
}

SyntaxType CJMODSyntax::inferType(const std::string& code) const {
    return inferTypeFromCode(code);
}

SyntaxType CJMODSyntax::inferType(const std::vector<std::any>& array) const {
    return inferTypeFromArray(array);
}

SyntaxType CJMODSyntax::inferType(const std::map<std::string, std::any>& object) const {
    return inferTypeFromObject(object);
}

// 验证实现
SyntaxValidationResult CJMODSyntax::validate(const std::any& value) const {
    SyntaxValidationResult result;
    result.valid = validateValue(value);
    
    if (!result.valid) {
        result.errors.push_back("Invalid value");
    }
    
    return result;
}

SyntaxValidationResult CJMODSyntax::validate(const std::string& code) const {
    SyntaxValidationResult result;
    result.valid = !code.empty();
    
    if (!result.valid) {
        result.errors.push_back("Empty code");
    }
    
    return result;
}

SyntaxValidationResult CJMODSyntax::validate(const std::vector<std::any>& array) const {
    SyntaxValidationResult result;
    result.valid = validateArray(array);
    
    if (!result.valid) {
        result.errors.push_back("Invalid array");
    }
    
    return result;
}

SyntaxValidationResult CJMODSyntax::validate(const std::map<std::string, std::any>& object) const {
    SyntaxValidationResult result;
    result.valid = validateObject(object);
    
    if (!result.valid) {
        result.errors.push_back("Invalid object");
    }
    
    return result;
}

// 转换实现
std::any CJMODSyntax::transform(const std::any& value, const SyntaxTransformOptions& options) const {
    if (options.validateInput && !validateValue(value)) {
        return std::any{};
    }
    
    // 简化的转换实现
    return value;
}

std::string CJMODSyntax::transformToString(const std::any& value, const SyntaxTransformOptions& options) const {
    if (options.validateInput && !validateValue(value)) {
        return "";
    }
    
    return serializeValue(value);
}

std::vector<std::any> CJMODSyntax::transformToArray(const std::any& value, const SyntaxTransformOptions& options) const {
    if (options.validateInput && !validateValue(value)) {
        return {};
    }
    
    if (isArray(value)) {
        return std::any_cast<std::vector<std::any>>(value);
    }
    
    return {value};
}

std::map<std::string, std::any> CJMODSyntax::transformToObject(const std::any& value, const SyntaxTransformOptions& options) const {
    if (options.validateInput && !validateValue(value)) {
        return {};
    }
    
    if (isObject(value)) {
        return std::any_cast<std::map<std::string, std::any>>(value);
    }
    
    return {{"value", value}};
}

// 序列化实现
std::string CJMODSyntax::serialize(const std::any& value) const {
    return serializeValue(value);
}

std::string CJMODSyntax::serialize(const std::vector<std::any>& array) const {
    return serializeArray(array);
}

std::string CJMODSyntax::serialize(const std::map<std::string, std::any>& object) const {
    return serializeObject(object);
}

// 反序列化实现
std::any CJMODSyntax::deserialize(const std::string& data) const {
    return deserializeValue(data);
}

std::vector<std::any> CJMODSyntax::deserializeArray(const std::string& data) const {
    return deserializeArrayValue(data);
}

std::map<std::string, std::any> CJMODSyntax::deserializeObject(const std::string& data) const {
    return deserializeObjectValue(data);
}

// 比较实现
bool CJMODSyntax::equals(const std::any& value1, const std::any& value2) const {
    return compareValues(value1, value2);
}

bool CJMODSyntax::equals(const std::vector<std::any>& array1, const std::vector<std::any>& array2) const {
    return compareArrays(array1, array2);
}

bool CJMODSyntax::equals(const std::map<std::string, std::any>& object1, const std::map<std::string, std::any>& object2) const {
    return compareObjects(object1, object2);
}

// 克隆实现
std::any CJMODSyntax::clone(const std::any& value) const {
    return cloneValue(value);
}

std::vector<std::any> CJMODSyntax::cloneArray(const std::vector<std::any>& array) const {
    return cloneArrayValue(array);
}

std::map<std::string, std::any> CJMODSyntax::cloneObject(const std::map<std::string, std::any>& object) const {
    return cloneObjectValue(object);
}

// 深度克隆实现
std::any CJMODSyntax::deepClone(const std::any& value) const {
    return deepCloneValue(value);
}

std::vector<std::any> CJMODSyntax::deepCloneArray(const std::vector<std::any>& array) const {
    return deepCloneArrayValue(array);
}

std::map<std::string, std::any> CJMODSyntax::deepCloneObject(const std::map<std::string, std::any>& object) const {
    return deepCloneObjectValue(object);
}

// 合并实现
std::any CJMODSyntax::merge(const std::any& value1, const std::any& value2) const {
    return mergeValues(value1, value2);
}

std::vector<std::any> CJMODSyntax::mergeArrays(const std::vector<std::any>& array1, const std::vector<std::any>& array2) const {
    return mergeArrayValues(array1, array2);
}

std::map<std::string, std::any> CJMODSyntax::mergeObjects(const std::map<std::string, std::any>& object1, const std::map<std::string, std::any>& object2) const {
    return mergeObjectValues(object1, object2);
}

// 过滤实现
std::vector<std::any> CJMODSyntax::filterArray(const std::vector<std::any>& array, std::function<bool(const std::any&)> predicate) const {
    std::vector<std::any> result;
    std::copy_if(array.begin(), array.end(), std::back_inserter(result), predicate);
    return result;
}

std::map<std::string, std::any> CJMODSyntax::filterObject(const std::map<std::string, std::any>& object, std::function<bool(const std::string&, const std::any&)> predicate) const {
    std::map<std::string, std::any> result;
    for (const auto& pair : object) {
        if (predicate(pair.first, pair.second)) {
            result[pair.first] = pair.second;
        }
    }
    return result;
}

// 映射实现
std::vector<std::any> CJMODSyntax::mapArray(const std::vector<std::any>& array, std::function<std::any(const std::any&)> mapper) const {
    std::vector<std::any> result;
    std::transform(array.begin(), array.end(), std::back_inserter(result), mapper);
    return result;
}

std::map<std::string, std::any> CJMODSyntax::mapObject(const std::map<std::string, std::any>& object, std::function<std::any(const std::string&, const std::any&)> mapper) const {
    std::map<std::string, std::any> result;
    for (const auto& pair : object) {
        result[pair.first] = mapper(pair.first, pair.second);
    }
    return result;
}

// 归约实现
std::any CJMODSyntax::reduceArray(const std::vector<std::any>& array, std::function<std::any(const std::any&, const std::any&)> reducer, const std::any& initialValue) const {
    std::any result = initialValue;
    for (const auto& value : array) {
        result = reducer(result, value);
    }
    return result;
}

std::any CJMODSyntax::reduceObject(const std::map<std::string, std::any>& object, std::function<std::any(const std::any&, const std::string&, const std::any&)> reducer, const std::any& initialValue) const {
    std::any result = initialValue;
    for (const auto& pair : object) {
        result = reducer(result, pair.first, pair.second);
    }
    return result;
}

// 查找实现
std::any CJMODSyntax::findArray(const std::vector<std::any>& array, std::function<bool(const std::any&)> predicate) const {
    auto it = std::find_if(array.begin(), array.end(), predicate);
    return it != array.end() ? *it : std::any{};
}

std::pair<std::string, std::any> CJMODSyntax::findObject(const std::map<std::string, std::any>& object, std::function<bool(const std::string&, const std::any&)> predicate) const {
    for (const auto& pair : object) {
        if (predicate(pair.first, pair.second)) {
            return pair;
        }
    }
    return {"", std::any{}};
}

// 排序实现
std::vector<std::any> CJMODSyntax::sortArray(const std::vector<std::any>& array, std::function<bool(const std::any&, const std::any&)> comparator) const {
    std::vector<std::any> result = array;
    if (comparator) {
        std::sort(result.begin(), result.end(), comparator);
    }
    return result;
}

// 统计实现
size_t CJMODSyntax::getArrayLength(const std::vector<std::any>& array) const {
    return array.size();
}

size_t CJMODSyntax::getObjectSize(const std::map<std::string, std::any>& object) const {
    return object.size();
}

std::vector<std::string> CJMODSyntax::getObjectKeys(const std::map<std::string, std::any>& object) const {
    std::vector<std::string> keys;
    for (const auto& pair : object) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::vector<std::any> CJMODSyntax::getObjectValues(const std::map<std::string, std::any>& object) const {
    std::vector<std::any> values;
    for (const auto& pair : object) {
        values.push_back(pair.second);
    }
    return values;
}

// 验证实现
bool CJMODSyntax::isValid() const {
    return true;
}

std::vector<std::string> CJMODSyntax::validate() const {
    std::vector<std::string> errors;
    
    if (!isValid()) {
        errors.push_back("Syntax analyzer validation failed");
    }
    
    return errors;
}

// 转换实现
std::string CJMODSyntax::toString() const {
    return "CJMODSyntax";
}

std::string CJMODSyntax::toDebugString() const {
    return generateDebugString();
}

// 辅助方法实现
bool CJMODSyntax::validateValue(const std::any& value) const {
    return value.has_value();
}

bool CJMODSyntax::validateArray(const std::vector<std::any>& array) const {
    return true; // 简化的数组验证
}

bool CJMODSyntax::validateObject(const std::map<std::string, std::any>& object) const {
    return true; // 简化的对象验证
}

SyntaxType CJMODSyntax::inferTypeFromValue(const std::any& value) const {
    if (!value.has_value()) {
        return SyntaxType::UNDEFINED;
    }
    
    if (value.type() == typeid(std::string)) {
        return SyntaxType::STRING;
    } else if (value.type() == typeid(int) || value.type() == typeid(double) || value.type() == typeid(float)) {
        return SyntaxType::NUMBER;
    } else if (value.type() == typeid(bool)) {
        return SyntaxType::BOOLEAN;
    } else if (value.type() == typeid(std::vector<std::any>)) {
        return SyntaxType::ARRAY;
    } else if (value.type() == typeid(std::map<std::string, std::any>)) {
        return SyntaxType::OBJECT;
    } else if (value.type() == typeid(std::function<void()>)) {
        return SyntaxType::FUNCTION;
    } else {
        return SyntaxType::UNKNOWN;
    }
}

SyntaxType CJMODSyntax::inferTypeFromCode(const std::string& code) const {
    // 简化的代码类型推断
    if (code.empty()) {
        return SyntaxType::UNDEFINED;
    }
    
    if (code[0] == '{' && code.back() == '}') {
        return SyntaxType::OBJECT;
    } else if (code[0] == '[' && code.back() == ']') {
        return SyntaxType::ARRAY;
    } else if (code[0] == '"' && code.back() == '"') {
        return SyntaxType::STRING;
    } else if (code == "true" || code == "false") {
        return SyntaxType::BOOLEAN;
    } else if (code == "null") {
        return SyntaxType::NULL_VALUE;
    } else if (code == "undefined") {
        return SyntaxType::UNDEFINED;
    } else if (code.find("function") != std::string::npos) {
        return SyntaxType::FUNCTION;
    } else if (code.find("class") != std::string::npos) {
        return SyntaxType::CLASS;
    } else if (code.find("async") != std::string::npos) {
        return SyntaxType::ASYNC_FUNCTION;
    } else if (code.find("CHTLJS") != std::string::npos) {
        return SyntaxType::CHTL_JS_FUNCTION;
    } else {
        return SyntaxType::UNKNOWN;
    }
}

SyntaxType CJMODSyntax::inferTypeFromArray(const std::vector<std::any>& array) const {
    return SyntaxType::ARRAY;
}

SyntaxType CJMODSyntax::inferTypeFromObject(const std::map<std::string, std::any>& object) const {
    return SyntaxType::OBJECT;
}

std::string CJMODSyntax::serializeValue(const std::any& value) const {
    if (!value.has_value()) {
        return "undefined";
    }
    
    if (value.type() == typeid(std::string)) {
        return "\"" + std::any_cast<std::string>(value) + "\"";
    } else if (value.type() == typeid(int)) {
        return std::to_string(std::any_cast<int>(value));
    } else if (value.type() == typeid(double)) {
        return std::to_string(std::any_cast<double>(value));
    } else if (value.type() == typeid(float)) {
        return std::to_string(std::any_cast<float>(value));
    } else if (value.type() == typeid(bool)) {
        return std::any_cast<bool>(value) ? "true" : "false";
    } else if (value.type() == typeid(std::vector<std::any>)) {
        return serializeArray(std::any_cast<std::vector<std::any>>(value));
    } else if (value.type() == typeid(std::map<std::string, std::any>)) {
        return serializeObject(std::any_cast<std::map<std::string, std::any>>(value));
    } else {
        return "unknown";
    }
}

std::string CJMODSyntax::serializeArray(const std::vector<std::any>& array) const {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < array.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << serializeValue(array[i]);
    }
    oss << "]";
    return oss.str();
}

std::string CJMODSyntax::serializeObject(const std::map<std::string, std::any>& object) const {
    std::ostringstream oss;
    oss << "{";
    bool first = true;
    for (const auto& pair : object) {
        if (!first) oss << ", ";
        oss << "\"" << pair.first << "\": " << serializeValue(pair.second);
        first = false;
    }
    oss << "}";
    return oss.str();
}

std::any CJMODSyntax::deserializeValue(const std::string& data) const {
    // 简化的反序列化实现
    if (data == "undefined") {
        return std::any{};
    } else if (data == "null") {
        return std::any{};
    } else if (data == "true") {
        return std::any{true};
    } else if (data == "false") {
        return std::any{false};
    } else if (data[0] == '"' && data.back() == '"') {
        return std::any{data.substr(1, data.length() - 2)};
    } else if (data[0] == '[' && data.back() == ']') {
        return std::any{deserializeArrayValue(data)};
    } else if (data[0] == '{' && data.back() == '}') {
        return std::any{deserializeObjectValue(data)};
    } else {
        // 尝试解析数字
        try {
            if (data.find('.') != std::string::npos) {
                return std::any{std::stod(data)};
            } else {
                return std::any{std::stoi(data)};
            }
        } catch (...) {
            return std::any{};
        }
    }
}

std::vector<std::any> CJMODSyntax::deserializeArrayValue(const std::string& data) const {
    // 简化的数组反序列化实现
    return {};
}

std::map<std::string, std::any> CJMODSyntax::deserializeObjectValue(const std::string& data) const {
    // 简化的对象反序列化实现
    return {};
}

bool CJMODSyntax::compareValues(const std::any& value1, const std::any& value2) const {
    if (!value1.has_value() && !value2.has_value()) {
        return true;
    }
    
    if (!value1.has_value() || !value2.has_value()) {
        return false;
    }
    
    if (value1.type() != value2.type()) {
        return false;
    }
    
    if (value1.type() == typeid(std::string)) {
        return std::any_cast<std::string>(value1) == std::any_cast<std::string>(value2);
    } else if (value1.type() == typeid(int)) {
        return std::any_cast<int>(value1) == std::any_cast<int>(value2);
    } else if (value1.type() == typeid(double)) {
        return std::any_cast<double>(value1) == std::any_cast<double>(value2);
    } else if (value1.type() == typeid(float)) {
        return std::any_cast<float>(value1) == std::any_cast<float>(value2);
    } else if (value1.type() == typeid(bool)) {
        return std::any_cast<bool>(value1) == std::any_cast<bool>(value2);
    } else if (value1.type() == typeid(std::vector<std::any>)) {
        return compareArrays(std::any_cast<std::vector<std::any>>(value1), std::any_cast<std::vector<std::any>>(value2));
    } else if (value1.type() == typeid(std::map<std::string, std::any>)) {
        return compareObjects(std::any_cast<std::map<std::string, std::any>>(value1), std::any_cast<std::map<std::string, std::any>>(value2));
    } else {
        return false;
    }
}

bool CJMODSyntax::compareArrays(const std::vector<std::any>& array1, const std::vector<std::any>& array2) const {
    if (array1.size() != array2.size()) {
        return false;
    }
    
    for (size_t i = 0; i < array1.size(); ++i) {
        if (!compareValues(array1[i], array2[i])) {
            return false;
        }
    }
    
    return true;
}

bool CJMODSyntax::compareObjects(const std::map<std::string, std::any>& object1, const std::map<std::string, std::any>& object2) const {
    if (object1.size() != object2.size()) {
        return false;
    }
    
    for (const auto& pair : object1) {
        auto it = object2.find(pair.first);
        if (it == object2.end() || !compareValues(pair.second, it->second)) {
            return false;
        }
    }
    
    return true;
}

std::any CJMODSyntax::cloneValue(const std::any& value) const {
    return value;
}

std::vector<std::any> CJMODSyntax::cloneArrayValue(const std::vector<std::any>& array) const {
    return array;
}

std::map<std::string, std::any> CJMODSyntax::cloneObjectValue(const std::map<std::string, std::any>& object) const {
    return object;
}

std::any CJMODSyntax::deepCloneValue(const std::any& value) const {
    return value;
}

std::vector<std::any> CJMODSyntax::deepCloneArrayValue(const std::vector<std::any>& array) const {
    return array;
}

std::map<std::string, std::any> CJMODSyntax::deepCloneObjectValue(const std::map<std::string, std::any>& object) const {
    return object;
}

std::any CJMODSyntax::mergeValues(const std::any& value1, const std::any& value2) const {
    return value2;
}

std::vector<std::any> CJMODSyntax::mergeArrayValues(const std::vector<std::any>& array1, const std::vector<std::any>& array2) const {
    std::vector<std::any> result = array1;
    result.insert(result.end(), array2.begin(), array2.end());
    return result;
}

std::map<std::string, std::any> CJMODSyntax::mergeObjectValues(const std::map<std::string, std::any>& object1, const std::map<std::string, std::any>& object2) const {
    std::map<std::string, std::any> result = object1;
    result.insert(object2.begin(), object2.end());
    return result;
}

std::string CJMODSyntax::generateDebugString() const {
    std::ostringstream oss;
    oss << "CJMODSyntax{valid=" << isValid() << "}";
    return oss.str();
}

} // namespace CHTL_JS