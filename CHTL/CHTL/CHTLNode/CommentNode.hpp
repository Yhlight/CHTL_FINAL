#pragma once

#include "BaseNode.hpp"
#include <string>

namespace CHTL {

/**
 * @brief 注释节点
 * 
 * 表示注释内容，支持多种注释格式
 */
class CommentNode : public BaseNode {
public:
    enum class CommentType {
        SINGLE_LINE,    // 单行注释 //
        MULTI_LINE,     // 多行注释 /* */
        HTML_COMMENT,   // HTML注释 <!-- -->
        CSS_COMMENT     // CSS注释 /* */
    };
    
    CommentNode(const std::string& content, CommentType type = CommentType::SINGLE_LINE, 
                size_t line = 0, size_t column = 0);
    
    // 获取注释类型
    CommentType getCommentType() const { return commentType_; }
    
    // 设置注释类型
    void setCommentType(CommentType type) { commentType_ = type; }
    
    // 获取注释内容
    const std::string& getContent() const { return getValue(); }
    
    // 设置注释内容
    void setContent(const std::string& content) { setValue(content); }
    
    // 检查是否为空注释
    bool isEmpty() const;
    
    // 检查是否为空白注释
    bool isWhitespace() const;
    
    // 转换为 HTML
    std::string toHTML() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    CommentType commentType_;
    
    // 转义 HTML 特殊字符
    std::string escapeHTML(const std::string& text) const;
};

} // namespace CHTL