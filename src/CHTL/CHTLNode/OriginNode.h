#ifndef ORIGIN_NODE_H
#define ORIGIN_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class OriginNode : public BaseNode {
public:
    OriginNode(const std::string& name = "origin");
    ~OriginNode() = default;
    
    // 原始内容
    void setOriginContent(const std::string& content);
    const std::string& getOriginContent() const { return originContent_; }
    
    // 原始类型
    void setOriginType(const std::string& type);
    const std::string& getOriginType() const { return originType_; }
    
    // 重写基类方法
    std::string generateHTML() const override;
    std::string generateCSS() const override;
    std::string generateJS() const override;
    
private:
    std::string originContent_;
    std::string originType_;
};

} // namespace CHTL

#endif // ORIGIN_NODE_H