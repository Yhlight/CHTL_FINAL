#pragma once

#include "BaseNode.hpp"
#include <string>

namespace CHTL {

/**
 * @brief 自定义节点
 * 
 * 表示自定义定义
 */
class CustomNode : public BaseNode {
public:
    CustomNode(const std::string& customType, size_t line = 0, size_t column = 0);
    
    // 获取自定义类型
    const std::string& getCustomType() const { return customType_; }
    
    // 设置自定义类型
    void setCustomType(const std::string& type) { customType_ = type; }
    
    // 转换为 HTML
    std::string toHTML() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    std::string customType_;
};

} // namespace CHTL