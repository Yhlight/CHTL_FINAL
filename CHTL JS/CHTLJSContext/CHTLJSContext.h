#ifndef CHTLJSCONTEXT_H
#define CHTLJSCONTEXT_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTLJS {

/**
 * CHTL JS编译上下文
 * 管理编译过程中的状态、变量、函数等信息
 */
class CHTLJSContext {
public:
    CHTLJSContext();
    ~CHTLJSContext();

    // 上下文管理
    void reset();
    void setSourceCode(const std::string& code);
    std::string getSourceCode() const;

    // 变量管理
    void addVariable(const std::string& name, const std::string& value);
    std::string getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;

    // 函数管理
    void addFunction(const std::string& name, const std::string& definition);
    std::string getFunction(const std::string& name) const;
    bool hasFunction(const std::string& name) const;

    // 虚对象管理
    void addVirtualObject(const std::string& name, const std::string& definition);
    std::string getVirtualObject(const std::string& name) const;
    bool hasVirtualObject(const std::string& name) const;

    // 配置管理
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key) const;

    // 错误处理
    void addError(const std::string& error);
    std::vector<std::string> getErrors() const;
    bool hasErrors() const;

    // 警告处理
    void addWarning(const std::string& warning);
    std::vector<std::string> getWarnings() const;

private:
    std::string sourceCode;
    std::map<std::string, std::string> variables;
    std::map<std::string, std::string> functions;
    std::map<std::string, std::string> virtualObjects;
    std::map<std::string, std::string> configuration;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

} // namespace CHTLJS

#endif // CHTLJSCONTEXT_H