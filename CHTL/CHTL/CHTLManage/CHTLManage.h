#ifndef CHTL_MANAGE_H
#define CHTL_MANAGE_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

class CHTLManage {
private:
    std::map<std::string, std::string> configurations;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    bool debugMode;
    bool strictMode;

public:
    CHTLManage();
    virtual ~CHTLManage() = default;
    
    // 配置管理
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key) const;
    bool hasConfiguration(const std::string& key) const;
    void removeConfiguration(const std::string& key);
    const std::map<std::string, std::string>& getConfigurations() const { return configurations; }
    
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
    
    // 管理器操作
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
    CHTLManage clone() const;
    
    // 比较
    bool operator==(const CHTLManage& other) const;
    bool operator!=(const CHTLManage& other) const;
};

} // namespace CHTL

#endif // CHTL_MANAGE_H