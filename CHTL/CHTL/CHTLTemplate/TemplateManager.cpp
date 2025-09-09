#include "TemplateManager.hpp"
#include "CHTLNode/StyleNode.hpp"
#include "CHTLNode/ElementNode.hpp"
#include "CHTLNode/TemplateNode.hpp"
#include <sstream>
#include <regex>

namespace CHTL {

TemplateManager& TemplateManager::getInstance() {
    static TemplateManager instance;
    return instance;
}

void TemplateManager::addTemplate(const std::string& name, TemplateType type, 
                                 std::shared_ptr<BaseNode> definition) {
    auto templateDef = std::make_shared<TemplateDefinition>();
    templateDef->type = type;
    templateDef->name = name;
    templateDef->definition = definition;
    templates_[name] = templateDef;
}

std::shared_ptr<TemplateManager::TemplateDefinition> TemplateManager::getTemplate(const std::string& name) const {
    auto it = templates_.find(name);
    return (it != templates_.end()) ? it->second : nullptr;
}

bool TemplateManager::hasTemplate(const std::string& name) const {
    return templates_.find(name) != templates_.end();
}

std::shared_ptr<BaseNode> TemplateManager::instantiateTemplate(const std::string& name, 
                                                              const std::map<std::string, std::string>& parameters) const {
    auto templateDef = getTemplate(name);
    if (!templateDef) {
        return nullptr;
    }
    
    switch (templateDef->type) {
        case TemplateType::STYLE_GROUP:
            return instantiateStyleGroupTemplate(*templateDef, parameters);
        case TemplateType::ELEMENT:
            return instantiateElementTemplate(*templateDef, parameters);
        case TemplateType::VARIABLE_GROUP:
            return instantiateVariableGroupTemplate(*templateDef, parameters);
        default:
            return nullptr;
    }
}

std::vector<std::string> TemplateManager::getAllTemplateNames() const {
    std::vector<std::string> names;
    for (const auto& [name, templateDef] : templates_) {
        names.push_back(name);
    }
    return names;
}

std::vector<std::string> TemplateManager::getTemplateNamesByType(TemplateType type) const {
    std::vector<std::string> names;
    for (const auto& [name, templateDef] : templates_) {
        if (templateDef->type == type) {
            names.push_back(name);
        }
    }
    return names;
}

void TemplateManager::clear() {
    templates_.clear();
}

void TemplateManager::removeTemplate(const std::string& name) {
    templates_.erase(name);
}

std::shared_ptr<BaseNode> TemplateManager::instantiateStyleGroupTemplate(
    const TemplateDefinition& templateDef,
    const std::map<std::string, std::string>& parameters) const {
    
    // 深度复制模板定义
    auto instance = deepCopyNode(templateDef.definition);
    
    // 如果是StyleNode，替换参数
    if (instance->getType() == BaseNode::NodeType::STYLE) {
        auto styleNode = std::dynamic_pointer_cast<StyleNode>(instance);
        if (styleNode) {
            // 替换CSS属性中的参数
            auto properties = styleNode->getCSSProperties();
            std::map<std::string, std::string> newProperties;
            
            for (const auto& [property, value] : properties) {
                newProperties[property] = replaceParameters(value, parameters);
            }
            
            styleNode->setCSSProperties(newProperties);
        }
    }
    
    return instance;
}

std::shared_ptr<BaseNode> TemplateManager::instantiateElementTemplate(
    const TemplateDefinition& templateDef,
    const std::map<std::string, std::string>& parameters) const {
    
    // 深度复制模板定义
    auto instance = deepCopyNode(templateDef.definition);
    
    // 递归替换所有子节点中的参数
    replaceParametersInNode(instance, parameters);
    
    return instance;
}

std::shared_ptr<BaseNode> TemplateManager::instantiateVariableGroupTemplate(
    const TemplateDefinition& templateDef,
    const std::map<std::string, std::string>& parameters) const {
    
    // 深度复制模板定义
    auto instance = deepCopyNode(templateDef.definition);
    
    // 如果是TemplateVarNode，替换变量值
    if (instance->getType() == BaseNode::NodeType::TEMPLATE) {
        auto templateNode = std::dynamic_pointer_cast<TemplateNode>(instance);
        if (templateNode && templateNode->getTemplateType() == TemplateNode::TemplateType::VAR) {
            auto varNode = std::dynamic_pointer_cast<TemplateVarNode>(templateNode);
            if (varNode) {
                // 替换变量值中的参数
                auto variables = varNode->getVariables();
                std::map<std::string, std::string> newVariables;
                
                for (const auto& [name, value] : variables) {
                    newVariables[name] = replaceParameters(value, parameters);
                }
                
                varNode->setVariables(newVariables);
            }
        }
    }
    
    return instance;
}

std::string TemplateManager::replaceParameters(const std::string& content, 
                                              const std::map<std::string, std::string>& parameters) const {
    std::string result = content;
    
    for (const auto& [paramName, paramValue] : parameters) {
        // 使用正则表达式替换参数
        std::regex paramRegex("\\b" + paramName + "\\b");
        result = std::regex_replace(result, paramRegex, paramValue);
    }
    
    return result;
}

std::shared_ptr<BaseNode> TemplateManager::deepCopyNode(std::shared_ptr<BaseNode> node) const {
    if (!node) {
        return nullptr;
    }
    
    // 根据节点类型创建新的节点
    switch (node->getType()) {
        case BaseNode::NodeType::ELEMENT: {
            auto elementNode = std::dynamic_pointer_cast<ElementNode>(node);
            if (elementNode) {
                auto newElement = std::make_shared<ElementNode>(
                    elementNode->getTagName(), 
                    elementNode->getLine(), 
                    elementNode->getColumn()
                );
                newElement->setSelfClosing(elementNode->isSelfClosing());
                
                // 复制属性
                for (const auto& [key, value] : elementNode->getAttributes()) {
                    newElement->setAttribute(key, value);
                }
                
                // 递归复制子节点
                for (const auto& child : elementNode->getChildren()) {
                    newElement->addChild(deepCopyNode(child));
                }
                
                return newElement;
            }
            break;
        }
        
        case BaseNode::NodeType::STYLE: {
            auto styleNode = std::dynamic_pointer_cast<StyleNode>(node);
            if (styleNode) {
                auto newStyle = std::make_shared<StyleNode>(
                    styleNode->getStyleType(),
                    styleNode->getLine(),
                    styleNode->getColumn()
                );
                newStyle->setCSSProperties(styleNode->getCSSProperties());
                return newStyle;
            }
            break;
        }
        
        case BaseNode::NodeType::TEXT: {
            auto textNode = std::dynamic_pointer_cast<TextNode>(node);
            if (textNode) {
                return std::make_shared<TextNode>(
                    textNode->getContent(),
                    textNode->getLine(),
                    textNode->getColumn()
                );
            }
            break;
        }
        
        case BaseNode::NodeType::TEMPLATE: {
            auto templateNode = std::dynamic_pointer_cast<TemplateNode>(node);
            if (templateNode) {
                auto newTemplate = std::make_shared<TemplateNode>(
                    templateNode->getTemplateType(),
                    templateNode->getLine(),
                    templateNode->getColumn()
                );
                
                // 根据模板类型进行特殊处理
                switch (templateNode->getTemplateType()) {
                    case TemplateNode::TemplateType::STYLE: {
                        auto styleTemplate = std::dynamic_pointer_cast<TemplateStyleNode>(templateNode);
                        if (styleTemplate) {
                            auto newStyleTemplate = std::make_shared<TemplateStyleNode>(
                                styleTemplate->getLine(),
                                styleTemplate->getColumn()
                            );
                            newStyleTemplate->setCSSProperties(styleTemplate->getCSSProperties());
                            return newStyleTemplate;
                        }
                        break;
                    }
                    
                    case TemplateNode::TemplateType::ELEMENT: {
                        auto elementTemplate = std::dynamic_pointer_cast<TemplateElementNode>(templateNode);
                        if (elementTemplate) {
                            auto newElementTemplate = std::make_shared<TemplateElementNode>(
                                elementTemplate->getLine(),
                                elementTemplate->getColumn()
                            );
                            
                            // 复制子节点
                            for (const auto& child : elementTemplate->getChildren()) {
                                newElementTemplate->addChild(deepCopyNode(child));
                            }
                            
                            return newElementTemplate;
                        }
                        break;
                    }
                    
                    case TemplateNode::TemplateType::VAR: {
                        auto varTemplate = std::dynamic_pointer_cast<TemplateVarNode>(templateNode);
                        if (varTemplate) {
                            auto newVarTemplate = std::make_shared<TemplateVarNode>(
                                varTemplate->getLine(),
                                varTemplate->getColumn()
                            );
                            newVarTemplate->setVariables(varTemplate->getVariables());
                            return newVarTemplate;
                        }
                        break;
                    }
                }
            }
            break;
        }
        
        default:
            // 对于其他类型的节点，创建一个基本的副本
            return std::make_shared<BaseNode>(node->getType(), node->getLine(), node->getColumn());
    }
    
    return nullptr;
}

void TemplateManager::replaceParametersInNode(std::shared_ptr<BaseNode> node, 
                                            const std::map<std::string, std::string>& parameters) const {
    if (!node) return;
    
    // 根据节点类型进行参数替换
    switch (node->getType()) {
        case BaseNode::NodeType::ELEMENT: {
            auto elementNode = std::dynamic_pointer_cast<ElementNode>(node);
            if (elementNode) {
                // 替换属性值
                for (const auto& [key, value] : elementNode->getAttributes()) {
                    elementNode->setAttribute(key, replaceParameters(value, parameters));
                }
                
                // 递归处理子节点
                for (const auto& child : elementNode->getChildren()) {
                    replaceParametersInNode(child, parameters);
                }
            }
            break;
        }
        
        case BaseNode::NodeType::STYLE: {
            auto styleNode = std::dynamic_pointer_cast<StyleNode>(node);
            if (styleNode) {
                // 替换CSS属性值
                auto properties = styleNode->getCSSProperties();
                std::map<std::string, std::string> newProperties;
                
                for (const auto& [property, value] : properties) {
                    newProperties[property] = replaceParameters(value, parameters);
                }
                
                styleNode->setCSSProperties(newProperties);
            }
            break;
        }
        
        case BaseNode::NodeType::TEXT: {
            auto textNode = std::dynamic_pointer_cast<TextNode>(node);
            if (textNode) {
                // 替换文本内容
                textNode->setContent(replaceParameters(textNode->getContent(), parameters));
            }
            break;
        }
        
        default:
            // 递归处理子节点
            for (const auto& child : node->getChildren()) {
                replaceParametersInNode(child, parameters);
            }
            break;
    }
}

} // namespace CHTL