#pragma once

#include "BaseNode.hpp"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// 前向声明
class ConstraintNode;

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
    
    // 添加约束
    void addConstraint(std::shared_ptr<ConstraintNode> constraint);
    
    // 获取约束
    const std::vector<std::shared_ptr<ConstraintNode>>& getConstraints() const;
    
    // 转换为 HTML
    std::string toHTML() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    std::string namespaceName_;
    std::vector<std::shared_ptr<ConstraintNode>> constraints_;
};

} // namespace CHTL