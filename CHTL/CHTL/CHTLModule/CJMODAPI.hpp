#pragma once

#include <CHTL/CHTLNode/BaseNode.hpp>
#include <CHTL/CHTLJS/CHTLJSNode/CHTLJSBaseNode.hpp>
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <set>

namespace CHTL {

/**
 * @brief CJMOD API 参数类型
 */
enum class CJMODParameterType {
    STRING,     // 字符串参数
    NUMBER,     // 数字参数
    BOOLEAN,    // 布尔参数
    OBJECT,     // 对象参数
    ARRAY,      // 数组参数
    FUNCTION    // 函数参数
};

/**
 * @brief CJMOD API 参数信息
 */
struct CJMODParameter {
    std::string name;
    CJMODParameterType type;
    std::string defaultValue;
    bool isRequired;
    std::string description;
    
    CJMODParameter() : type(CJMODParameterType::STRING), isRequired(false) {}
};

/**
 * @brief CJMOD API 函数信息
 */
struct CJMODFunction {
    std::string name;
    std::vector<CJMODParameter> parameters;
    CJMODParameterType returnType;
    std::string description;
    std::string implementation;
    
    CJMODFunction() : returnType(CJMODParameterType::STRING) {}
};

/**
 * @brief CJMOD API 类
 * 
 * 根据 CHTL.md 文档实现
 * 提供 CHTL JS 模块的语法分析、参数绑定、代码生成等功能
 */
class CJMODAPI {
public:
    CJMODAPI();
    ~CJMODAPI() = default;
    
    // 模块解析
    bool parseModule(const std::string& moduleName, const std::string& content);
    bool parseModuleFromFile(const std::string& moduleName, const std::string& filePath);
    
    // 语法分析
    std::vector<std::string> analyzeSyntax(const std::string& content);
    std::vector<std::string> getSyntaxErrors(const std::string& content);
    bool validateSyntax(const std::string& content);
    
    // 参数绑定
    bool bindParameter(const std::string& functionName, const std::string& parameterName, const std::string& value);
    bool bindParameters(const std::string& functionName, const std::map<std::string, std::string>& parameters);
    std::map<std::string, std::string> getBoundParameters(const std::string& functionName) const;
    
    // 函数管理
    bool registerFunction(const CJMODFunction& function);
    bool unregisterFunction(const std::string& functionName);
    std::shared_ptr<CJMODFunction> getFunction(const std::string& functionName) const;
    std::vector<std::string> getFunctionNames() const;
    
    // 代码生成
    std::string generateCode(const std::string& functionName) const;
    std::string generateModuleCode(const std::string& moduleName) const;
    std::string generateParameterBindingCode(const std::string& functionName) const;
    
    // 模块管理
    bool loadModule(const std::string& moduleName);
    bool unloadModule(const std::string& moduleName);
    bool isModuleLoaded(const std::string& moduleName) const;
    std::vector<std::string> getLoadedModules() const;
    
    // 依赖管理
    bool addDependency(const std::string& moduleName, const std::string& dependency);
    std::vector<std::string> getDependencies(const std::string& moduleName) const;
    bool resolveDependencies(const std::string& moduleName);
    
    // 导出管理
    bool addExport(const std::string& moduleName, const std::string& exportName, const std::string& value);
    std::string getExport(const std::string& moduleName, const std::string& exportName) const;
    std::map<std::string, std::string> getExports(const std::string& moduleName) const;
    
    // 验证和测试
    bool validateModule(const std::string& moduleName) const;
    bool testFunction(const std::string& functionName, const std::map<std::string, std::string>& testParameters) const;
    std::string runFunction(const std::string& functionName, const std::map<std::string, std::string>& parameters) const;
    
private:
    std::map<std::string, std::shared_ptr<CJMODFunction>> functions_;
    std::map<std::string, std::map<std::string, std::string>> boundParameters_;
    std::map<std::string, std::shared_ptr<CHTLJSBaseNode>> moduleASTs_;
    std::map<std::string, std::vector<std::string>> moduleDependencies_;
    std::map<std::string, std::map<std::string, std::string>> moduleExports_;
    std::set<std::string> loadedModules_;
    
    // 内部方法
    bool parseCHTLJSContent(const std::string& content, std::shared_ptr<CHTLJSBaseNode>& ast);
    std::string generateFunctionCode(const CJMODFunction& function) const;
    std::string generateParameterValidationCode(const CJMODFunction& function) const;
    bool validateParameters(const CJMODFunction& function, const std::map<std::string, std::string>& parameters) const;
    std::string convertParameterType(const std::string& value, CJMODParameterType type) const;
};

} // namespace CHTL