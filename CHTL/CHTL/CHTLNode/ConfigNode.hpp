#pragma once

#include "BaseNode.hpp"
#include <string>

namespace CHTL {

/**
 * @brief 配置节点
 * 
 * 表示配置定义
 */
class ConfigNode : public BaseNode {
public:
    ConfigNode(size_t line = 0, size_t column = 0);
    
    // 转换为 HTML
    std::string toHTML() const override;
    
    // 转换为字符串
    std::string toString() const override;
};

} // namespace CHTL