#ifndef CJMOD_CHTLJS_FUNCTION_H
#define CJMOD_CHTLJS_FUNCTION_H

#include "Syntax.h"
#include "Arg.h"
#include "CJMODGenerator.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace CHTL {
namespace CJMOD {

struct FunctionParameter {
    std::string name;
    std::string type;
    std::string defaultValue;
    bool isOptional;
    bool isUnordered;
    bool isVariadic;
    
    FunctionParameter() : isOptional(false), isUnordered(false), isVariadic(false) {}
};

struct FunctionSignature {
    std::string name;
    std::vector<FunctionParameter> parameters;
    std::string returnType;
    bool supportsVirtualObject;
    bool supportsUnorderedPairs;
    bool supportsOptionalPairs;
    bool supportsUnquotedLiterals;
    
    FunctionSignature() : supportsVirtualObject(true), supportsUnorderedPairs(true), 
                         supportsOptionalPairs(true), supportsUnquotedLiterals(true) {}
};

class CHTLJSFunction {
private:
    FunctionSignature signature;
    std::string template;
    std::shared_ptr<Arg> parsedArgs;
    std::unordered_map<std::string, std::string> virtualObjectBindings;
    std::vector<std::string> generatedFunctions;
    bool isBuilt;
    
    // 内部方法
    bool validateSignature() const;
    bool validateParameters() const;
    std::string generateParameterList() const;
    std::string generateFunctionBody() const;
    std::string generateVirtualObjectSupport() const;
    std::string generateUnorderedPairSupport() const;
    std::string generateOptionalPairSupport() const;
    std::string generateUnquotedLiteralSupport() const;
    
public:
    CHTLJSFunction();
    CHTLJSFunction(const std::string& name, const std::string& template);
    CHTLJSFunction(const FunctionSignature& signature, const std::string& template);
    ~CHTLJSFunction() = default;
    
    // 静态工厂方法
    static std::shared_ptr<CHTLJSFunction> CreateCHTLJSFunction(const std::string& template);
    static std::shared_ptr<CHTLJSFunction> CreateCHTLJSFunction(const std::string& name, const std::string& template);
    
    // 函数构建
    bool build();
    bool build(const std::string& template);
    bool rebuild();
    
    // 签名管理
    void setSignature(const FunctionSignature& signature);
    FunctionSignature getSignature() const;
    void setName(const std::string& name);
    std::string getName() const;
    
    // 参数管理
    void addParameter(const FunctionParameter& parameter);
    void removeParameter(const std::string& parameterName);
    void setParameters(const std::vector<FunctionParameter>& parameters);
    std::vector<FunctionParameter> getParameters() const;
    FunctionParameter getParameter(const std::string& name) const;
    
    // 模板管理
    void setTemplate(const std::string& template);
    std::string getTemplate() const;
    bool parseTemplate();
    
    // 虚对象支持
    void bindVirtualObject(const std::string& objectName, const std::string& binding);
    void unbindVirtualObject(const std::string& objectName);
    bool hasVirtualObjectBinding(const std::string& objectName) const;
    std::string getVirtualObjectBinding(const std::string& objectName) const;
    std::unordered_map<std::string, std::string> getAllVirtualObjectBindings() const;
    
    // 特性支持
    void setSupportsVirtualObject(bool support);
    void setSupportsUnorderedPairs(bool support);
    void setSupportsOptionalPairs(bool support);
    void setSupportsUnquotedLiterals(bool support);
    
    bool getSupportsVirtualObject() const;
    bool getSupportsUnorderedPairs() const;
    bool getSupportsOptionalPairs() const;
    bool getSupportsUnquotedLiterals() const;
    
    // 代码生成
    std::string generateJavaScript() const;
    std::string generateCHTLJS() const;
    std::string generateFunctionCall(const std::vector<std::string>& arguments) const;
    std::string generateFunctionDeclaration() const;
    
    // 函数调用
    std::string call(const std::vector<std::string>& arguments) const;
    std::string call(const std::unordered_map<std::string, std::string>& arguments) const;
    std::string call(const std::string& argumentString) const;
    
    // 验证
    bool validate() const;
    bool validateArguments(const std::vector<std::string>& arguments) const;
    bool validateArguments(const std::unordered_map<std::string, std::string>& arguments) const;
    std::vector<std::string> getValidationErrors() const;
    
    // 绑定虚对象
    static bool bindVirtualObject(const std::string& functionName);
    static bool unbindVirtualObject(const std::string& functionName);
    static bool hasVirtualObjectBinding(const std::string& functionName);
    
    // 注册表管理
    static void registerFunction(const std::string& name, std::shared_ptr<CHTLJSFunction> function);
    static void unregisterFunction(const std::string& name);
    static std::shared_ptr<CHTLJSFunction> getFunction(const std::string& name);
    static bool hasFunction(const std::string& name);
    static std::vector<std::string> getAllFunctionNames();
    static void clearRegistry();
    
    // 调试方法
    void printSignature() const;
    void printParameters() const;
    void printTemplate() const;
    void printVirtualObjectBindings() const;
    void printGeneratedFunctions() const;
    void printStatistics() const;
    
    // 工具方法
    void clear();
    void reset();
    bool isEmpty() const;
    bool isBuilt() const;
    
private:
    static std::unordered_map<std::string, std::shared_ptr<CHTLJSFunction>> functionRegistry;
    static std::unordered_map<std::string, bool> virtualObjectBindings;
};

} // namespace CJMOD
} // namespace CHTL

#endif // CJMOD_CHTLJS_FUNCTION_H