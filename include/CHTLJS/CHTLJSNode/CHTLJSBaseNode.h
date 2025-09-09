#pragma once

#include <string>

namespace CHTL {

/**
 * @brief CHTL JS基础节点
 * 所有CHTL JS AST节点的基类
 */
class CHTLJSBaseNode {
public:
    CHTLJSBaseNode();
    virtual ~CHTLJSBaseNode();
    
    // 节点类型
    std::string getNodeType() const;
    void setNodeType(const std::string& nodeType);

private:
    std::string m_nodeType;
};

} // namespace CHTL