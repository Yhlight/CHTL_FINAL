#pragma once
#include "Arg.h"
#include "AtomArg.h"
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

namespace CHTL {

// 函数类型枚举
enum class CHTLJSFunctionType {
    Utility,        // 工具函数
    EventHandler,   // 事件处理函数
    Animation,      // 动画函数
    Router,         // 路由函数
    VirtualObject,  // 虚对象函数
    Custom          // 自定义函数
};

// 函数参数结构
struct FunctionParameter {
    std::string name;
    ArgType type;
    bool isOptional;
    Arg defaultValue;
    std::string description;
};

// CHTL JS函数类
class CHTLJSFunction {
public:
    CHTLJSFunction();
    explicit CHTLJSFunction(const std::string& name);
    ~CHTLJSFunction() = default;
    
    // 基本信息
    void setName(const std::string& name);
    std::string getName() const { return name; }
    
    void setDescription(const std::string& description);
    std::string getDescription() const { return description; }
    
    void setType(CHTLJSFunctionType type);
    CHTLJSFunctionType getType() const { return type; }
    
    void setReturnType(ArgType returnType);
    ArgType getReturnType() const { return returnType; }
    
    // 参数管理
    void addParameter(const FunctionParameter& param);
    void removeParameter(const std::string& paramName);
    bool hasParameter(const std::string& paramName) const;
    FunctionParameter getParameter(const std::string& paramName) const;
    std::vector<FunctionParameter> getParameters() const { return parameters; }
    
    // 函数体管理
    void setBody(const std::string& body);
    std::string getBody() const { return body; }
    
    void setImplementation(std::function<Arg(const std::vector<Arg>&)> impl);
    std::function<Arg(const std::vector<Arg>&)> getImplementation() const { return implementation; }
    
    // 函数调用
    Arg call(const std::vector<Arg>& args) const;
    Arg call(const std::map<std::string, Arg>& namedArgs) const;
    
    // 验证
    bool isValid() const;
    std::vector<std::string> getValidationErrors() const;
    
    // 序列化
    std::string toJSON() const;
    std::string toString() const;

private:
    std::string name;
    std::string description;
    CHTLJSFunctionType type;
    ArgType returnType;
    std::vector<FunctionParameter> parameters;
    std::string body;
    std::function<Arg(const std::vector<Arg>&)> implementation;
    
    void validateFunction();
    std::vector<std::string> validationErrors;
    
    Arg validateAndConvertArgs(const std::vector<Arg>& args) const;
    Arg executeImplementation(const std::vector<Arg>& args) const;
};

} // namespace CHTL