#pragma once

#include "CHTLJSBaseNode.hpp"
#include <string>
#include <map>

namespace CHTL {

/**
 * @brief 虚对象节点
 * 
 * 表示 vir 语法
 */
class VirNode : public CHTLJSBaseNode {
public:
    VirNode(const std::string& name, size_t line = 0, size_t column = 0);
    
    // 获取虚对象名称
    const std::string& getName() const { return name_; }
    
    // 设置虚对象名称
    void setName(const std::string& name) { name_ = name; }
    
    // 添加键值对
    void addKeyValue(const std::string& key, const std::string& value);
    
    // 获取所有键值对
    const std::map<std::string, std::string>& getKeyValues() const { return keyValues_; }
    
    // 转换为 JavaScript 代码
    std::string toJavaScript() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    std::string name_;
    std::map<std::string, std::string> keyValues_;
};

} // namespace CHTL