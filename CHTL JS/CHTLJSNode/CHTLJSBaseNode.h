#ifndef CHTLJSBASENODE_H
#define CHTLJSBASENODE_H

#include <string>
#include <vector>
#include <memory>

namespace CHTLJS {

/**
 * CHTL JS基础节点类
 * 所有CHTL JS AST节点的基类
 */
class CHTLJSBaseNode {
public:
    enum class NodeType {
        SCRIPT_LOADER,
        LISTEN,
        ANIMATE,
        ROUTER,
        VIR,
        INEVERAWAY,
        UTIL_THEN,
        PRINTMYLOVE,
        SELECTOR,
        RESPONSIVE_VALUE,
        EXPRESSION,
        STATEMENT,
        BLOCK
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

protected:
    NodeType type_;
    std::vector<std::shared_ptr<CHTLJSBaseNode>> children_;
};

} // namespace CHTLJS

#endif // CHTLJSBASENODE_H