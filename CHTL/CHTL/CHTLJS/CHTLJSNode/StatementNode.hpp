#pragma once

#include "CHTLJSBaseNode.hpp"
#include <string>

namespace CHTL {

/**
 * @brief 语句节点
 * 
 * 根据 CHTL.md 文档实现
 * 支持 CHTL JS 语句的表示
 */
class StatementNode : public CHTLJSBaseNode {
public:
    StatementNode(const std::string& content = "", size_t line = 0, size_t column = 0);
    
    // 设置语句内容
    void setContent(const std::string& content) { content_ = content; }
    
    // 获取语句内容
    const std::string& getContent() const { return content_; }
    
    // 转换为 JavaScript 代码
    std::string toJavaScript() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    std::string content_;
};

} // namespace CHTL