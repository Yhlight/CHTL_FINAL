#include "TemplateManager.hpp"
#include <algorithm>

namespace CHTL {

TemplateManager& TemplateManager::getInstance() {
    static TemplateManager instance;
    return instance;
}

// 样式组模板管理
void TemplateManager::addStyleTemplate(const std::string& name, std::shared_ptr<TemplateStyleNode> template_node) {
    styleTemplates_[name] = template_node;
}

std::shared_ptr<TemplateStyleNode> TemplateManager::getStyleTemplate(const std::string& name) const {
    auto it = styleTemplates_.find(name);
    return (it != styleTemplates_.end()) ? it->second : nullptr;
}

bool TemplateManager::hasStyleTemplate(const std::string& name) const {
    return styleTemplates_.find(name) != styleTemplates_.end();
}

std::vector<std::string> TemplateManager::getAllStyleTemplateNames() const {
    std::vector<std::string> names;
    for (const auto& pair : styleTemplates_) {
        names.push_back(pair.first);
    }
    return names;
}

// 元素模板管理
void TemplateManager::addElementTemplate(const std::string& name, std::shared_ptr<TemplateElementNode> template_node) {
    elementTemplates_[name] = template_node;
}

std::shared_ptr<TemplateElementNode> TemplateManager::getElementTemplate(const std::string& name) const {
    auto it = elementTemplates_.find(name);
    return (it != elementTemplates_.end()) ? it->second : nullptr;
}

bool TemplateManager::hasElementTemplate(const std::string& name) const {
    return elementTemplates_.find(name) != elementTemplates_.end();
}

std::vector<std::string> TemplateManager::getAllElementTemplateNames() const {
    std::vector<std::string> names;
    for (const auto& pair : elementTemplates_) {
        names.push_back(pair.first);
    }
    return names;
}

// 变量组模板管理
void TemplateManager::addVarTemplate(const std::string& name, std::shared_ptr<TemplateVarNode> template_node) {
    varTemplates_[name] = template_node;
}

std::shared_ptr<TemplateVarNode> TemplateManager::getVarTemplate(const std::string& name) const {
    auto it = varTemplates_.find(name);
    return (it != varTemplates_.end()) ? it->second : nullptr;
}

bool TemplateManager::hasVarTemplate(const std::string& name) const {
    return varTemplates_.find(name) != varTemplates_.end();
}

std::vector<std::string> TemplateManager::getAllVarTemplateNames() const {
    std::vector<std::string> names;
    for (const auto& pair : varTemplates_) {
        names.push_back(pair.first);
    }
    return names;
}

void TemplateManager::resolveInheritance() {
    // 解析样式组模板继承
    for (const auto& pair : styleTemplates_) {
        resolveStyleTemplateInheritance(pair.first);
    }
    
    // 解析元素模板继承
    for (const auto& pair : elementTemplates_) {
        resolveElementTemplateInheritance(pair.first);
    }
    
    // 解析变量组模板继承
    for (const auto& pair : varTemplates_) {
        resolveVarTemplateInheritance(pair.first);
    }
}

std::string TemplateManager::resolveStyleTemplate(const std::string& templateName) const {
    auto template_node = getStyleTemplate(templateName);
    if (template_node) {
        return template_node->toCSS();
    }
    return "";
}

std::string TemplateManager::resolveElementTemplate(const std::string& templateName) const {
    auto template_node = getElementTemplate(templateName);
    if (template_node) {
        return template_node->toHTML();
    }
    return "";
}

std::string TemplateManager::resolveVarTemplate(const std::string& templateName, const std::string& variableName) const {
    auto template_node = getVarTemplate(templateName);
    if (template_node) {
        return template_node->getVariable(variableName);
    }
    return "";
}

void TemplateManager::clear() {
    styleTemplates_.clear();
    elementTemplates_.clear();
    varTemplates_.clear();
}

void TemplateManager::reset() {
    clear();
}

void TemplateManager::resolveStyleTemplateInheritance(const std::string& templateName) {
    auto template_node = getStyleTemplate(templateName);
    if (!template_node) return;
    
    // 处理继承的模板
    for (const auto& inheritedName : template_node->getInheritedTemplates()) {
        auto inheritedTemplate = getStyleTemplate(inheritedName);
        if (inheritedTemplate) {
            // 合并继承的 CSS 属性
            for (const auto& prop : inheritedTemplate->getCSSProperties()) {
                if (!template_node->hasCSSProperty(prop.first)) {
                    template_node->addCSSProperty(prop.first, prop.second);
                }
            }
        }
    }
}

void TemplateManager::resolveElementTemplateInheritance(const std::string& templateName) {
    auto template_node = getElementTemplate(templateName);
    if (!template_node) return;
    
    // 处理继承的模板
    for (const auto& inheritedName : template_node->getInheritedTemplates()) {
        auto inheritedTemplate = getElementTemplate(inheritedName);
        if (inheritedTemplate) {
            // 合并继承的子节点
            for (const auto& child : inheritedTemplate->getChildren()) {
                template_node->addChild(child);
            }
        }
    }
}

void TemplateManager::resolveVarTemplateInheritance(const std::string& templateName) {
    auto template_node = getVarTemplate(templateName);
    if (!template_node) return;
    
    // 处理继承的模板
    for (const auto& inheritedName : template_node->getInheritedTemplates()) {
        auto inheritedTemplate = getVarTemplate(inheritedName);
        if (inheritedTemplate) {
            // 合并继承的变量
            for (const auto& var : inheritedTemplate->getVariables()) {
                if (!template_node->hasVariable(var.first)) {
                    template_node->addVariable(var.first, var.second);
                }
            }
        }
    }
}

// 通用模板注册方法实现
void TemplateManager::registerTemplate(const std::string& name, std::shared_ptr<TemplateStyleNode> template_node) {
    styleTemplates_[name] = template_node;
}

void TemplateManager::registerTemplate(const std::string& name, std::shared_ptr<TemplateElementNode> template_node) {
    elementTemplates_[name] = template_node;
}

void TemplateManager::registerTemplate(const std::string& name, std::shared_ptr<TemplateVarNode> template_node) {
    varTemplates_[name] = template_node;
}

// 自定义模板注册方法实现
void TemplateManager::registerCustom(const std::string& name, std::shared_ptr<CustomStyleNode> custom_node) {
    customStyles_[name] = custom_node;
}

void TemplateManager::registerCustom(const std::string& name, std::shared_ptr<CustomElementNode> custom_node) {
    customElements_[name] = custom_node;
}

void TemplateManager::registerCustom(const std::string& name, std::shared_ptr<CustomVarNode> custom_node) {
    customVars_[name] = custom_node;
}

// 自定义模板获取方法实现
std::shared_ptr<CustomStyleNode> TemplateManager::getCustomStyle(const std::string& name) const {
    auto it = customStyles_.find(name);
    return (it != customStyles_.end()) ? it->second : nullptr;
}

std::shared_ptr<CustomElementNode> TemplateManager::getCustomElement(const std::string& name) const {
    auto it = customElements_.find(name);
    return (it != customElements_.end()) ? it->second : nullptr;
}

std::shared_ptr<CustomVarNode> TemplateManager::getCustomVar(const std::string& name) const {
    auto it = customVars_.find(name);
    return (it != customVars_.end()) ? it->second : nullptr;
}

} // namespace CHTL