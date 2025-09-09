#include "CHTL/CHTLCustom.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

// Specialization 实现
Specialization::Specialization(SpecializationType type, const std::string& target, 
                               const std::string& content, int line, int column)
    : m_type(type), m_target(target), m_content(content), m_line(line), m_column(column) {
}

// Custom 基类实现
Custom::Custom(CustomType type, const std::string& name, int line, int column)
    : Template(static_cast<TemplateType>(type), name, line, column), m_customType(type) {
}

void Custom::addSpecialization(std::shared_ptr<Specialization> spec) {
    if (spec) {
        m_specializations.push_back(spec);
    }
}

const std::vector<std::shared_ptr<Specialization>>& Custom::getSpecializations() const {
    return m_specializations;
}

void Custom::clearSpecializations() {
    m_specializations.clear();
}

void Custom::addUnvaluedAttribute(const std::string& attribute) {
    if (std::find(m_unvaluedAttributes.begin(), m_unvaluedAttributes.end(), attribute) == m_unvaluedAttributes.end()) {
        m_unvaluedAttributes.push_back(attribute);
    }
}

const std::vector<std::string>& Custom::getUnvaluedAttributes() const {
    return m_unvaluedAttributes;
}

bool Custom::hasUnvaluedAttribute(const std::string& attribute) const {
    return std::find(m_unvaluedAttributes.begin(), m_unvaluedAttributes.end(), attribute) != m_unvaluedAttributes.end();
}

// CustomStyle 实现
CustomStyle::CustomStyle(const std::string& name, int line, int column)
    : Custom(CustomType::STYLE, name, line, column) {
}

void CustomStyle::accept(CustomVisitor& visitor) {
    visitor.visit(*this);
}

// CustomElement 实现
CustomElement::CustomElement(const std::string& name, int line, int column)
    : Custom(CustomType::ELEMENT, name, line, column) {
}

std::shared_ptr<BaseNode> CustomElement::getElementByIndex(size_t index) const {
    if (index < m_children.size()) {
        return m_children[index];
    }
    return nullptr;
}

void CustomElement::setElementByIndex(size_t index, std::shared_ptr<BaseNode> element) {
    if (index < m_children.size()) {
        m_children[index] = element;
    } else if (index == m_children.size()) {
        m_children.push_back(element);
    }
}

void CustomElement::insertElement(size_t position, std::shared_ptr<BaseNode> element) {
    if (position <= m_children.size()) {
        m_children.insert(m_children.begin() + position, element);
    }
}

void CustomElement::removeElement(size_t index) {
    if (index < m_children.size()) {
        m_children.erase(m_children.begin() + index);
    }
}

void CustomElement::removeElement(const std::string& selector) {
    // 根据选择器移除元素
    // 这里需要实现选择器匹配逻辑
    m_children.erase(
        std::remove_if(m_children.begin(), m_children.end(),
            [&selector](const std::shared_ptr<BaseNode>& node) {
                // 简化的选择器匹配
                if (node->getType() == NodeType::ELEMENT) {
                    auto elementNode = std::static_pointer_cast<ElementNode>(node);
                    return elementNode->getTagName() == selector;
                }
                return false;
            }),
        m_children.end()
    );
}

void CustomElement::accept(CustomVisitor& visitor) {
    visitor.visit(*this);
}

// CustomVar 实现
CustomVar::CustomVar(const std::string& name, int line, int column)
    : Custom(CustomType::VAR, name, line, column) {
}

void CustomVar::accept(CustomVisitor& visitor) {
    visitor.visit(*this);
}

// CustomManager 实现
CustomManager::CustomManager(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

void CustomManager::registerCustom(std::shared_ptr<Custom> custom) {
    if (custom) {
        registerCustom(custom->getName(), custom);
    }
}

void CustomManager::registerCustom(const std::string& name, std::shared_ptr<Custom> custom) {
    if (custom) {
        m_customs[name] = custom;
        m_customsByType[custom->getCustomType()].push_back(custom);
    }
}

std::shared_ptr<Custom> CustomManager::getCustom(const std::string& name) const {
    auto it = m_customs.find(name);
    return (it != m_customs.end()) ? it->second : nullptr;
}

std::shared_ptr<Custom> CustomManager::getCustom(const std::string& name, CustomType type) const {
    auto custom = getCustom(name);
    if (custom && custom->getCustomType() == type) {
        return custom;
    }
    return nullptr;
}

std::vector<std::shared_ptr<Custom>> CustomManager::getCustoms(CustomType type) const {
    auto it = m_customsByType.find(type);
    return (it != m_customsByType.end()) ? it->second : std::vector<std::shared_ptr<Custom>>();
}

std::vector<std::shared_ptr<Custom>> CustomManager::getAllCustoms() const {
    std::vector<std::shared_ptr<Custom>> allCustoms;
    for (const auto& [name, custom] : m_customs) {
        allCustoms.push_back(custom);
    }
    return allCustoms;
}

void CustomManager::processSpecializations() {
    for (const auto& [name, custom] : m_customs) {
        processCustomSpecializations(custom);
    }
}

std::string CustomManager::applySpecializations(const std::string& customName, 
                                              const std::unordered_map<std::string, std::string>& variables) const {
    auto custom = getCustom(customName);
    if (!custom) {
        return "";
    }
    
    switch (custom->getCustomType()) {
        case CustomType::STYLE:
            return applyStyleSpecializations(std::static_pointer_cast<CustomStyle>(custom), variables);
        case CustomType::ELEMENT:
            return applyElementSpecializations(std::static_pointer_cast<CustomElement>(custom), variables);
        case CustomType::VAR:
            return applyVarSpecializations(std::static_pointer_cast<CustomVar>(custom), variables);
        default:
            return "";
    }
}

void CustomManager::clear() {
    m_customs.clear();
    m_customsByType.clear();
}

void CustomManager::removeCustom(const std::string& name) {
    auto custom = getCustom(name);
    if (custom) {
        m_customs.erase(name);
        
        auto& typeCustoms = m_customsByType[custom->getCustomType()];
        typeCustoms.erase(
            std::remove(typeCustoms.begin(), typeCustoms.end(), custom),
            typeCustoms.end()
        );
    }
}

void CustomManager::processCustomSpecializations(std::shared_ptr<Custom> custom) {
    for (const auto& spec : custom->getSpecializations()) {
        switch (spec->getType()) {
            case SpecializationType::DELETE:
                // 处理删除操作
                if (custom->getCustomType() == CustomType::ELEMENT) {
                    auto elementCustom = std::static_pointer_cast<CustomElement>(custom);
                    elementCustom->removeElement(spec->getTarget());
                } else if (custom->getCustomType() == CustomType::STYLE) {
                    auto styleCustom = std::static_pointer_cast<CustomStyle>(custom);
                    styleCustom->setAttribute(spec->getTarget(), "");
                }
                break;
                
            case SpecializationType::INSERT:
                // 处理插入操作
                if (custom->getCustomType() == CustomType::ELEMENT) {
                    auto elementCustom = std::static_pointer_cast<CustomElement>(custom);
                    // 这里需要根据具体语法解析插入的内容
                    // 暂时跳过具体实现
                }
                break;
                
            case SpecializationType::REPLACE:
                // 处理替换操作
                if (custom->getCustomType() == CustomType::STYLE) {
                    auto styleCustom = std::static_pointer_cast<CustomStyle>(custom);
                    styleCustom->setAttribute(spec->getTarget(), spec->getContent());
                }
                break;
                
            case SpecializationType::MODIFY:
                // 处理修改操作
                if (custom->getCustomType() == CustomType::STYLE) {
                    auto styleCustom = std::static_pointer_cast<CustomStyle>(custom);
                    styleCustom->setAttribute(spec->getTarget(), spec->getContent());
                }
                break;
        }
    }
}

std::string CustomManager::applyStyleSpecializations(std::shared_ptr<CustomStyle> custom, 
                                                   const std::unordered_map<std::string, std::string>& variables) const {
    std::stringstream ss;
    
    ss << "." << custom->getName() << " {\n";
    
    for (const auto& [key, value] : custom->getAttributes()) {
        if (!value.empty()) { // 跳过被删除的属性
            // 替换变量
            std::string processedValue = value;
            for (const auto& [varName, varValue] : variables) {
                std::string placeholder = "{{" + varName + "}}";
                size_t pos = 0;
                while ((pos = processedValue.find(placeholder, pos)) != std::string::npos) {
                    processedValue.replace(pos, placeholder.length(), varValue);
                    pos += varValue.length();
                }
            }
            
            ss << "  " << key << ": " << processedValue << ";\n";
        }
    }
    
    ss << "}";
    return ss.str();
}

std::string CustomManager::applyElementSpecializations(std::shared_ptr<CustomElement> custom, 
                                                     const std::unordered_map<std::string, std::string>& variables) const {
    std::stringstream ss;
    
    ss << "<!-- Custom Element: " << custom->getName() << " -->\n";
    ss << "<div class=\"" << custom->getName() << "\">\n";
    
    for (const auto& child : custom->getChildren()) {
        // 这里需要根据具体的节点类型生成相应的 HTML
        // 暂时跳过具体实现
        ss << "  <!-- Element content -->\n";
    }
    
    ss << "</div>";
    return ss.str();
}

std::string CustomManager::applyVarSpecializations(std::shared_ptr<CustomVar> custom, 
                                                 const std::unordered_map<std::string, std::string>& variables) const {
    std::stringstream ss;
    
    ss << "var " << custom->getName() << " = {\n";
    
    bool first = true;
    for (const auto& [key, value] : custom->getAttributes()) {
        if (!first) {
            ss << ",\n";
        }
        
        // 替换变量
        std::string processedValue = value;
        for (const auto& [varName, varValue] : variables) {
            std::string placeholder = "{{" + varName + "}}";
            size_t pos = 0;
            while ((pos = processedValue.find(placeholder, pos)) != std::string::npos) {
                processedValue.replace(pos, placeholder.length(), varValue);
                pos += varValue.length();
            }
        }
        
        ss << "  " << key << ": \"" << processedValue << "\"";
        first = false;
    }
    
    ss << "\n};";
    return ss.str();
}

// CustomParser 实现
CustomParser::CustomParser(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

std::shared_ptr<Custom> CustomParser::parseCustom(std::shared_ptr<CustomNode> node) {
    if (!node) {
        return nullptr;
    }
    
    switch (node->getType()) {
        case NodeType::CUSTOM_STYLE:
            return parseCustomStyle(node);
        case NodeType::CUSTOM_ELEMENT:
            return parseCustomElement(node);
        case NodeType::CUSTOM_VAR:
            return parseCustomVar(node);
        default:
            m_context->addError("Unknown custom type");
            return nullptr;
    }
}

std::vector<std::shared_ptr<Custom>> CustomParser::parseCustoms(std::shared_ptr<BaseNode> root) {
    std::vector<std::shared_ptr<Custom>> customs;
    
    if (!root) {
        return customs;
    }
    
    // 递归查找自定义节点
    std::function<void(std::shared_ptr<BaseNode>)> findCustoms = 
        [&](std::shared_ptr<BaseNode> node) {
            if (node->getType() == NodeType::CUSTOM_STYLE ||
                node->getType() == NodeType::CUSTOM_ELEMENT ||
                node->getType() == NodeType::CUSTOM_VAR) {
                auto customNode = std::static_pointer_cast<CustomNode>(node);
                auto custom = parseCustom(customNode);
                if (custom) {
                    customs.push_back(custom);
                }
            }
            
            for (const auto& child : node->getChildren()) {
                findCustoms(child);
            }
        };
    
    findCustoms(root);
    return customs;
}

std::vector<std::shared_ptr<Specialization>> CustomParser::parseSpecializations(std::shared_ptr<CustomNode> node) {
    std::vector<std::shared_ptr<Specialization>> specializations;
    
    for (const auto& child : node->getChildren()) {
        auto spec = parseSpecialization(child);
        if (spec) {
            specializations.push_back(spec);
        }
    }
    
    return specializations;
}

std::shared_ptr<CustomStyle> CustomParser::parseCustomStyle(std::shared_ptr<CustomNode> node) {
    auto customStyle = std::make_shared<CustomStyle>(
        node->getName(), node->getLine(), node->getColumn());
    
    // 解析属性
    for (const auto& [key, value] : node->getAttributes()) {
        customStyle->setAttribute(key, value);
    }
    
    // 解析特例化操作
    auto specializations = parseSpecializations(node);
    for (const auto& spec : specializations) {
        customStyle->addSpecialization(spec);
    }
    
    return customStyle;
}

std::shared_ptr<CustomElement> CustomParser::parseCustomElement(std::shared_ptr<CustomNode> node) {
    auto customElement = std::make_shared<CustomElement>(
        node->getName(), node->getLine(), node->getColumn());
    
    // 解析子节点
    for (const auto& child : node->getChildren()) {
        // 这里需要根据具体的语法规则来解析自定义元素的内容
        // 暂时跳过具体实现
    }
    
    // 解析特例化操作
    auto specializations = parseSpecializations(node);
    for (const auto& spec : specializations) {
        customElement->addSpecialization(spec);
    }
    
    return customElement;
}

std::shared_ptr<CustomVar> CustomParser::parseCustomVar(std::shared_ptr<CustomNode> node) {
    auto customVar = std::make_shared<CustomVar>(
        node->getName(), node->getLine(), node->getColumn());
    
    // 解析属性
    for (const auto& [key, value] : node->getAttributes()) {
        customVar->setAttribute(key, value);
    }
    
    // 解析特例化操作
    auto specializations = parseSpecializations(node);
    for (const auto& spec : specializations) {
        customVar->addSpecialization(spec);
    }
    
    return customVar;
}

std::shared_ptr<Specialization> CustomParser::parseSpecialization(std::shared_ptr<BaseNode> node) {
    if (node->getType() == NodeType::OPERATOR) {
        auto opNode = std::static_pointer_cast<OperatorNode>(node);
        
        switch (opNode->getOperatorType()) {
            case OperatorNode::OperatorType::DELETE:
                return std::make_shared<Specialization>(
                    SpecializationType::DELETE, "", "", opNode->getLine(), opNode->getColumn());
            case OperatorNode::OperatorType::INSERT:
                return std::make_shared<Specialization>(
                    SpecializationType::INSERT, "", "", opNode->getLine(), opNode->getColumn());
            default:
                return nullptr;
        }
    }
    
    return nullptr;
}

// CustomGenerator 实现
CustomGenerator::CustomGenerator(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

std::string CustomGenerator::generate(std::shared_ptr<Custom> custom) {
    m_output.clear();
    
    if (custom) {
        custom->accept(*this);
    }
    
    return m_output;
}

std::string CustomGenerator::generateAll(const std::vector<std::shared_ptr<Custom>>& customs) {
    m_output.clear();
    
    for (const auto& custom : customs) {
        if (custom) {
            custom->accept(*this);
            m_output += "\n";
        }
    }
    
    return m_output;
}

void CustomGenerator::visit(CustomStyle& custom) {
    m_output += generateCustomStyle(custom);
}

void CustomGenerator::visit(CustomElement& custom) {
    m_output += generateCustomElement(custom);
}

void CustomGenerator::visit(CustomVar& custom) {
    m_output += generateCustomVar(custom);
}

std::string CustomGenerator::generateCustomStyle(CustomStyle& custom) {
    std::stringstream ss;
    
    ss << "." << custom.getName() << " {\n";
    
    for (const auto& [key, value] : custom.getAttributes()) {
        if (!value.empty()) {
            ss << "  " << key << ": " << value << ";\n";
        }
    }
    
    ss << "}";
    return ss.str();
}

std::string CustomGenerator::generateCustomElement(CustomElement& custom) {
    std::stringstream ss;
    
    ss << "<!-- Custom Element: " << custom.getName() << " -->\n";
    ss << "<div class=\"" << custom.getName() << "\">\n";
    ss << "  <!-- Element content -->\n";
    ss << "</div>";
    
    return ss.str();
}

std::string CustomGenerator::generateCustomVar(CustomVar& custom) {
    std::stringstream ss;
    
    ss << "var " << custom.getName() << " = {\n";
    
    bool first = true;
    for (const auto& [key, value] : custom.getAttributes()) {
        if (!first) {
            ss << ",\n";
        }
        ss << "  " << key << ": \"" << value << "\"";
        first = false;
    }
    
    ss << "\n};";
    return ss.str();
}

std::string CustomGenerator::generateSpecializations(const std::vector<std::shared_ptr<Specialization>>& specializations) {
    std::stringstream ss;
    
    for (const auto& spec : specializations) {
        switch (spec->getType()) {
            case SpecializationType::DELETE:
                ss << "/* Deleted: " << spec->getTarget() << " */\n";
                break;
            case SpecializationType::INSERT:
                ss << "/* Inserted: " << spec->getContent() << " */\n";
                break;
            case SpecializationType::REPLACE:
                ss << "/* Replaced: " << spec->getTarget() << " -> " << spec->getContent() << " */\n";
                break;
            case SpecializationType::MODIFY:
                ss << "/* Modified: " << spec->getTarget() << " -> " << spec->getContent() << " */\n";
                break;
        }
    }
    
    return ss.str();
}

} // namespace CHTL