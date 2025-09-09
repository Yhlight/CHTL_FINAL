#pragma once

#include "CHTLJSBaseNode.h"

namespace CHTL {

/**
 * @brief CHTL JS选择器节点
 * 表示CHTL JS选择器定义
 */
class CHTLJSSelectorNode : public CHTLJSBaseNode {
public:
    CHTLJSSelectorNode();
    virtual ~CHTLJSSelectorNode();
    
    // 选择器
    std::string getSelector() const;
    void setSelector(const std::string& selector);

private:
    std::string m_selector;
};

} // namespace CHTL