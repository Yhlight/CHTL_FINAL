#include "CHTLParser.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {

void CHTLParser::setTokens(const std::vector<CHTLToken>& tokenList) {
    tokens = tokenList;
    currentToken = 0;
    errors.clear();
    warnings.clear();
}

bool CHTLParser::hasErrors() const {
    return !errors.empty();
}

std::vector<std::string> CHTLParser::getErrors() const {
    return errors;
}

std::vector<std::string> CHTLParser::getWarnings() const {
    return warnings;
}

void CHTLParser::addTemplate(std::shared_ptr<TemplateNode> templateNode) {
    if (templateNode) {
        templates[templateNode->name] = templateNode;
    }
}

void CHTLParser::addCustom(std::shared_ptr<CustomNode> customNode) {
    if (customNode) {
        customs[customNode->name] = customNode;
    }
}

void CHTLParser::addConfiguration(std::shared_ptr<ConfigurationNode> configNode) {
    if (configNode) {
        configurations[configNode->name] = configNode;
    }
}

std::shared_ptr<TemplateNode> CHTLParser::getTemplate(const std::string& name) const {
    auto it = templates.find(name);
    return it != templates.end() ? it->second : nullptr;
}

std::shared_ptr<CustomNode> CHTLParser::getCustom(const std::string& name) const {
    auto it = customs.find(name);
    return it != customs.end() ? it->second : nullptr;
}

std::shared_ptr<ConfigurationNode> CHTLParser::getConfiguration(const std::string& name) const {
    auto it = configurations.find(name);
    return it != configurations.end() ? it->second : nullptr;
}

void CHTLParser::printAST(std::shared_ptr<CHTLNode> node, int depth) const {
    if (!node) return;
    
    std::string indent(depth * 2, ' ');
    std::cout << indent << node->toString() << std::endl;
    
    // 如果是元素节点，打印子节点
    if (node->type == CHTLNode::NodeType::ELEMENT) {
        auto element = std::static_pointer_cast<ElementNode>(node);
        for (const auto& child : element->children) {
            printAST(child, depth + 1);
        }
    }
    // 如果是模板节点，打印子节点
    else if (node->type == CHTLNode::NodeType::TEMPLATE) {
        auto templateNode = std::static_pointer_cast<TemplateNode>(node);
        for (const auto& child : templateNode->children) {
            printAST(child, depth + 1);
        }
    }
    // 如果是自定义节点，打印子节点
    else if (node->type == CHTLNode::NodeType::CUSTOM) {
        auto customNode = std::static_pointer_cast<CustomNode>(node);
        for (const auto& child : customNode->children) {
            printAST(child, depth + 1);
        }
    }
    // 如果是命名空间节点，打印子节点
    else if (node->type == CHTLNode::NodeType::NAMESPACE) {
        auto namespaceNode = std::static_pointer_cast<NamespaceNode>(node);
        for (const auto& child : namespaceNode->children) {
            printAST(child, depth + 1);
        }
    }
}

void CHTLParser::printErrors() const {
    for (const auto& error : errors) {
        std::cout << "错误: " << error << std::endl;
    }
}

void CHTLParser::printWarnings() const {
    for (const auto& warning : warnings) {
        std::cout << "警告: " << warning << std::endl;
    }
}

void CHTLParser::reset() {
    currentToken = 0;
    errors.clear();
    warnings.clear();
}


} // namespace CHTL