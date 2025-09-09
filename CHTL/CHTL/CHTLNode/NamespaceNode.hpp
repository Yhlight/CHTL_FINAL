#pragma once

#include "BaseNode.hpp"
#include <string>

namespace CHTL {

/**
 * @brief 命名空间节点
 * 
 * 表示命名空间定义
 */
class NamespaceNode : public BaseNode {
public:
    NamespaceNode(const std::string& namespaceName, size_t line = 0, size_t column = 0);
    
    // 获取命名空间名称
    const std::string& getNamespaceName() const { return namespaceName_; }
    
    // 设置命名空间名称
    void setNamespaceName(const std::string& name) { namespaceName_ = name; }
    
    // 转换为 HTML
    std::string toHTML() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    std::string namespaceName_;
};

} // namespace CHTL