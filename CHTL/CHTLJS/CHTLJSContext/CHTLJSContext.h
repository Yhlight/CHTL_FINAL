#ifndef CHTLJS_CONTEXT_H
#define CHTLJS_CONTEXT_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>

namespace CHTLJS {

class CHTLJSContext {
private:
    std::unordered_map<std::string, std::string> variables;
    std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>> functions;
    std::vector<std::unordered_map<std::string, std::string>> scopeStack;
    
    // CHTL JS 特有功能
    std::unordered_map<std::string, std::string> virtualObjects;
    std::unordered_map<std::string, std::string> selectors;
    std::unordered_map<std::string, std::string> reactiveValues;
    
    // 配置选项
    bool debugMode;
    bool minifyOutput;
    std::string outputPath;
    
    void initializeBuiltinFunctions();
    void initializeCHTLJSFunctions();
    
public:
    CHTLJSContext();
    ~CHTLJSContext() = default;
    
    // 变量管理
    void setVariable(const std::string& name, const std::string& value);
    std::string getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    void removeVariable(const std::string& name);
    
    // 函数管理
    void setFunction(const std::string& name, std::function<std::string(const std::vector<std::string>&)> func);
    std::function<std::string(const std::vector<std::string>&)> getFunction(const std::string& name) const;
    bool hasFunction(const std::string& name) const;
    void removeFunction(const std::string& name);
    
    // 作用域管理
    void pushScope();
    void popScope();
    void clearScope();
    
    // CHTL JS 特有功能
    void setVirtualObject(const std::string& name, const std::string& value);
    std::string getVirtualObject(const std::string& name) const;
    bool hasVirtualObject(const std::string& name) const;
    
    void setSelector(const std::string& name, const std::string& value);
    std::string getSelector(const std::string& name) const;
    bool hasSelector(const std::string& name) const;
    
    void setReactiveValue(const std::string& name, const std::string& value);
    std::string getReactiveValue(const std::string& name) const;
    bool hasReactiveValue(const std::string& name) const;
    
    // 配置管理
    void setDebugMode(bool debug);
    bool isDebugMode() const;
    
    void setMinifyOutput(bool minify);
    bool isMinifyOutput() const;
    
    void setOutputPath(const std::string& path);
    std::string getOutputPath() const;
    
    // 工具方法
    void clear();
    void reset();
    
    std::vector<std::string> getVariableNames() const;
    std::vector<std::string> getFunctionNames() const;
    std::vector<std::string> getVirtualObjectNames() const;
    std::vector<std::string> getSelectorNames() const;
    std::vector<std::string> getReactiveValueNames() const;
    
    // 调试方法
    void printContext() const;
    void printVariables() const;
    void printFunctions() const;
    void printVirtualObjects() const;
    void printSelectors() const;
    void printReactiveValues() const;
};

} // namespace CHTLJS

#endif // CHTLJS_CONTEXT_H