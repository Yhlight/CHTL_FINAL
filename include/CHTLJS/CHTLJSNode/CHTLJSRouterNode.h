#pragma once

#include "CHTLJSBaseNode.h"

namespace CHTL {

/**
 * @brief CHTL JS路由节点
 * 表示CHTL JS路由定义
 */
class CHTLJSRouterNode : public CHTLJSBaseNode {
public:
    CHTLJSRouterNode();
    virtual ~CHTLJSRouterNode();
    
    // 路由路径
    std::string getRoutePath() const;
    void setRoutePath(const std::string& routePath);

private:
    std::string m_routePath;
};

} // namespace CHTL