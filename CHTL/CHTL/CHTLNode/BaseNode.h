#ifndef CHTL_BASE_NODE_H
#define CHTL_BASE_NODE_H

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

enum class NodeType {
    ELEMENT,        // HTML元素节点
    TEXT,           // 文本节点
    COMMENT,        // 注释节点
    TEMPLATE,       // 模板节点
    CUSTOM,         // 自定义节点
    STYLE,          // 样式节点
    SCRIPT,         // 脚本节点
    ORIGIN,         // 原始嵌入节点
    IMPORT,         // 导入节点
    CONFIGURATION,  // 配置节点
    NAMESPACE,      // 命名空间节点
    OPERATOR        // 操作符节点
};

class BaseNode : public std::enable_shared_from_this<BaseNode> {
protected:
    NodeType nodeType;
    std::string name;
    std::string value;
    std::map<std::string, std::string> attributes;
    std::vector<std::shared_ptr<BaseNode>> children;
    std::weak_ptr<BaseNode> parent;
    
    // 位置信息
    size_t line;
    size_t column;
    size_t position;

public:
    BaseNode(NodeType type, const std::string& name = "", const std::string& value = "");
    virtual ~BaseNode() = default;
    
    // 基本属性访问
    NodeType getNodeType() const { return nodeType; }
    const std::string& getName() const { return name; }
    const std::string& getValue() const { return value; }
    
    void setName(const std::string& name) { this->name = name; }
    void setValue(const std::string& value) { this->value = value; }
    
    // 位置信息
    size_t getLine() const { return line; }
    size_t getColumn() const { return column; }
    size_t getPosition() const { return position; }
    
    void setPosition(size_t line, size_t column, size_t position) {
        this->line = line;
        this->column = column;
        this->position = position;
    }
    
    // 属性操作
    void setAttribute(const std::string& key, const std::string& value);
    std::string getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    void removeAttribute(const std::string& key);
    const std::map<std::string, std::string>& getAttributes() const { return attributes; }
    
    // 子节点操作
    void addChild(std::shared_ptr<BaseNode> child);
    void removeChild(std::shared_ptr<BaseNode> child);
    void insertChild(size_t index, std::shared_ptr<BaseNode> child);
    std::vector<std::shared_ptr<BaseNode>>& getChildren() { return children; }
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const { return children; }
    size_t getChildCount() const { return children.size(); }
    
    // 父节点操作
    std::shared_ptr<BaseNode> getParent() const;
    void setParent(std::shared_ptr<BaseNode> parent);
    
    // 查找子节点
    std::vector<std::shared_ptr<BaseNode>> findChildren(const std::string& name) const;
    std::shared_ptr<BaseNode> findFirstChild(const std::string& name) const;
    std::shared_ptr<BaseNode> findChildByAttribute(const std::string& attrName, const std::string& attrValue) const;
    
    // 遍历
    void traverse(std::function<void(std::shared_ptr<BaseNode>)> callback);
    void traverse(std::function<void(std::shared_ptr<BaseNode>)> callback) const;
    
    // 克隆
    virtual std::shared_ptr<BaseNode> clone() const;
    
    // 验证
    virtual bool isValid() const;
    
    // 转换为字符串
    virtual std::string toString() const;
    virtual std::string toHTML() const;
    
    // 调试信息
    virtual std::string getDebugInfo() const;
    
    // 比较操作
    bool operator==(const BaseNode& other) const;
    bool operator!=(const BaseNode& other) const;
};

} // namespace CHTL

#endif // CHTL_BASE_NODE_H