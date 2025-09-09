#ifndef CHTL_COMMENT_NODE_H
#define CHTL_COMMENT_NODE_H

#include "BaseNode.h"

namespace CHTL {

class CommentNode : public BaseNode {
private:
    CommentType commentType;
    bool isGeneratorComment;  // 是否为生成器注释

public:
    enum class CommentType {
        SINGLE_LINE,    // 单行注释 //
        MULTI_LINE,     // 多行注释 /* */
        GENERATOR       // 生成器注释 --
    };

    CommentNode(const std::string& content = "", CommentType type = CommentType::SINGLE_LINE);
    virtual ~CommentNode() = default;
    
    // 注释内容
    const std::string& getContent() const { return value; }
    void setContent(const std::string& content) { value = content; }
    
    // 注释类型
    CommentType getCommentType() const { return commentType; }
    void setCommentType(CommentType type) { commentType = type; }
    
    // 生成器注释
    bool isGeneratorCommentType() const { return isGeneratorComment; }
    void setGeneratorComment(bool generator) { isGeneratorComment = generator; }
    
    // 注释处理
    std::string getTrimmedContent() const;
    bool isEmpty() const;
    bool isWhitespace() const;
    
    // 验证
    bool isValid() const override;
    
    // 转换为HTML
    std::string toHTML() const override;
    
    // 调试信息
    std::string getDebugInfo() const override;
    
    // 克隆
    std::shared_ptr<BaseNode> clone() const override;
    
    // 比较操作
    bool operator==(const CommentNode& other) const;
    bool operator!=(const CommentNode& other) const;

private:
    // 辅助方法
    std::string getCommentTypeName() const;
    bool isWhitespaceChar(char c) const;
};

} // namespace CHTL

#endif // CHTL_COMMENT_NODE_H