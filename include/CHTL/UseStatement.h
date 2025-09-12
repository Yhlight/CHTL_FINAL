#pragma once

#include "CHTL/CHTLNode.h"
#include "CHTL/ConfigurationSystem.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * Use语句类型
 */
enum class UseStatementType {
    HTML5,
    CONFIG
};

/**
 * Use语句
 */
class UseStatement : public CHTLNode {
public:
    UseStatement(UseStatementType type, const std::string& value = "");
    ~UseStatement();
    
    // 基本信息
    UseStatementType getType() const;
    void setType(UseStatementType type);
    
    const std::string& getValue() const;
    void setValue(const std::string& value);
    
    // 节点功能
    std::string generateHTML() const override;
    std::string generateUseStatement() const;
    std::shared_ptr<CHTLNode> clone() const override;
    
    // 验证
    bool validate() const override;
    std::vector<std::string> getValidationErrors() const;
    
private:
    UseStatementType type_;
    std::string value_;
    
    // 生成辅助函数
    std::string generateHTML5Statement() const;
    std::string generateConfigStatement() const;
};

/**
 * Use语句管理器
 */
class UseStatementManager {
public:
    UseStatementManager();
    ~UseStatementManager();
    
    // Use语句管理
    void addUseStatement(std::shared_ptr<UseStatement> useStatement);
    void removeUseStatement(const std::string& value);
    std::vector<std::shared_ptr<UseStatement>> getUseStatements() const;
    std::vector<std::shared_ptr<UseStatement>> getUseStatementsByType(UseStatementType type) const;
    
    // 配置管理
    void setConfigurationManager(std::shared_ptr<ConfigurationManager> configManager);
    std::shared_ptr<ConfigurationManager> getConfigurationManager() const;
    
    // 应用Use语句
    void applyUseStatements();
    void applyHTML5UseStatement();
    void applyConfigUseStatement(const std::string& configName);
    
    // 查询
    bool hasUseStatement(const std::string& value) const;
    bool hasHTML5UseStatement() const;
    bool hasConfigUseStatement(const std::string& configName) const;
    
    // 清理
    void clear();
    void clearUseStatements();
    
private:
    std::vector<std::shared_ptr<UseStatement>> use_statements_;
    std::shared_ptr<ConfigurationManager> config_manager_;
    
    // 应用辅助函数
    void applyHTML5Configuration();
    void applyConfigConfiguration(const std::string& configName);
};

/**
 * Use语句解析器
 */
class UseStatementParser {
public:
    UseStatementParser();
    ~UseStatementParser();
    
    // 解析Use语句
    std::shared_ptr<UseStatement> parseUseStatement(const std::string& content);
    std::vector<std::shared_ptr<UseStatement>> parseUseStatements(const std::string& content);
    
    // 解析辅助函数
    bool parseHTML5Statement(const std::string& content, UseStatement& useStatement);
    bool parseConfigStatement(const std::string& content, UseStatement& useStatement);
    
    // 验证
    bool validateUseStatement(const UseStatement& useStatement) const;
    std::vector<std::string> getValidationErrors(const UseStatement& useStatement) const;
    
private:
    // 解析辅助函数
    std::string trim(const std::string& str) const;
    std::vector<std::string> split(const std::string& str, char delimiter) const;
    bool isHTML5Statement(const std::string& content) const;
    bool isConfigStatement(const std::string& content) const;
    std::string extractConfigName(const std::string& content) const;
};

} // namespace CHTL