#pragma once

#include "BaseNode.h"

namespace CHTL {

/**
 * @brief 操作符类型枚举
 */
enum class OperatorType {
    DELETE,         // delete操作
    INSERT,         // insert操作
    INHERIT,        // inherit操作
    USE             // use操作
};

/**
 * @brief 插入位置枚举
 */
enum class InsertPosition {
    AFTER,          // after
    BEFORE,         // before
    REPLACE,        // replace
    AT_TOP,         // at top
    AT_BOTTOM       // at bottom
};

/**
 * @brief 操作符节点
 * 表示CHTL操作符定义
 */
class OperatorNode : public BaseNode {
public:
    OperatorNode(OperatorType type);
    virtual ~OperatorNode();
    
    // 操作符类型
    OperatorType getOperatorType() const;
    
    // 目标元素
    void setTarget(const std::string& target);
    std::string getTarget() const;
    bool hasTarget() const;
    
    // 插入位置（用于insert操作）
    void setInsertPosition(InsertPosition position);
    InsertPosition getInsertPosition() const;
    
    // 插入内容（用于insert操作）
    void setInsertContent(std::shared_ptr<BaseNode> content);
    std::shared_ptr<BaseNode> getInsertContent() const;
    bool hasInsertContent() const;
    
    // 继承目标（用于inherit操作）
    void setInheritTarget(const std::string& target);
    std::string getInheritTarget() const;
    bool hasInheritTarget() const;
    
    // 使用目标（用于use操作）
    void setUseTarget(const std::string& target);
    std::string getUseTarget() const;
    bool hasUseTarget() const;
    
    // 删除目标列表（用于delete操作）
    void addDeleteTarget(const std::string& target);
    std::vector<std::string> getDeleteTargets() const;
    bool hasDeleteTarget(const std::string& target) const;
    
    // 访问者模式
    virtual void accept(NodeVisitor& visitor) override;
    
    // 克隆节点
    virtual std::shared_ptr<BaseNode> clone() const override;
    
    // 节点验证
    virtual bool validate() const override;
    
    // 调试信息
    virtual std::string toString() const override;

private:
    OperatorType m_operatorType;
    std::string m_target;
    InsertPosition m_insertPosition;
    std::shared_ptr<BaseNode> m_insertContent;
    std::string m_inheritTarget;
    std::string m_useTarget;
    std::vector<std::string> m_deleteTargets;
};

} // namespace CHTL