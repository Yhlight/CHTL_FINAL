#pragma once

#include "BaseNode.hpp"
#include <string>
#include <map>

namespace CHTL {

/**
 * @brief 配置节点
 * 
 * 表示配置定义
 */
class ConfigNode : public BaseNode {
public:
    ConfigNode(size_t line = 0, size_t column = 0);
    
    // 添加配置项
    void addConfiguration(const std::string& key, const std::string& value);
    
    // 获取配置项
    std::string getConfiguration(const std::string& key) const;
    
    // 获取所有配置
    const std::map<std::string, std::string>& getConfigurations() const;
    
    // 添加名称块
    void addNameBlock(const std::string& oldName, const std::string& newName);
    
    // 获取名称块
    const std::map<std::string, std::string>& getNameBlocks() const;
    
    // 转换为 HTML
    std::string toHTML() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    std::map<std::string, std::string> configurations_;
    std::map<std::string, std::string> nameBlocks_;
};

} // namespace CHTL