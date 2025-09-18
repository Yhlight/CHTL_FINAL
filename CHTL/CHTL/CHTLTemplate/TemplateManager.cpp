#include "TemplateManager.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include <sstream>

namespace CHTL {

TemplateManager::TemplateManager() {
}

void TemplateManager::registerTemplate(std::shared_ptr<TemplateNode> templateNode) {
    if (templateNode) {
        m_templates[templateNode->getTemplateName()] = templateNode;
    }
}

void TemplateManager::registerCustom(std::shared_ptr<CustomNode> customNode) {
    if (customNode) {
        m_customs[customNode->getCustomName()] = customNode;
    }
}

std::shared_ptr<TemplateNode> TemplateManager::findTemplate(const std::string& name) const {
    auto it = m_templates.find(name);
    return (it != m_templates.end()) ? it->second : nullptr;
}

std::shared_ptr<CustomNode> TemplateManager::findCustom(const std::string& name) const {
    auto it = m_customs.find(name);
    return (it != m_customs.end()) ? it->second : nullptr;
}

std::shared_ptr<BaseNode> TemplateManager::useTemplate(const std::string& name, 
                                                      const std::map<std::string, std::string>& parameters) const {
    auto templateNode = findTemplate(name);
    if (!templateNode) {
        return nullptr;
    }
    
    return cloneAndReplace(templateNode, parameters);
}

std::shared_ptr<BaseNode> TemplateManager::useCustom(const std::string& name, 
                                                    const std::map<std::string, std::string>& parameters) const {
    auto customNode = findCustom(name);
    if (!customNode) {
        return nullptr;
    }
    
    return cloneAndReplace(customNode, parameters);
}

std::shared_ptr<BaseNode> TemplateManager::inheritTemplate(std::shared_ptr<TemplateNode> child, 
                                                          std::shared_ptr<TemplateNode> parent) const {
    if (!child || !parent) {
        return child;
    }
    
    // 创建继承后的模板
    auto inherited = std::make_shared<TemplateNode>(child->getTemplateType(), child->getTemplateName());
    
    // 先添加父模板的内容
    for (const auto& childNode : parent->getChildren()) {
        inherited->addChild(cloneAndReplace(childNode, {}));
    }
    
    // 再添加子模板的内容（会覆盖重复的属性）
    for (const auto& childNode : child->getChildren()) {
        inherited->addChild(cloneAndReplace(childNode, {}));
    }
    
    return inherited;
}

std::shared_ptr<BaseNode> TemplateManager::inheritCustom(std::shared_ptr<CustomNode> child, 
                                                        std::shared_ptr<CustomNode> parent) const {
    if (!child || !parent) {
        return child;
    }
    
    // 创建继承后的自定义
    auto inherited = std::make_shared<CustomNode>(child->getCustomType(), child->getCustomName());
    
    // 先添加父自定义的内容
    for (const auto& childNode : parent->getChildren()) {
        inherited->addChild(cloneAndReplace(childNode, {}));
    }
    
    // 再添加子自定义的内容（会覆盖重复的属性）
    for (const auto& childNode : child->getChildren()) {
        inherited->addChild(cloneAndReplace(childNode, {}));
    }
    
    return inherited;
}

void TemplateManager::clear() {
    m_templates.clear();
    m_customs.clear();
}

std::shared_ptr<BaseNode> TemplateManager::cloneAndReplace(std::shared_ptr<BaseNode> node, 
                                                          const std::map<std::string, std::string>& parameters) const {
    if (!node) {
        return nullptr;
    }
    
    // 根据节点类型进行克隆
    switch (node->getType()) {
        case BaseNode::NodeType::ELEMENT: {
            auto elementNode = std::static_pointer_cast<ElementNode>(node);
            auto cloned = std::make_shared<ElementNode>(elementNode->getTagName());
            cloned->setSelfClosing(elementNode->isSelfClosing());
            
            // 复制属性
            for (const auto& attr : elementNode->getAttributes()) {
                std::string value = attr.second;
                // 替换参数
                for (const auto& param : parameters) {
                    std::string placeholder = "{" + param.first + "}";
                    size_t pos = 0;
                    while ((pos = value.find(placeholder, pos)) != std::string::npos) {
                        value.replace(pos, placeholder.length(), param.second);
                        pos += param.second.length();
                    }
                }
                cloned->setAttribute(attr.first, value);
            }
            
            // 克隆子节点
            for (const auto& child : elementNode->getChildren()) {
                cloned->addChild(cloneAndReplace(child, parameters));
            }
            
            return cloned;
        }
        case BaseNode::NodeType::TEXT: {
            auto textNode = std::static_pointer_cast<TextNode>(node);
            std::string value = textNode->getValue();
            
            // 替换参数
            for (const auto& param : parameters) {
                std::string placeholder = "{" + param.first + "}";
                size_t pos = 0;
                while ((pos = value.find(placeholder, pos)) != std::string::npos) {
                    value.replace(pos, placeholder.length(), param.second);
                    pos += param.second.length();
                }
            }
            
            return std::make_shared<TextNode>(value);
        }
        case BaseNode::NodeType::STYLE: {
            auto styleNode = std::static_pointer_cast<StyleNode>(node);
            auto cloned = std::make_shared<StyleNode>();
            cloned->setStyleType(styleNode->getStyleType());
            cloned->setSelector(styleNode->getSelector());
            
            // 复制样式属性
            for (const auto& prop : styleNode->getStyleProperties()) {
                std::string value = prop.second;
                
                // 替换参数
                for (const auto& param : parameters) {
                    std::string placeholder = "{" + param.first + "}";
                    size_t pos = 0;
                    while ((pos = value.find(placeholder, pos)) != std::string::npos) {
                        value.replace(pos, placeholder.length(), param.second);
                        pos += param.second.length();
                    }
                }
                
                cloned->addStyleProperty(prop.first, value);
            }
            
            return cloned;
        }
        default: {
            // 对于其他类型的节点，使用clone方法
            return node->clone();
        }
    }
}

void TemplateManager::mergeStyleProperties(std::shared_ptr<BaseNode> target, 
                                          std::shared_ptr<BaseNode> source) const {
    if (!target || !source || target->getType() != BaseNode::NodeType::STYLE || 
        source->getType() != BaseNode::NodeType::STYLE) {
        return;
    }
    
    auto targetStyle = std::static_pointer_cast<StyleNode>(target);
    auto sourceStyle = std::static_pointer_cast<StyleNode>(source);
    
    // 合并样式属性
    for (const auto& prop : sourceStyle->getStyleProperties()) {
        targetStyle->addStyleProperty(prop.first, prop.second);
    }
}

} // namespace CHTL