#ifndef CUSTOM_NODE_H
#define CUSTOM_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>

namespace CHTL {

class CustomNode : public BaseNode {
public:
    CustomNode(const std::string& name, const std::string& type);
    ~CustomNode() = default;
    
    // 自定义类型
    const std::string& getCustomType() const { return customType_; }
    void setCustomType(const std::string& type) { customType_ = type; }
    
    // 自定义内容
    void setCustomContent(const std::string& content);
    const std::string& getCustomContent() const { return customContent_; }
    
    // 操作管理
    void addOperation(const std::string& operation);
    const std::vector<std::string>& getOperations() const { return operations_; }
    
    // 目标管理
    void setTarget(const std::string& target);
    const std::string& getTarget() const { return target_; }
    
    // 条件管理
    void setCondition(const std::string& condition);
    const std::string& getCondition() const { return condition_; }
    
    // 应用
    std::string apply(const std::string& targetContent) const;
    
    // 重写基类方法
    std::string generateHTML() const override;
    std::string generateCSS() const override;
    std::string generateJS() const override;
    
private:
    std::string customType_;
    std::string customContent_;
    std::vector<std::string> operations_;
    std::string target_;
    std::string condition_;
    
    // 应用辅助方法
    std::string processOperations(const std::string& targetContent) const;
    std::string processInsert(const std::string& targetContent) const;
    std::string processDelete(const std::string& targetContent) const;
    std::string processReplace(const std::string& targetContent) const;
    bool processCondition(const std::string& targetContent) const;
};

} // namespace CHTL

#endif // CUSTOM_NODE_H