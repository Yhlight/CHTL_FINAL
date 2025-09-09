#pragma once

#include "CHTLJSBaseNode.h"

namespace CHTL {

/**
 * @brief CHTL JS动画节点
 * 表示CHTL JS动画定义
 */
class CHTLJSAnimationNode : public CHTLJSBaseNode {
public:
    CHTLJSAnimationNode();
    virtual ~CHTLJSAnimationNode();
    
    // 动画名称
    std::string getAnimationName() const;
    void setAnimationName(const std::string& animationName);

private:
    std::string m_animationName;
};

} // namespace CHTL