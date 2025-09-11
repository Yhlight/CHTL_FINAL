#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <any>
#include <functional>

namespace CHTL_JS {

// 语法类型枚举
enum class SyntaxType {
    UNKNOWN = 0,
    OBJECT,         // 对象
    FUNCTION,       // 函数
    ARRAY,          // 数组
    STRING,         // 字符串
    NUMBER,         // 数字
    BOOLEAN,        // 布尔值
    NULL_VALUE,     // null值
    UNDEFINED,      // undefined值
    SYMBOL,         // 符号
    REGEXP,         // 正则表达式
    DATE,           // 日期
    ERROR,          // 错误
    PROMISE,        // Promise
    GENERATOR,      // 生成器
    ASYNC_FUNCTION, // 异步函数
    CLASS,          // 类
    MODULE,         // 模块
    CHTL_JS_FUNCTION // CHTL JS函数
};

// 语法分析结果
struct SyntaxAnalysisResult {
    SyntaxType type;
    bool isObject;
    bool isFunction;
    bool isArray;
    bool isCHTLJSFunction;
    std::string description;
    std::map<std::string, std::any> metadata;
    
    SyntaxAnalysisResult() : type(SyntaxType::UNKNOWN), isObject(false), isFunction(false), 
                            isArray(false), isCHTLJSFunction(false) {}
};

// 语法验证结果
struct SyntaxValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, std::any> metadata;
    
    SyntaxValidationResult() : valid(false) {}
};

// 语法转换选项
struct SyntaxTransformOptions {
    bool preserveType = true;
    bool validateInput = true;
    bool validateOutput = true;
    bool strictMode = false;
    std::map<std::string, std::any> metadata;
};

// CJMOD语法分析器
class CJMODSyntax {
public:
    CJMODSyntax();
    ~CJMODSyntax();
    
    // 基本分析
    SyntaxAnalysisResult analyze(const std::any& value) const;
    SyntaxAnalysisResult analyze(const std::string& code) const;
    SyntaxAnalysisResult analyze(const std::vector<std::any>& array) const;
    SyntaxAnalysisResult analyze(const std::map<std::string, std::any>& object) const;
    
    // 类型检查
    bool isObject(const std::any& value) const;
    bool isFunction(const std::any& value) const;
    bool isArray(const std::any& value) const;
    bool isString(const std::any& value) const;
    bool isNumber(const std::any& value) const;
    bool isBoolean(const std::any& value) const;
    bool isNull(const std::any& value) const;
    bool isUndefined(const std::any& value) const;
    bool isSymbol(const std::any& value) const;
    bool isRegExp(const std::any& value) const;
    bool isDate(const std::any& value) const;
    bool isError(const std::any& value) const;
    bool isPromise(const std::any& value) const;
    bool isGenerator(const std::any& value) const;
    bool isAsyncFunction(const std::any& value) const;
    bool isClass(const std::any& value) const;
    bool isModule(const std::any& value) const;
    bool isCHTLJSFunction(const std::any& value) const;
    
    // 类型推断
    SyntaxType inferType(const std::any& value) const;
    SyntaxType inferType(const std::string& code) const;
    SyntaxType inferType(const std::vector<std::any>& array) const;
    SyntaxType inferType(const std::map<std::string, std::any>& object) const;
    
    // 验证
    SyntaxValidationResult validate(const std::any& value) const;
    SyntaxValidationResult validate(const std::string& code) const;
    SyntaxValidationResult validate(const std::vector<std::any>& array) const;
    SyntaxValidationResult validate(const std::map<std::string, std::any>& object) const;
    
    // 转换
    std::any transform(const std::any& value, const SyntaxTransformOptions& options = SyntaxTransformOptions{}) const;
    std::string transformToString(const std::any& value, const SyntaxTransformOptions& options = SyntaxTransformOptions{}) const;
    std::vector<std::any> transformToArray(const std::any& value, const SyntaxTransformOptions& options = SyntaxTransformOptions{}) const;
    std::map<std::string, std::any> transformToObject(const std::any& value, const SyntaxTransformOptions& options = SyntaxTransformOptions{}) const;
    
    // 序列化
    std::string serialize(const std::any& value) const;
    std::string serialize(const std::vector<std::any>& array) const;
    std::string serialize(const std::map<std::string, std::any>& object) const;
    
    // 反序列化
    std::any deserialize(const std::string& data) const;
    std::vector<std::any> deserializeArray(const std::string& data) const;
    std::map<std::string, std::any> deserializeObject(const std::string& data) const;
    
    // 比较
    bool equals(const std::any& value1, const std::any& value2) const;
    bool equals(const std::vector<std::any>& array1, const std::vector<std::any>& array2) const;
    bool equals(const std::map<std::string, std::any>& object1, const std::map<std::string, std::any>& object2) const;
    
    // 克隆
    std::any clone(const std::any& value) const;
    std::vector<std::any> cloneArray(const std::vector<std::any>& array) const;
    std::map<std::string, std::any> cloneObject(const std::map<std::string, std::any>& object) const;
    
    // 深度克隆
    std::any deepClone(const std::any& value) const;
    std::vector<std::any> deepCloneArray(const std::vector<std::any>& array) const;
    std::map<std::string, std::any> deepCloneObject(const std::map<std::string, std::any>& object) const;
    
    // 合并
    std::any merge(const std::any& value1, const std::any& value2) const;
    std::vector<std::any> mergeArrays(const std::vector<std::any>& array1, const std::vector<std::any>& array2) const;
    std::map<std::string, std::any> mergeObjects(const std::map<std::string, std::any>& object1, const std::map<std::string, std::any>& object2) const;
    
    // 过滤
    std::vector<std::any> filterArray(const std::vector<std::any>& array, std::function<bool(const std::any&)> predicate) const;
    std::map<std::string, std::any> filterObject(const std::map<std::string, std::any>& object, std::function<bool(const std::string&, const std::any&)> predicate) const;
    
    // 映射
    std::vector<std::any> mapArray(const std::vector<std::any>& array, std::function<std::any(const std::any&)> mapper) const;
    std::map<std::string, std::any> mapObject(const std::map<std::string, std::any>& object, std::function<std::any(const std::string&, const std::any&)> mapper) const;
    
    // 归约
    std::any reduceArray(const std::vector<std::any>& array, std::function<std::any(const std::any&, const std::any&)> reducer, const std::any& initialValue = std::any{}) const;
    std::any reduceObject(const std::map<std::string, std::any>& object, std::function<std::any(const std::any&, const std::string&, const std::any&)> reducer, const std::any& initialValue = std::any{}) const;
    
    // 查找
    std::any findArray(const std::vector<std::any>& array, std::function<bool(const std::any&)> predicate) const;
    std::pair<std::string, std::any> findObject(const std::map<std::string, std::any>& object, std::function<bool(const std::string&, const std::any&)> predicate) const;
    
    // 排序
    std::vector<std::any> sortArray(const std::vector<std::any>& array, std::function<bool(const std::any&, const std::any&)> comparator = nullptr) const;
    
    // 统计
    size_t getArrayLength(const std::vector<std::any>& array) const;
    size_t getObjectSize(const std::map<std::string, std::any>& object) const;
    std::vector<std::string> getObjectKeys(const std::map<std::string, std::any>& object) const;
    std::vector<std::any> getObjectValues(const std::map<std::string, std::any>& object) const;
    
    // 验证
    bool isValid() const;
    std::vector<std::string> validate() const;
    
    // 转换
    std::string toString() const;
    std::string toDebugString() const;
    
private:
    // 辅助方法
    bool validateValue(const std::any& value) const;
    bool validateArray(const std::vector<std::any>& array) const;
    bool validateObject(const std::map<std::string, std::any>& object) const;
    
    SyntaxType inferTypeFromValue(const std::any& value) const;
    SyntaxType inferTypeFromCode(const std::string& code) const;
    SyntaxType inferTypeFromArray(const std::vector<std::any>& array) const;
    SyntaxType inferTypeFromObject(const std::map<std::string, std::any>& object) const;
    
    std::string serializeValue(const std::any& value) const;
    std::string serializeArray(const std::vector<std::any>& array) const;
    std::string serializeObject(const std::map<std::string, std::any>& object) const;
    
    std::any deserializeValue(const std::string& data) const;
    std::vector<std::any> deserializeArrayValue(const std::string& data) const;
    std::map<std::string, std::any> deserializeObjectValue(const std::string& data) const;
    
    bool compareValues(const std::any& value1, const std::any& value2) const;
    bool compareArrays(const std::vector<std::any>& array1, const std::vector<std::any>& array2) const;
    bool compareObjects(const std::map<std::string, std::any>& object1, const std::map<std::string, std::any>& object2) const;
    
    std::any cloneValue(const std::any& value) const;
    std::vector<std::any> cloneArrayValue(const std::vector<std::any>& array) const;
    std::map<std::string, std::any> cloneObjectValue(const std::map<std::string, std::any>& object) const;
    
    std::any deepCloneValue(const std::any& value) const;
    std::vector<std::any> deepCloneArrayValue(const std::vector<std::any>& array) const;
    std::map<std::string, std::any> deepCloneObjectValue(const std::map<std::string, std::any>& object) const;
    
    std::any mergeValues(const std::any& value1, const std::any& value2) const;
    std::vector<std::any> mergeArrayValues(const std::vector<std::any>& array1, const std::vector<std::any>& array2) const;
    std::map<std::string, std::any> mergeObjectValues(const std::map<std::string, std::any>& object1, const std::map<std::string, std::any>& object2) const;
    
    std::string generateDebugString() const;
};

} // namespace CHTL_JS