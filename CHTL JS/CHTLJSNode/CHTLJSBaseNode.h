#ifndef CHTLJS_BASENODE_H
#define CHTLJS_BASENODE_H

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTLJS {

/**
 * CHTL JS基础节点类
 * CHTL JS是独立的编程语言，不是JavaScript
 */
class CHTLJSBaseNode {
public:
    enum class NodeType {
        // CHTL JS特有节点类型
        SCRIPT_LOADER,      // ScriptLoader - 模块加载器
        LISTEN,             // Listen - 事件监听
        ANIMATE,            // Animate - 动画系统
        ROUTER,             // Router - 路由系统
        VIR,                // Vir - 虚拟对象
        UTIL_THEN,          // util.then - 工具链式操作
        PRINTMYLOVE,        // printMylove - 图像转ASCII
        INEVERAWAY,         // iNeverAway - 函数重载
        
        // CHTL JS表达式
        RESPONSIVE_GET,     // $变量名$ - 响应式获取
        RESPONSIVE_SET,     // $变量名$ = - 响应式设置
        CHTL_SELECTOR,      // {{选择器}} - CHTL选择器
        CHTL_RESPONSIVE,    // $JS变量名$ - CHTL响应式值
        
        // 传统表达式
        IDENTIFIER,         // 标识符
        LITERAL,            // 字面量
        BINARY_EXPRESSION,  // 二元表达式
        UNARY_EXPRESSION,   // 一元表达式
        ASSIGNMENT,         // 赋值表达式
        FUNCTION_CALL,      // 函数调用
        
        // 语句
        EXPRESSION_STATEMENT, // 表达式语句
        BLOCK_STATEMENT,    // 块语句
        IF_STATEMENT,       // 条件语句
        WHILE_STATEMENT,    // 循环语句
        FOR_STATEMENT,      // for循环
        RETURN_STATEMENT,   // 返回语句
        
        // 声明
        VARIABLE_DECLARATION, // 变量声明
        FUNCTION_DECLARATION, // 函数声明
        
        // 特殊
        COMMENT,            // 注释
        NEWLINE,            // 换行
        EOF_TOKEN           // 文件结束
    };
    
    CHTLJSBaseNode(NodeType type);
    virtual ~CHTLJSBaseNode();
    
    NodeType getType() const;
    virtual std::string generateCode() const = 0;
    virtual std::string toString() const;
    
    // 子节点管理
    void addChild(std::shared_ptr<CHTLJSBaseNode> child);
    const std::vector<std::shared_ptr<CHTLJSBaseNode>>& getChildren() const;
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
    std::vector<std::shared_ptr<CHTLJSBaseNode>> children_;
    std::map<std::string, std::string> attributes_;
    int line_;
    int column_;
};

} // namespace CHTLJS

#endif // CHTLJS_BASENODE_H