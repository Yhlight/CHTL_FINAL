#ifndef STYLE_NODE_H
#define STYLE_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class StyleNode : public BaseNode {
public:
    StyleNode(const std::string& name = "style");
    ~StyleNode() = default;
    
    // 样式内容
    void setStyleContent(const std::string& content);
    const std::string& getStyleContent() const { return styleContent_; }
    
    // 样式类型
    void setStyleType(const std::string& type);
    const std::string& getStyleType() const { return styleType_; }
    
    // 重写基类方法
    std::string generateHTML() const override;
    std::string generateCSS() const override;
    std::string generateJS() const override;
    
private:
    std::string styleContent_;
    std::string styleType_;
};

} // namespace CHTL

#endif // STYLE_NODE_H