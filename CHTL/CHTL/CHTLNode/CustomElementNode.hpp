#pragma once

#include "BaseNode.hpp"
#include <string>
#include <vector>

namespace CHTL {

/**
 * @brief 自定义元素节点
 * 
 * 根据 CHTL.md 文档第 490-687 行实现
 * 支持自定义元素的特例化操作
 */
class CustomElementNode : public BaseNode {
public:
    CustomElementNode(const std::string& name, size_t line = 0, size_t column = 0);
    
    // 获取自定义名称
    const std::string& getCustomName() const { return customName_; }
    
    // 设置自定义名称
    void setCustomName(const std::string& name) { customName_ = name; }
    
    // 添加要删除的元素
    void addDeletedElement(const std::string& elementSelector);
    
    // 获取要删除的元素列表
    const std::vector<std::string>& getDeletedElements() const { return deletedElements_; }
    
    // 检查是否要删除元素
    bool isDeletedElement(const std::string& elementSelector) const;
    
    // 添加要删除的继承
    void addDeletedInheritance(const std::string& templateName);
    
    // 获取要删除的继承列表
    const std::vector<std::string>& getDeletedInheritances() const { return deletedInheritances_; }
    
    // 检查是否要删除继承
    bool isDeletedInheritance(const std::string& templateName) const;
    
    // 添加插入的元素
    void addInsertedElement(const std::string& position, const std::string& selector, 
                           std::shared_ptr<BaseNode> element);
    
    // 获取插入的元素列表
    const std::vector<std::pair<std::string, std::shared_ptr<BaseNode>>>& getInsertedElements() const { 
        return insertedElements_; 
    }
    
    // 添加继承的模板
    void addInheritedTemplate(const std::string& templateName);
    
    // 获取继承的模板列表
    const std::vector<std::string>& getInheritedTemplates() const { return inheritedTemplates_; }
    
    // 检查是否继承指定模板
    bool inheritsFrom(const std::string& templateName) const;
    
    // 应用特例化操作
    void applySpecialization();
    
    // 转换为 HTML
    std::string toHTML() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    std::string customName_;
    std::vector<std::string> deletedElements_;
    std::vector<std::string> deletedInheritances_;
    std::vector<std::pair<std::string, std::shared_ptr<BaseNode>>> insertedElements_;
    std::vector<std::string> inheritedTemplates_;
    
    // 生成元素内容
    std::string generateElementContent() const;
};

} // namespace CHTL