#ifndef CHTL_BASENODE_H
#define CHTL_BASENODE_H

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

/**
 * CHTL基础节点类
 * 所有CHTL AST节点的基类
 */
class CHTLBaseNode {
public:
    enum class NodeType {
        // 基本节点
        ELEMENT,        // HTML元素
        TEXT,           // 文本节点
        COMMENT,        // 注释节点
        
        // 模板节点
        TEMPLATE,       // 模板
        TEMPLATE_STYLE, // 样式组模板
        TEMPLATE_ELEMENT, // 元素模板
        TEMPLATE_VAR,   // 变量组模板
        
        // 自定义节点
        CUSTOM,         // 自定义
        CUSTOM_STYLE,   // 自定义样式组
        CUSTOM_ELEMENT, // 自定义元素
        CUSTOM_VAR,     // 自定义变量组
        
        // 原始嵌入节点
        ORIGIN,         // 原始嵌入
        ORIGIN_HTML,    // HTML原始嵌入
        ORIGIN_STYLE,   // CSS原始嵌入
        ORIGIN_JAVASCRIPT, // JS原始嵌入
        
        // 导入节点
        IMPORT,         // 导入
        IMPORT_HTML,    // HTML导入
        IMPORT_STYLE,   // CSS导入
        IMPORT_JAVASCRIPT, // JS导入
        IMPORT_CHTL,    // CHTL导入
        IMPORT_CJMOD,   // CJMOD导入
        
        // 命名空间节点
        NAMESPACE,      // 命名空间
        
        // 配置节点
        CONFIGURATION,  // 配置
        CONFIG_CONFIG,  // 命名配置
        
        // 样式和脚本节点
        STYLE,          // 样式块
        SCRIPT,         // 脚本块
        
        // 操作符节点
        DELETE,         // delete操作
        INSERT,         // insert操作
        USE,            // use操作
        
        // 属性节点
        ATTRIBUTE,      // 属性
        
        // 表达式节点
        EXPRESSION,     // 表达式
        
        // 块节点
        BLOCK,          // 代码块
        
        // 特殊节点
        SELECTOR,       // 选择器
        RESPONSIVE_VALUE, // 响应式值
        GENERATOR_COMMENT // 生成器注释
    };
    
    CHTLBaseNode(NodeType type);
    virtual ~CHTLBaseNode();
    
    NodeType getType() const;
    virtual std::string generateCode() const = 0;
    virtual std::string toString() const;
    
    // 子节点管理
    void addChild(std::shared_ptr<CHTLBaseNode> child);
    const std::vector<std::shared_ptr<CHTLBaseNode>>& getChildren() const;
    void clearChildren();
    
    // 属性管理
    void setAttribute(const std::string& name, const std::string& value);
    std::string getAttribute(const std::string& name) const;
    bool hasAttribute(const std::string& name) const;
    const std::map<std::string, std::string>& getAttributes() const;
    void clearAttributes();
    
    // 位置信息
    void setPosition(int line, int column);
    int getLine() const;
    int getColumn() const;

protected:
    NodeType type_;
    std::vector<std::shared_ptr<CHTLBaseNode>> children_;
    std::map<std::string, std::string> attributes_;
    int line_;
    int column_;
};

} // namespace CHTL

#endif // CHTL_BASENODE_H