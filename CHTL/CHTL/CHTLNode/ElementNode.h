#pragma once

#include "BaseNode.h"

namespace CHTL {

class ElementNode : public BaseNode {
public:
    ElementNode(const std::string& tagName);
    
    std::string toHTML() const override;
    std::string toCSS() const override;
    std::string toJS() const override;
    
    std::shared_ptr<BaseNode> clone() const override;
    
    // 元素特定方法
    bool isSelfClosing() const;
    void setSelfClosing(bool selfClosing);
    
    // 获取标签名
    const std::string& getTagName() const { return getName(); }
    
private:
    bool self_closing_;
};

} // namespace CHTL