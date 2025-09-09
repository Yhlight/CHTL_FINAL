#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

/**
 * @brief CHTL JS 基础节点
 * 
 * 根据 CHTL.md 文档第 1200-1800 行实现
 * 支持 CHTL JS 扩展语法的 AST 节点
 */
class CHTLJSBaseNode {
public:
    enum class NodeType {
        FILELOADER,
        SCRIPT,
        ENHANCED_SELECTOR,
        ANIMATE,
        LISTEN,
        UTIL,
        EXPRESSION,
        STATEMENT,
        FUNCTION,
        VARIABLE,
        OPERATOR
    };
    
    CHTLJSBaseNode(NodeType type, size_t line = 0, size_t column = 0);
    virtual ~CHTLJSBaseNode() = default;
    
    // 获取节点类型
    NodeType getType() const { return type_; }
    
    // 获取行号
    size_t getLine() const { return line_; }
    
    // 获取列号
    size_t getColumn() const { return column_; }
    
    // 添加子节点
    void addChild(std::shared_ptr<CHTLJSBaseNode> child);
    
    // 获取子节点
    const std::vector<std::shared_ptr<CHTLJSBaseNode>>& getChildren() const { return children_; }
    
    // 获取子节点数量
    size_t getChildCount() const { return children_.size(); }
    
    // 设置属性
    void setAttribute(const std::string& key, const std::string& value);
    
    // 获取属性
    std::string getAttribute(const std::string& key) const;
    
    // 检查是否有属性
    bool hasAttribute(const std::string& key) const;
    
    // 转换为 JavaScript 代码
    virtual std::string toJavaScript() const = 0;
    
    // 转换为字符串
    virtual std::string toString() const;
    
protected:
    NodeType type_;
    size_t line_;
    size_t column_;
    std::vector<std::shared_ptr<CHTLJSBaseNode>> children_;
    std::map<std::string, std::string> attributes_;
};

} // namespace CHTL