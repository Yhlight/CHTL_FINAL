#pragma once

#include "BaseNode.hpp"
#include <string>

namespace CHTL {

/**
 * @brief 原始嵌入节点
 * 
 * 表示原始嵌入内容
 */
class OriginNode : public BaseNode {
public:
    OriginNode(const std::string& originType, size_t line = 0, size_t column = 0);
    
    // 获取原始嵌入类型
    const std::string& getOriginType() const { return originType_; }
    
    // 设置原始嵌入类型
    void setOriginType(const std::string& type) { originType_ = type; }
    
    // 转换为 HTML
    std::string toHTML() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    std::string originType_;
};

} // namespace CHTL