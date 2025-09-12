#pragma once

#include "CHTL/TemplateNode.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace CHTL {

/**
 * 自定义类型
 */
enum class CustomType {
    STYLE,          // 自定义样式组
    ELEMENT,        // 自定义元素
    VAR             // 自定义变量组
};

/**
 * 特例化操作类型
 */
enum class SpecializationType {
    DELETE_PROPERTY,        // 删除属性
    DELETE_INHERITANCE,     // 删除继承
    ADD_PROPERTY,           // 添加属性
    MODIFY_PROPERTY,        // 修改属性
    INSERT_ELEMENT,         // 插入元素
    DELETE_ELEMENT,         // 删除元素
    REPLACE_ELEMENT        // 替换元素
};

/**
 * 特例化操作
 */
struct SpecializationOperation {
    SpecializationType type;
    std::string target;
    std::string value;
    size_t index;           // 用于索引访问
    
    SpecializationOperation(SpecializationType t, const std::string& target, 
                           const std::string& value = "", size_t idx = 0)
        : type(t), target(target), value(value), index(idx) {}
};

/**
 * 自定义节点基类
 */
class CustomNode : public CHTLNode {
public:
    CustomNode(CustomType type, const std::string& name);
    
    // 自定义类型
    CustomType getCustomType() const;
    void setCustomType(CustomType type);
    
    // 自定义名称
    std::string getCustomName() const;
    void setCustomName(const std::string& name);
    
    // 特例化操作管理
    void addSpecialization(const SpecializationOperation& operation);
    std::vector<SpecializationOperation> getSpecializations() const;
    void clearSpecializations();
    
    // 继承管理
    void addInheritance(const std::string& templateName);
    std::vector<std::string> getInheritances() const;
    
    // 克隆
    std::shared_ptr<CHTLNode> clone() const override;
    
    // 验证
    bool validate() const override;
    
    // 生成HTML
    std::string generateHTML() const override;

protected:
    CustomType custom_type_;
    std::string custom_name_;
    std::vector<SpecializationOperation> specializations_;
    std::vector<std::string> inheritances_;
};

/**
 * 自定义样式组节点
 */
class CustomStyleNode : public CustomNode {
public:
    CustomStyleNode(const std::string& name);
    
    // CSS属性管理
    void addCSSProperty(const std::string& property, const std::string& value);
    std::string getCSSProperty(const std::string& property) const;
    bool hasCSSProperty(const std::string& property) const;
    void removeCSSProperty(const std::string& property);
    
    // 选择器管理
    void addSelector(const std::string& selector);
    std::vector<std::string> getSelectors() const;
    
    // 无值样式组支持
    void addUnvaluedProperty(const std::string& property);
    std::vector<std::string> getUnvaluedProperties() const;
    bool hasUnvaluedProperty(const std::string& property) const;
    
    // 应用自定义到样式节点
    void applyToStyleNode(std::shared_ptr<StyleNode> styleNode) const;
    
    std::shared_ptr<CHTLNode> clone() const override;
    bool validate() const override;
    std::string generateHTML() const override;

private:
    std::unordered_map<std::string, std::string> css_properties_;
    std::vector<std::string> selectors_;
    std::unordered_set<std::string> unvalued_properties_;
};

/**
 * 自定义元素节点
 */
class CustomElementNode : public CustomNode {
public:
    CustomElementNode(const std::string& name);
    
    // 索引访问支持
    std::shared_ptr<CHTLNode> getElementByIndex(size_t index) const;
    void setElementByIndex(size_t index, std::shared_ptr<CHTLNode> element);
    
    // 插入元素
    void insertElement(size_t position, std::shared_ptr<CHTLNode> element);
    void insertElementAfter(const std::string& selector, std::shared_ptr<CHTLNode> element);
    void insertElementBefore(const std::string& selector, std::shared_ptr<CHTLNode> element);
    void insertElementAtTop(std::shared_ptr<CHTLNode> element);
    void insertElementAtBottom(std::shared_ptr<CHTLNode> element);
    
    // 删除元素
    void deleteElement(const std::string& selector);
    void deleteElementByIndex(size_t index);
    void deleteElementByType(const std::string& elementType);
    
    // 替换元素
    void replaceElement(const std::string& selector, std::shared_ptr<CHTLNode> element);
    void replaceElementByIndex(size_t index, std::shared_ptr<CHTLNode> element);
    
    // 应用自定义到元素节点
    void applyToElementNode(std::shared_ptr<ElementNode> elementNode) const;
    
    std::shared_ptr<CHTLNode> clone() const override;
    bool validate() const override;
    std::string generateHTML() const override;

private:
    std::vector<std::shared_ptr<CHTLNode>> elements_;
    
    // 辅助函数
    size_t findElementIndex(const std::string& selector) const;
    void applySpecializations();
};

/**
 * 自定义变量组节点
 */
class CustomVarNode : public CustomNode {
public:
    CustomVarNode(const std::string& name);
    
    // 变量管理
    void addVariable(const std::string& name, const std::string& value);
    std::string getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    void removeVariable(const std::string& name);
    
    // 变量特例化
    void specializeVariable(const std::string& name, const std::string& value);
    
    // 应用变量
    std::string applyVariables(const std::string& content) const;
    
    std::shared_ptr<CHTLNode> clone() const override;
    bool validate() const override;
    std::string generateHTML() const override;

private:
    std::unordered_map<std::string, std::string> variables_;
    std::unordered_map<std::string, std::string> specializations_;
};

/**
 * 自定义管理器
 */
class CustomManager {
public:
    CustomManager();
    ~CustomManager();
    
    // 自定义注册
    void registerCustom(std::shared_ptr<CustomNode> customNode);
    void unregisterCustom(CustomType type, const std::string& name);
    
    // 自定义查询
    std::shared_ptr<CustomNode> getCustom(CustomType type, const std::string& name) const;
    bool hasCustom(CustomType type, const std::string& name) const;
    
    // 自定义应用
    void applyCustomStyle(const std::string& name, std::shared_ptr<StyleNode> styleNode) const;
    void applyCustomElement(const std::string& name, std::shared_ptr<ElementNode> elementNode) const;
    std::string applyCustomVar(const std::string& name, const std::string& content) const;
    
    // 特例化处理
    void processSpecializations();
    
    // 清理
    void clear();

private:
    std::unordered_map<std::string, std::shared_ptr<CustomNode>> custom_styles_;
    std::unordered_map<std::string, std::shared_ptr<CustomNode>> custom_elements_;
    std::unordered_map<std::string, std::shared_ptr<CustomNode>> custom_vars_;
    
    // 特例化处理
    void processStyleSpecializations(std::shared_ptr<CustomStyleNode> customStyle);
    void processElementSpecializations(std::shared_ptr<CustomElementNode> customElement);
    void processVarSpecializations(std::shared_ptr<CustomVarNode> customVar);
};

} // namespace CHTL