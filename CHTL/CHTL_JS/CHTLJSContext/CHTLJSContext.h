#ifndef CHTL_JS_CONTEXT_H
#define CHTL_JS_CONTEXT_H

#include <string>
#include <map>
#include <vector>
#include <memory>

namespace CHTL_JS {

class CHTLJSContext {
private:
    std::string sourceCode;
    std::string outputPath;
    std::map<std::string, std::string> variables;
    std::map<std::string, std::string> configurations;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    bool debugMode;
    bool strictMode;

public:
    CHTLJSContext();
    CHTLJSContext(const std::string& source, const std::string& output);
    virtual ~CHTLJSContext() = default;
    
    // 基本属性
    const std::string& getSourceCode() const { return sourceCode; }
    void setSourceCode(const std::string& code) { sourceCode = code; }
    
    const std::string& getOutputPath() const { return outputPath; }
    void setOutputPath(const std::string& path) { outputPath = path; }
    
    // 变量管理
    void setVariable(const std::string& name, const std::string& value);
    std::string getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    void removeVariable(const std::string& name);
    
    // 配置管理
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key) const;
    bool hasConfiguration(const std::string& key) const;
    void removeConfiguration(const std::string& key);
    
    // 错误和警告管理
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    const std::vector<std::string>& getErrors() const { return errors; }
    const std::vector<std::string>& getWarnings() const { return warnings; }
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    void clearErrors();
    void clearWarnings();
    
    // 模式设置
    bool isDebugMode() const { return debugMode; }
    void setDebugMode(bool mode) { debugMode = mode; }
    
    bool isStrictMode() const { return strictMode; }
    void setStrictMode(bool mode) { strictMode = mode; }
    
    // 上下文操作
    void reset();
    void clear();
    bool isValid() const;
    
    // 字符串表示
    std::string toString() const;
    std::string toDebugString() const;
    
    // 序列化
    std::string toJSON() const;
    std::string toXML() const;
    std::string toYAML() const;
    
    // 克隆
    CHTLJSContext clone() const;
    
    // 比较
    bool operator==(const CHTLJSContext& other) const;
    bool operator!=(const CHTLJSContext& other) const;
};

} // namespace CHTL_JS

#endif // CHTL_JS_CONTEXT_H