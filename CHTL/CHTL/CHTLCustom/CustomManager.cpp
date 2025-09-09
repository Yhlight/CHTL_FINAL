#include "CustomManager.hpp"
#include "CHTLNode/StyleNode.hpp"
#include "CHTLNode/ElementNode.hpp"
#include "CHTLNode/CustomNode.hpp"
#include "CHTLNode/CustomStyleNode.hpp"
#include "CHTLNode/CustomElementNode.hpp"
#include "CHTLNode/CustomVarNode.hpp"
#include <sstream>
#include <regex>

namespace CHTL {

CustomManager& CustomManager::getInstance() {
    static CustomManager instance;
    return instance;
}

void CustomManager::addCustom(const std::string& name, CustomType type, 
                             std::shared_ptr<BaseNode> definition) {
    auto customDef = std::make_shared<CustomDefinition>();
    customDef->type = type;
    customDef->name = name;
    customDef->definition = definition;
    customs_[name] = customDef;
}

std::shared_ptr<CustomManager::CustomDefinition> CustomManager::getCustom(const std::string& name) const {
    auto it = customs_.find(name);
    return (it != customs_.end()) ? it->second : nullptr;
}

bool CustomManager::hasCustom(const std::string& name) const {
    return customs_.find(name) != customs_.end();
}

std::shared_ptr<BaseNode> CustomManager::instantiateCustom(const std::string& name, 
                                                          const std::map<std::string, std::string>& parameters) const {
    auto customDef = getCustom(name);
    if (!customDef) {
        return nullptr;
    }
    
    // 深度复制定义
    auto instance = deepCopyNode(customDef->definition);
    
    // 应用特例化操作
    applyDeleteOperations(instance, customDef->deleteOperations);
    applyInsertOperations(instance, customDef->insertOperations);
    applyReplaceOperations(instance, customDef->replaceOperations);
    
    // 替换参数
    replaceParametersInNode(instance, parameters);
    
    switch (customDef->type) {
        case CustomType::STYLE_GROUP:
            return instantiateCustomStyleGroup(*customDef, parameters);
        case CustomType::ELEMENT:
            return instantiateCustomElement(*customDef, parameters);
        case CustomType::VARIABLE_GROUP:
            return instantiateCustomVariableGroup(*customDef, parameters);
        default:
            return instance;
    }
}

void CustomManager::addDeleteOperation(const std::string& name, const std::string& operation) {
    auto customDef = getCustom(name);
    if (customDef) {
        customDef->deleteOperations.push_back(operation);
    }
}

void CustomManager::addInsertOperation(const std::string& name, std::shared_ptr<BaseNode> operation) {
    auto customDef = getCustom(name);
    if (customDef) {
        customDef->insertOperations.push_back(operation);
    }
}

void CustomManager::addReplaceOperation(const std::string& name, const std::string& selector, 
                                       const std::string& replacement) {
    auto customDef = getCustom(name);
    if (customDef) {
        customDef->replaceOperations[selector] = replacement;
    }
}

std::vector<std::string> CustomManager::getAllCustomNames() const {
    std::vector<std::string> names;
    for (const auto& [name, customDef] : customs_) {
        names.push_back(name);
    }
    return names;
}

std::vector<std::string> CustomManager::getCustomNamesByType(CustomType type) const {
    std::vector<std::string> names;
    for (const auto& [name, customDef] : customs_) {
        if (customDef->type == type) {
            names.push_back(name);
        }
    }
    return names;
}

void CustomManager::clear() {
    customs_.clear();
}

void CustomManager::removeCustom(const std::string& name) {
    customs_.erase(name);
}

std::shared_ptr<BaseNode> CustomManager::instantiateCustomStyleGroup(
    const CustomDefinition& customDef,
    const std::map<std::string, std::string>& parameters) const {
    
    auto instance = deepCopyNode(customDef.definition);
    
    // 应用特例化操作
    applyDeleteOperations(instance, customDef.deleteOperations);
    applyInsertOperations(instance, customDef.insertOperations);
    applyReplaceOperations(instance, customDef.replaceOperations);
    
    return instance;
}

std::shared_ptr<BaseNode> CustomManager::instantiateCustomElement(
    const CustomDefinition& customDef,
    const std::map<std::string, std::string>& parameters) const {
    
    auto instance = deepCopyNode(customDef.definition);
    
    // 应用特例化操作
    applyDeleteOperations(instance, customDef.deleteOperations);
    applyInsertOperations(instance, customDef.insertOperations);
    applyReplaceOperations(instance, customDef.replaceOperations);
    
    return instance;
}

std::shared_ptr<BaseNode> CustomManager::instantiateCustomVariableGroup(
    const CustomDefinition& customDef,
    const std::map<std::string, std::string>& parameters) const {
    
    auto instance = deepCopyNode(customDef.definition);
    
    // 应用特例化操作
    applyDeleteOperations(instance, customDef.deleteOperations);
    applyInsertOperations(instance, customDef.insertOperations);
    applyReplaceOperations(instance, customDef.replaceOperations);
    
    return instance;
}

void CustomManager::applyDeleteOperations(std::shared_ptr<BaseNode> node, 
                                         const std::vector<std::string>& deleteOps) const {
    if (!node || deleteOps.empty()) return;
    
    // 根据节点类型应用删除操作
    switch (node->getType()) {
        case BaseNode::NodeType::STYLE: {
            auto styleNode = std::dynamic_pointer_cast<StyleNode>(node);
            if (styleNode) {
                auto properties = styleNode->getCSSProperties();
                for (const auto& deleteOp : deleteOps) {
                    properties.erase(deleteOp);
                }
                styleNode->setCSSProperties(properties);
            }
            break;
        }
        
        case BaseNode::NodeType::ELEMENT: {
            auto elementNode = std::dynamic_pointer_cast<ElementNode>(node);
            if (elementNode) {
                // 删除指定的子元素
                std::vector<std::shared_ptr<BaseNode>> newChildren;
                for (const auto& child : elementNode->getChildren()) {
                    bool shouldDelete = false;
                    for (const auto& deleteOp : deleteOps) {
                        if (child->getType() == BaseNode::NodeType::ELEMENT) {
                            auto childElement = std::dynamic_pointer_cast<ElementNode>(child);
                            if (childElement && childElement->getTagName() == deleteOp) {
                                shouldDelete = true;
                                break;
                            }
                        }
                    }
                    if (!shouldDelete) {
                        newChildren.push_back(child);
                    }
                }
                elementNode->setChildren(newChildren);
            }
            break;
        }
        
        default:
            // 递归处理子节点
            for (const auto& child : node->getChildren()) {
                applyDeleteOperations(child, deleteOps);
            }
            break;
    }
}

void CustomManager::applyInsertOperations(std::shared_ptr<BaseNode> node, 
                                         const std::vector<std::shared_ptr<BaseNode>>& insertOps) const {
    if (!node || insertOps.empty()) return;
    
    // 根据节点类型应用插入操作
    switch (node->getType()) {
        case BaseNode::NodeType::ELEMENT: {
            auto elementNode = std::dynamic_pointer_cast<ElementNode>(node);
            if (elementNode) {
                // 在元素末尾插入新节点
                for (const auto& insertOp : insertOps) {
                    elementNode->addChild(deepCopyNode(insertOp));
                }
            }
            break;
        }
        
        default:
            // 递归处理子节点
            for (const auto& child : node->getChildren()) {
                applyInsertOperations(child, insertOps);
            }
            break;
    }
}

void CustomManager::applyReplaceOperations(std::shared_ptr<BaseNode> node, 
                                          const std::map<std::string, std::string>& replaceOps) const {
    if (!node || replaceOps.empty()) return;
    
    // 根据节点类型应用替换操作
    switch (node->getType()) {
        case BaseNode::NodeType::STYLE: {
            auto styleNode = std::dynamic_pointer_cast<StyleNode>(node);
            if (styleNode) {
                auto properties = styleNode->getCSSProperties();
                for (const auto& [selector, replacement] : replaceOps) {
                    properties[selector] = replacement;
                }
                styleNode->setCSSProperties(properties);
            }
            break;
        }
        
        case BaseNode::NodeType::ELEMENT: {
            auto elementNode = std::dynamic_pointer_cast<ElementNode>(node);
            if (elementNode) {
                // 替换属性值
                for (const auto& [selector, replacement] : replaceOps) {
                    elementNode->setAttribute(selector, replacement);
                }
            }
            break;
        }
        
        default:
            // 递归处理子节点
            for (const auto& child : node->getChildren()) {
                applyReplaceOperations(child, replaceOps);
            }
            break;
    }
}

std::shared_ptr<BaseNode> CustomManager::deepCopyNode(std::shared_ptr<BaseNode> node) const {
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
        
        default:
            // 对于其他类型的节点，创建一个基本的副本
            return std::make_shared<BaseNode>(node->getType(), node->getLine(), node->getColumn());
    }
    
    return nullptr;
}

std::string CustomManager::replaceParameters(const std::string& content, 
                                            const std::map<std::string, std::string>& parameters) const {
    std::string result = content;
    
    for (const auto& [paramName, paramValue] : parameters) {
        // 使用正则表达式替换参数
        std::regex paramRegex("\\b" + paramName + "\\b");
        result = std::regex_replace(result, paramRegex, paramValue);
    }
    
    return result;
}

void CustomManager::replaceParametersInNode(std::shared_ptr<BaseNode> node, 
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