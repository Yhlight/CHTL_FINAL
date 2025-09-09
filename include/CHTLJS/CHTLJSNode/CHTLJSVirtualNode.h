#pragma once

#include "CHTLJSBaseNode.h"

namespace CHTL {

/**
 * @brief CHTL JS虚对象节点
 * 表示CHTL JS虚对象定义
 */
class CHTLJSVirtualNode : public CHTLJSBaseNode {
public:
    CHTLJSVirtualNode();
    virtual ~CHTLJSVirtualNode();
    
    // 虚对象名称
    std::string getVirtualName() const;
    void setVirtualName(const std::string& virtualName);

private:
    std::string m_virtualName;
};

} // namespace CHTL