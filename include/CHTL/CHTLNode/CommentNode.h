#pragma once

#include "BaseNode.h"

namespace CHTL {

/**
 * @brief 注释类型枚举
 */
enum class CommentType {
    LINE,           // 行注释 //
    BLOCK,          // 块注释 /* */
    GENERATOR       // 生成器注释 --
};

/**
 * @brief 注释节点
 * 表示CHTL注释
 */
class CommentNode : public BaseNode {
public:
    CommentNode(CommentType type, const std::string& content = "");
    virtual ~CommentNode();
    
    // 注释类型
    CommentType getCommentType() const;
    
    // 注释内容
    void setCommentContent(const std::string& content);
    std::string getCommentContent() const;
    
    // 是否为生成器注释
    bool isGeneratorComment() const;
    
    // 访问者模式
    virtual void accept(NodeVisitor& visitor) override;
    
    // 克隆节点
    virtual std::shared_ptr<BaseNode> clone() const override;
    
    // 节点验证
    virtual bool validate() const override;
    
    // 调试信息
    virtual std::string toString() const override;

private:
    CommentType m_commentType;
    std::string m_commentContent;
};

} // namespace CHTL