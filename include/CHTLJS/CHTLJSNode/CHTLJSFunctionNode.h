#pragma once

#include "CHTLJSBaseNode.h"

namespace CHTL {

/**
 * @brief CHTL JS函数节点
 * 表示CHTL JS函数定义
 */
class CHTLJSFunctionNode : public CHTLJSBaseNode {
public:
    CHTLJSFunctionNode();
    virtual ~CHTLJSFunctionNode();
    
    // 函数名称
    std::string getFunctionName() const;
    void setFunctionName(const std::string& functionName);

private:
    std::string m_functionName;
};

} // namespace CHTL