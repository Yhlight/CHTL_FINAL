#pragma once

#include "BaseNode.hpp"
#include <string>

namespace CHTL {

/**
 * @brief 模板节点
 * 
 * 表示模板定义
 */
class TemplateNode : public BaseNode {
public:
    TemplateNode(const std::string& templateType, size_t line = 0, size_t column = 0);
    
    // 获取模板类型
    const std::string& getTemplateType() const { return templateType_; }
    
    // 设置模板类型
    void setTemplateType(const std::string& type) { templateType_ = type; }
    
    // 转换为 HTML
    std::string toHTML() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    std::string templateType_;
};

} // namespace CHTL