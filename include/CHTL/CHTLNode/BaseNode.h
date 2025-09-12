#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <any>

namespace CHTL {

/**
 * @brief 节点类型枚举
 */
enum class NodeType {
    ELEMENT,        // 元素节点
    TEXT,           // 文本节点
    COMMENT,        // 注释节点
    TEMPLATE,       // 模板节点
    CUSTOM,         // 自定义节点
    STYLE,          // 样式节点
    SCRIPT,         // 脚本节点
    ORIGIN,         // 原始嵌入节点
    IMPORT,         // 导入节点
    CONFIG,         // 配置节点
    NAMESPACE,      // 命名空间节点
    MODULE,         // 模块节点
    OPERATOR,       // 操作符节点
    UNKNOWN         // 未知节点
};

/**
 * @brief 基础节点类
 * 
 * 所有CHTL节点的基类，提供通用的节点功能
 */
class BaseNode {
public:
    /**
     * @brief 构造函数
     * @param type 节点类型
     */
    explicit BaseNode(NodeType type);
    
    /**
     * @brief 虚析构函数
     */
    virtual ~BaseNode();
    
    /**
     * @brief 获取节点类型
     * @return 节点类型
     */
    NodeType getType() const;
    
    /**
     * @brief 设置节点名称
     * @param name 节点名称
     */
    void setName(const std::string& name);
    
    /**
     * @brief 获取节点名称
     * @return 节点名称
     */
    const std::string& getName() const;
    
    /**
     * @brief 设置节点值
     * @param value 节点值
     */
    void setValue(const std::string& value);
    
    /**
     * @brief 获取节点值
     * @return 节点值
     */
    const std::string& getValue() const;
    
    /**
     * @brief 添加子节点
     * @param child 子节点
     */
    void addChild(std::shared_ptr<BaseNode> child);
    
    /**
     * @brief 移除子节点
     * @param index 子节点索引
     * @return 是否移除成功
     */
    bool removeChild(size_t index);
    
    /**
     * @brief 获取子节点数量
     * @return 子节点数量
     */
    size_t getChildCount() const;
    
    /**
     * @brief 获取子节点
     * @param index 子节点索引
     * @return 子节点指针
     */
    std::shared_ptr<BaseNode> getChild(size_t index) const;
    
    /**
     * @brief 获取所有子节点
     * @return 子节点列表
     */
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const;
    
    /**
     * @brief 设置属性
     * @param key 属性键
     * @param value 属性值
     */
    void setAttribute(const std::string& key, const std::string& value);
    
    /**
     * @brief 获取属性
     * @param key 属性键
     * @param defaultValue 默认值
     * @return 属性值
     */
    std::string getAttribute(const std::string& key, const std::string& defaultValue = "") const;
    
    /**
     * @brief 检查属性是否存在
     * @param key 属性键
     * @return 是否存在
     */
    bool hasAttribute(const std::string& key) const;
    
    /**
     * @brief 移除属性
     * @param key 属性键
     * @return 是否移除成功
     */
    bool removeAttribute(const std::string& key);
    
    /**
     * @brief 获取所有属性
     * @return 属性映射
     */
    const std::unordered_map<std::string, std::string>& getAttributes() const;
    
    /**
     * @brief 设置元数据
     * @param key 元数据键
     * @param value 元数据值
     */
    void setMetadata(const std::string& key, const std::any& value);
    
    /**
     * @brief 获取元数据
     * @param key 元数据键
     * @return 元数据值
     */
    std::any getMetadata(const std::string& key) const;
    
    /**
     * @brief 检查元数据是否存在
     * @param key 元数据键
     * @return 是否存在
     */
    bool hasMetadata(const std::string& key) const;
    
    /**
     * @brief 设置位置信息
     * @param line 行号
     * @param column 列号
     */
    void setPosition(size_t line, size_t column);
    
    /**
     * @brief 获取行号
     * @return 行号
     */
    size_t getLine() const;
    
    /**
     * @brief 获取列号
     * @return 列号
     */
    size_t getColumn() const;
    
    /**
     * @brief 克隆节点
     * @return 克隆的节点
     */
    virtual std::shared_ptr<BaseNode> clone() const;
    
    /**
     * @brief 转换为字符串表示
     * @return 字符串表示
     */
    virtual std::string toString() const;
    
    /**
     * @brief 接受访问者
     * @param visitor 访问者
     */
    virtual void accept(class NodeVisitor& visitor) {}

protected:
    NodeType m_type;
    std::string m_name;
    std::string m_value;
    std::vector<std::shared_ptr<BaseNode>> m_children;
    std::unordered_map<std::string, std::string> m_attributes;
    std::unordered_map<std::string, std::any> m_metadata;
    size_t m_line;
    size_t m_column;
};

} // namespace CHTL