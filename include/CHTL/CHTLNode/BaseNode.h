#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

/**
 * @brief 访问者模式基类
 */
class Visitor;

/**
 * @brief AST 节点类型枚举
 */
enum class NodeType {
    ELEMENT,        ///< 元素节点
    TEXT,           ///< 文本节点
    COMMENT,        ///< 注释节点
    TEMPLATE,       ///< 模板节点
    CUSTOM,         ///< 自定义节点
    STYLE,          ///< 样式节点
    SCRIPT,         ///< 脚本节点
    ORIGIN,         ///< 原始嵌入节点
    IMPORT,         ///< 导入节点
    CONFIG,         ///< 配置节点
    NAMESPACE,      ///< 命名空间节点
    OPERATOR        ///< 操作符节点
};

/**
 * @brief AST 节点基类
 * 
 * 所有 AST 节点的基类，提供统一的接口
 */
class BaseNode {
public:
    /**
     * @brief 构造函数
     * @param type 节点类型
     * @param line 行号
     * @param column 列号
     */
    BaseNode(NodeType type, size_t line = 0, size_t column = 0);
    
    /**
     * @brief 虚析构函数
     */
    virtual ~BaseNode() = default;
    
    /**
     * @brief 接受访问者
     * @param visitor 访问者对象
     */
    virtual void accept(Visitor& visitor) = 0;
    
    /**
     * @brief 生成代码
     * @return 生成的代码
     */
    virtual std::string generate() = 0;
    
    /**
     * @brief 获取节点类型
     * @return 节点类型
     */
    NodeType getNodeType() const { return nodeType_; }
    
    /**
     * @brief 获取行号
     * @return 行号
     */
    size_t getLine() const { return line_; }
    
    /**
     * @brief 获取列号
     * @return 列号
     */
    size_t getColumn() const { return column_; }
    
    /**
     * @brief 设置行号
     * @param line 行号
     */
    void setLine(size_t line) { line_ = line; }
    
    /**
     * @brief 设置列号
     * @param column 列号
     */
    void setColumn(size_t column) { column_ = column; }
    
    /**
     * @brief 添加子节点
     * @param child 子节点
     */
    void addChild(std::shared_ptr<BaseNode> child);
    
    /**
     * @brief 获取子节点列表
     * @return 子节点列表
     */
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const { return children_; }
    
    /**
     * @brief 设置属性
     * @param key 属性键
     * @param value 属性值
     */
    void setAttribute(const std::string& key, const std::string& value);
    
    /**
     * @brief 获取属性
     * @param key 属性键
     * @return 属性值
     */
    std::string getAttribute(const std::string& key) const;
    
    /**
     * @brief 检查是否有属性
     * @param key 属性键
     * @return 是否有该属性
     */
    bool hasAttribute(const std::string& key) const;
    
    /**
     * @brief 获取所有属性
     * @return 属性映射
     */
    const std::map<std::string, std::string>& getAttributes() const { return attributes_; }
    
    /**
     * @brief 转换为字符串
     * @return 字符串表示
     */
    virtual std::string toString() const;

protected:
    NodeType nodeType_;                                    ///< 节点类型
    size_t line_;                                          ///< 行号
    size_t column_;                                        ///< 列号
    std::vector<std::shared_ptr<BaseNode>> children_;     ///< 子节点列表
    std::map<std::string, std::string> attributes_;       ///< 属性映射
};

/**
 * @brief 节点指针类型
 */
using NodePtr = std::shared_ptr<BaseNode>;

/**
 * @brief 节点列表类型
 */
using NodeList = std::vector<NodePtr>;

} // namespace CHTL