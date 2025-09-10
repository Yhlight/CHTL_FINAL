#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <any>
#include <functional>

namespace CHTL_JS {

// 参数类型枚举
enum class ArgType {
    UNKNOWN = 0,
    REQUIRED,       // 必需参数
    OPTIONAL,       // 可选参数
    DEFAULT,        // 默认参数
    REST,           // 剩余参数
    DESTRUCTURED,   // 解构参数
    SPREAD,         // 展开参数
    ASYNC,          // 异步参数
    GENERATOR,      // 生成器参数
    CALLBACK        // 回调参数
};

// 参数绑定选项
struct ArgBindOptions {
    bool strict = false;
    bool validate = true;
    bool transform = false;
    std::map<std::string, std::any> metadata;
};

// 参数转换选项
struct ArgTransformOptions {
    bool preserveType = true;
    bool validateInput = true;
    bool validateOutput = true;
    bool strictMode = false;
    std::map<std::string, std::any> metadata;
};

// 参数验证结果
struct ArgValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, std::any> metadata;
    
    ArgValidationResult() : valid(false) {}
};

// CJMOD参数类
class CJMODArg {
public:
    CJMODArg();
    CJMODArg(const std::string& name, const std::any& value = std::any{});
    CJMODArg(const std::string& name, const std::any& value, ArgType type);
    ~CJMODArg();
    
    // 基本属性
    void setName(const std::string& name);
    std::string getName() const;
    
    void setValue(const std::any& value);
    std::any getValue() const;
    
    void setType(ArgType type);
    ArgType getType() const;
    
    void setRequired(bool required);
    bool isRequired() const;
    
    void setOptional(bool optional);
    bool isOptional() const;
    
    void setDefault(const std::any& defaultValue);
    std::any getDefault() const;
    bool hasDefault() const;
    
    // 参数信息
    void setDescription(const std::string& description);
    std::string getDescription() const;
    
    void setTypeName(const std::string& typeName);
    std::string getTypeName() const;
    
    void setPosition(int position);
    int getPosition() const;
    
    void setIndex(int index);
    int getIndex() const;
    
    // 验证
    ArgValidationResult validate() const;
    ArgValidationResult validate(const std::any& value) const;
    bool isValid() const;
    bool isValid(const std::any& value) const;
    
    // 绑定
    std::any bind(const std::any& value, const ArgBindOptions& options = ArgBindOptions{}) const;
    std::any bind(const std::vector<std::any>& values, const ArgBindOptions& options = ArgBindOptions{}) const;
    std::any bind(const std::map<std::string, std::any>& object, const ArgBindOptions& options = ArgBindOptions{}) const;
    
    // 转换
    std::any transform(const std::any& value, const ArgTransformOptions& options = ArgTransformOptions{}) const;
    std::string transformToString(const std::any& value, const ArgTransformOptions& options = ArgTransformOptions{}) const;
    std::vector<std::any> transformToArray(const std::any& value, const ArgTransformOptions& options = ArgTransformOptions{}) const;
    std::map<std::string, std::any> transformToObject(const std::any& value, const ArgTransformOptions& options = ArgTransformOptions{}) const;
    
    // 填充
    std::any fillValue(const std::any& value) const;
    std::any fillValue(const std::vector<std::any>& values) const;
    std::any fillValue(const std::map<std::string, std::any>& object) const;
    
    // 匹配
    std::any match(const std::string& pattern, std::function<std::any(const std::string&)> matcher) const;
    std::any match(const std::string& pattern, std::function<std::string(const std::string&)> matcher) const;
    std::any match(const std::string& pattern, std::function<int(const std::string&)> matcher) const;
    std::any match(const std::string& pattern, std::function<bool(const std::string&)> matcher) const;
    
    // 检查
    bool checkType(const std::any& value) const;
    bool checkValue(const std::any& value) const;
    bool checkRequired(const std::any& value) const;
    bool checkOptional(const std::any& value) const;
    
    // 比较
    bool equals(const CJMODArg& other) const;
    bool equals(const std::any& value) const;
    
    // 克隆
    std::shared_ptr<CJMODArg> clone() const;
    std::shared_ptr<CJMODArg> deepClone() const;
    
    // 转换
    std::string toString() const;
    std::string toDebugString() const;
    std::string toJSON() const;
    std::string toXML() const;
    std::string toYAML() const;
    
    // 格式化
    std::string format() const;
    std::string minify() const;
    std::string beautify() const;
    
    // 统计
    size_t getValueSize() const;
    std::string getValueType() const;
    bool hasValue() const;
    
    // 重置
    void reset();
    
private:
    std::string name;
    std::any value;
    ArgType type;
    bool required;
    bool optional;
    std::any defaultValue;
    std::string description;
    std::string typeName;
    int position;
    int index;
    
    // 辅助方法
    bool validateArg() const;
    bool validateValue(const std::any& value) const;
    bool validateType(const std::any& value) const;
    bool validateRequired(const std::any& value) const;
    bool validateOptional(const std::any& value) const;
    
    std::any bindValue(const std::any& value, const ArgBindOptions& options) const;
    std::any bindArray(const std::vector<std::any>& values, const ArgBindOptions& options) const;
    std::any bindObject(const std::map<std::string, std::any>& object, const ArgBindOptions& options) const;
    
    std::any transformValue(const std::any& value, const ArgTransformOptions& options) const;
    std::string transformToStringValue(const std::any& value, const ArgTransformOptions& options) const;
    std::vector<std::any> transformToArrayValue(const std::any& value, const ArgTransformOptions& options) const;
    std::map<std::string, std::any> transformToObjectValue(const std::any& value, const ArgTransformOptions& options) const;
    
    std::any fillValueFromValue(const std::any& value) const;
    std::any fillValueFromArray(const std::vector<std::any>& values) const;
    std::any fillValueFromObject(const std::map<std::string, std::any>& object) const;
    
    bool checkTypeValue(const std::any& value) const;
    bool checkValueValue(const std::any& value) const;
    bool checkRequiredValue(const std::any& value) const;
    bool checkOptionalValue(const std::any& value) const;
    
    std::string generateJSON() const;
    std::string generateXML() const;
    std::string generateYAML() const;
    
    std::string formatArg() const;
    std::string minifyArg() const;
    std::string beautifyArg() const;
    
    std::string getValueTypeName() const;
    size_t getValueSizeValue() const;
};

} // namespace CHTL_JS