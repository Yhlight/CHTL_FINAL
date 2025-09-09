#pragma once

#include "BaseNode.h"

namespace CHTL {

class TextNode : public BaseNode {
public:
    TextNode(const std::string& content);
    
    std::string toHTML() const override;
    std::string toCSS() const override;
    std::string toJS() const override;
    
    std::shared_ptr<BaseNode> clone() const override;
    
    // 文本特定方法
    const std::string& getContent() const { return getName(); }
    void setContent(const std::string& content) { setName(content); }
    
private:
    std::string escapeHTML(const std::string& text) const;
};

} // namespace CHTL