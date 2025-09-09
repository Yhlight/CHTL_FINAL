#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "Token.h"

namespace CHTL {

// 前向声明
class CHTLContext;

enum class NodeType {
    // 基础节点
    ELEMENT,        // HTML元素
    TEXT,           // 文本节点
    COMMENT,        // 注释节点
    
    // 模板节点
    TEMPLATE_STYLE, // 样式组模板
    TEMPLATE_ELEMENT, // 元素模板
    TEMPLATE_VAR,   // 变量组模板
    
    // 自定义节点
    CUSTOM_STYLE,   // 自定义样式组
    CUSTOM_ELEMENT, // 自定义元素
    CUSTOM_VAR,     // 自定义变量组
    
    // 特殊节点
    STYLE,          // 样式块
    SCRIPT,         // 脚本块
    ORIGIN,         // 原始嵌入
    IMPORT,         // 导入
    NAMESPACE,      // 命名空间
    CONFIGURATION,  // 配置
    OPERATOR        // 操作符节点
};

class BaseNode {
protected:
    NodeType nodeType;
    std::string name;
    std::map<std::string, std::string> attributes;
    std::vector<std::shared_ptr<BaseNode>> children;
    std::weak_ptr<BaseNode> parent;
    
    // 位置信息
    size_t line;
    size_t column;
    size_t position;
    
    // 内存优化：使用对象池
    static std::vector<std::unique_ptr<BaseNode>> nodePool;
    static size_t poolSize;
    static const size_t MAX_POOL_SIZE = 1000;
    
public:
    BaseNode(NodeType type, const std::string& name = "");
    virtual ~BaseNode() = default;
    
    // 基本属性
    NodeType getNodeType() const { return nodeType; }
    const std::string& getName() const { return name; }
    void setName(const std::string& name) { this->name = name; }
    
    // 属性管理
    void setAttribute(const std::string& key, const std::string& value);
    std::string getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    void removeAttribute(const std::string& key);
    const std::map<std::string, std::string>& getAttributes() const { return attributes; }
    
    // 子节点管理
    void addChild(std::shared_ptr<BaseNode> child);
    void removeChild(std::shared_ptr<BaseNode> child);
    void insertChild(size_t index, std::shared_ptr<BaseNode> child);
    std::vector<std::shared_ptr<BaseNode>>& getChildren() { return children; }
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const { return children; }
    size_t getChildCount() const { return children.size(); }
    std::shared_ptr<BaseNode> getChild(size_t index) const;
    
    // 父节点管理
    std::shared_ptr<BaseNode> getParent() const;
    void setParent(std::shared_ptr<BaseNode> parent);
    
    // 位置信息
    void setPosition(size_t line, size_t column, size_t position);
    size_t getLine() const { return line; }
    size_t getColumn() const { return column; }
    size_t getPosition() const { return position; }
    
    // 查找方法
    std::vector<std::shared_ptr<BaseNode>> findChildren(const std::string& name) const;
    std::shared_ptr<BaseNode> findFirstChild(const std::string& name) const;
    std::vector<std::shared_ptr<BaseNode>> findChildrenByType(NodeType type) const;
    
    // 遍历方法
    void traverse(std::function<void(std::shared_ptr<BaseNode>)> visitor);
    void traverse(std::function<void(std::shared_ptr<BaseNode>)> visitor) const;
    
    // 克隆方法
    virtual std::shared_ptr<BaseNode> clone() const;
    
    // 序列化方法
    virtual std::string toString() const;
    virtual std::string toHTML() const;
    virtual std::string toCSS() const;
    virtual std::string toJS() const;
    
    // 验证方法
    virtual bool isValid() const;
    
    // 访问者模式
    virtual void accept(class NodeVisitor& visitor);
    virtual void accept(const class NodeVisitor& visitor) const;
    
    // 调试方法
    virtual std::string debugString() const;
    
    // 内存管理
    static std::shared_ptr<BaseNode> createNode(NodeType type, const std::string& name = "");
    static void returnNode(std::shared_ptr<BaseNode> node);
    static void clearPool();
    static size_t getPoolSize() { return poolSize; }
    static size_t getMaxPoolSize() { return MAX_POOL_SIZE; }
    
    // 内存统计
    struct MemoryStats {
        size_t totalNodes;
        size_t poolNodes;
        size_t activeNodes;
        size_t memoryUsage;
    };
    
    static MemoryStats getMemoryStats();
};

// 访问者模式基类
class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;
    virtual void visit(std::shared_ptr<BaseNode> node) = 0;
    virtual void visit(const std::shared_ptr<BaseNode> node) const = 0;
};

// 常量访问者基类
class ConstNodeVisitor {
public:
    virtual ~ConstNodeVisitor() = default;
    virtual void visit(const std::shared_ptr<BaseNode> node) const = 0;
};

} // namespace CHTL