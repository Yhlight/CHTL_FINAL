#include "CHTL/CHTLTemplate.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

// Template 基类实现
Template::Template(TemplateType type, const std::string& name, int line, int column)
    : m_type(type), m_name(name), m_line(line), m_column(column) {
}

void Template::setAttribute(const std::string& key, const std::string& value) {
    m_attributes[key] = value;
}

std::string Template::getAttribute(const std::string& key) const {
    auto it = m_attributes.find(key);
    return (it != m_attributes.end()) ? it->second : "";
}

bool Template::hasAttribute(const std::string& key) const {
    return m_attributes.find(key) != m_attributes.end();
}

const std::unordered_map<std::string, std::string>& Template::getAttributes() const {
    return m_attributes;
}

void Template::addChild(std::shared_ptr<Template> child) {
    m_children.push_back(child);
}

const std::vector<std::shared_ptr<Template>>& Template::getChildren() const {
    return m_children;
}

void Template::addInheritance(const std::string& templateName) {
    m_inheritances.push_back(templateName);
}

const std::vector<std::string>& Template::getInheritances() const {
    return m_inheritances;
}

// StyleTemplate 实现
StyleTemplate::StyleTemplate(const std::string& name, int line, int column)
    : Template(TemplateType::STYLE, name, line, column) {
}

void StyleTemplate::accept(TemplateVisitor& visitor) {
    visitor.visit(*this);
}

// ElementTemplate 实现
ElementTemplate::ElementTemplate(const std::string& name, int line, int column)
    : Template(TemplateType::ELEMENT, name, line, column) {
}

void ElementTemplate::accept(TemplateVisitor& visitor) {
    visitor.visit(*this);
}

// VarTemplate 实现
VarTemplate::VarTemplate(const std::string& name, int line, int column)
    : Template(TemplateType::VAR, name, line, column) {
}

void VarTemplate::accept(TemplateVisitor& visitor) {
    visitor.visit(*this);
}

// TemplateManager 实现
TemplateManager::TemplateManager(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

void TemplateManager::registerTemplate(std::shared_ptr<Template> template_) {
    if (template_) {
        registerTemplate(template_->getName(), template_);
    }
}

void TemplateManager::registerTemplate(const std::string& name, std::shared_ptr<Template> template_) {
    if (template_) {
        m_templates[name] = template_;
        m_templatesByType[template_->getType()].push_back(template_);
    }
}

std::shared_ptr<Template> TemplateManager::getTemplate(const std::string& name) const {
    auto it = m_templates.find(name);
    return (it != m_templates.end()) ? it->second : nullptr;
}

std::shared_ptr<Template> TemplateManager::getTemplate(const std::string& name, TemplateType type) const {
    auto template_ = getTemplate(name);
    if (template_ && template_->getType() == type) {
        return template_;
    }
    return nullptr;
}

std::vector<std::shared_ptr<Template>> TemplateManager::getTemplates(TemplateType type) const {
    auto it = m_templatesByType.find(type);
    return (it != m_templatesByType.end()) ? it->second : std::vector<std::shared_ptr<Template>>();
}

std::vector<std::shared_ptr<Template>> TemplateManager::getAllTemplates() const {
    std::vector<std::shared_ptr<Template>> allTemplates;
    for (const auto& [name, template_] : m_templates) {
        allTemplates.push_back(template_);
    }
    return allTemplates;
}

void TemplateManager::resolveInheritances() {
    for (const auto& [name, template_] : m_templates) {
        resolveTemplateInheritance(template_);
    }
}

bool TemplateManager::hasCircularInheritance(const std::string& templateName, 
                                           const std::string& targetName) const {
    if (templateName == targetName) {
        return true;
    }
    
    auto template_ = getTemplate(templateName);
    if (!template_) {
        return false;
    }
    
    for (const auto& inheritance : template_->getInheritances()) {
        if (hasCircularInheritance(inheritance, targetName)) {
            return true;
        }
    }
    
    return false;
}

std::string TemplateManager::applyTemplate(const std::string& templateName, 
                                         const std::unordered_map<std::string, std::string>& variables) const {
    auto template_ = getTemplate(templateName);
    if (!template_) {
        return "";
    }
    
    switch (template_->getType()) {
        case TemplateType::STYLE:
            return applyStyleTemplate(std::static_pointer_cast<StyleTemplate>(template_), variables);
        case TemplateType::ELEMENT:
            return applyElementTemplate(std::static_pointer_cast<ElementTemplate>(template_), variables);
        case TemplateType::VAR:
            return applyVarTemplate(std::static_pointer_cast<VarTemplate>(template_), variables);
        default:
            return "";
    }
}

void TemplateManager::clear() {
    m_templates.clear();
    m_templatesByType.clear();
}

void TemplateManager::removeTemplate(const std::string& name) {
    auto template_ = getTemplate(name);
    if (template_) {
        m_templates.erase(name);
        
        auto& typeTemplates = m_templatesByType[template_->getType()];
        typeTemplates.erase(
            std::remove(typeTemplates.begin(), typeTemplates.end(), template_),
            typeTemplates.end()
        );
    }
}

void TemplateManager::resolveTemplateInheritance(std::shared_ptr<Template> template_) {
    for (const auto& inheritance : template_->getInheritances()) {
        auto parentTemplate = getTemplate(inheritance);
        if (parentTemplate) {
            // 检查循环继承
            if (hasCircularInheritance(template_->getName(), inheritance)) {
                m_context->addError("Circular inheritance detected: " + template_->getName() + 
                                  " -> " + inheritance);
                continue;
            }
            
            // 合并属性
            for (const auto& [key, value] : parentTemplate->getAttributes()) {
                if (!template_->hasAttribute(key)) {
                    template_->setAttribute(key, value);
                }
            }
            
            // 递归解析父模板的继承
            resolveTemplateInheritance(parentTemplate);
        } else {
            m_context->addWarning("Template not found: " + inheritance);
        }
    }
}

std::string TemplateManager::applyStyleTemplate(std::shared_ptr<StyleTemplate> template_, 
                                              const std::unordered_map<std::string, std::string>& variables) const {
    std::stringstream ss;
    
    // 生成 CSS 规则
    ss << "." << template_->getName() << " {\n";
    
    for (const auto& [key, value] : template_->getAttributes()) {
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
    
    ss << "}";
    return ss.str();
}

std::string TemplateManager::applyElementTemplate(std::shared_ptr<ElementTemplate> template_, 
                                                const std::unordered_map<std::string, std::string>& variables) const {
    std::stringstream ss;
    
    // 生成 HTML 元素
    ss << "<div class=\"" << template_->getName() << "\">\n";
    
    for (const auto& child : template_->getChildren()) {
        child->accept(*this);
    }
    
    ss << "</div>";
    return ss.str();
}

std::string TemplateManager::applyVarTemplate(std::shared_ptr<VarTemplate> template_, 
                                            const std::unordered_map<std::string, std::string>& variables) const {
    std::stringstream ss;
    
    // 生成变量定义
    ss << "var " << template_->getName() << " = {\n";
    
    bool first = true;
    for (const auto& [key, value] : template_->getAttributes()) {
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

// TemplateParser 实现
TemplateParser::TemplateParser(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

std::shared_ptr<Template> TemplateParser::parseTemplate(std::shared_ptr<TemplateNode> node) {
    if (!node) {
        return nullptr;
    }
    
    switch (node->getType()) {
        case NodeType::TEMPLATE_STYLE:
            return parseStyleTemplate(node);
        case NodeType::TEMPLATE_ELEMENT:
            return parseElementTemplate(node);
        case NodeType::TEMPLATE_VAR:
            return parseVarTemplate(node);
        default:
            m_context->addError("Unknown template type");
            return nullptr;
    }
}

std::vector<std::shared_ptr<Template>> TemplateParser::parseTemplates(std::shared_ptr<BaseNode> root) {
    std::vector<std::shared_ptr<Template>> templates;
    
    if (!root) {
        return templates;
    }
    
    // 递归查找模板节点
    std::function<void(std::shared_ptr<BaseNode>)> findTemplates = 
        [&](std::shared_ptr<BaseNode> node) {
            if (node->getType() == NodeType::TEMPLATE_STYLE ||
                node->getType() == NodeType::TEMPLATE_ELEMENT ||
                node->getType() == NodeType::TEMPLATE_VAR) {
                auto templateNode = std::static_pointer_cast<TemplateNode>(node);
                auto template_ = parseTemplate(templateNode);
                if (template_) {
                    templates.push_back(template_);
                }
            }
            
            for (const auto& child : node->getChildren()) {
                findTemplates(child);
            }
        };
    
    findTemplates(root);
    return templates;
}

void TemplateParser::parseInheritance(std::shared_ptr<Template> template_, std::shared_ptr<TemplateNode> node) {
    // 解析继承关系
    for (const auto& child : node->getChildren()) {
        if (child->getType() == NodeType::OPERATOR) {
            auto opNode = std::static_pointer_cast<OperatorNode>(child);
            if (opNode->getOperatorType() == OperatorNode::OperatorType::INHERIT) {
                // 处理 inherit 操作符
                // 这里需要根据具体的语法规则来解析
            }
        }
    }
}

std::shared_ptr<StyleTemplate> TemplateParser::parseStyleTemplate(std::shared_ptr<TemplateNode> node) {
    auto styleTemplate = std::make_shared<StyleTemplate>(
        node->getName(), node->getLine(), node->getColumn());
    
    // 解析属性
    for (const auto& [key, value] : node->getAttributes()) {
        styleTemplate->setAttribute(key, value);
    }
    
    // 解析子节点
    for (const auto& child : node->getChildren()) {
        if (child->getType() == NodeType::STYLE) {
            auto styleNode = std::static_pointer_cast<StyleNode>(child);
            for (const auto& [key, value] : styleNode->getAttributes()) {
                styleTemplate->setAttribute(key, value);
            }
        }
    }
    
    return styleTemplate;
}

std::shared_ptr<ElementTemplate> TemplateParser::parseElementTemplate(std::shared_ptr<TemplateNode> node) {
    auto elementTemplate = std::make_shared<ElementTemplate>(
        node->getName(), node->getLine(), node->getColumn());
    
    // 解析子节点
    for (const auto& child : node->getChildren()) {
        // 这里需要根据具体的语法规则来解析元素模板的内容
        // 暂时跳过具体实现
    }
    
    return elementTemplate;
}

std::shared_ptr<VarTemplate> TemplateParser::parseVarTemplate(std::shared_ptr<TemplateNode> node) {
    auto varTemplate = std::make_shared<VarTemplate>(
        node->getName(), node->getLine(), node->getColumn());
    
    // 解析属性
    for (const auto& [key, value] : node->getAttributes()) {
        varTemplate->setAttribute(key, value);
    }
    
    return varTemplate;
}

// TemplateGenerator 实现
TemplateGenerator::TemplateGenerator(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

std::string TemplateGenerator::generate(std::shared_ptr<Template> template_) {
    m_output.clear();
    
    if (template_) {
        template_->accept(*this);
    }
    
    return m_output;
}

std::string TemplateGenerator::generateAll(const std::vector<std::shared_ptr<Template>>& templates) {
    m_output.clear();
    
    for (const auto& template_ : templates) {
        if (template_) {
            template_->accept(*this);
            m_output += "\n";
        }
    }
    
    return m_output;
}

void TemplateGenerator::visit(StyleTemplate& template_) {
    m_output += generateStyleTemplate(template_);
}

void TemplateGenerator::visit(ElementTemplate& template_) {
    m_output += generateElementTemplate(template_);
}

void TemplateGenerator::visit(VarTemplate& template_) {
    m_output += generateVarTemplate(template_);
}

std::string TemplateGenerator::generateStyleTemplate(StyleTemplate& template_) {
    std::stringstream ss;
    
    ss << "." << template_.getName() << " {\n";
    
    for (const auto& [key, value] : template_.getAttributes()) {
        ss << "  " << key << ": " << value << ";\n";
    }
    
    ss << "}";
    return ss.str();
}

std::string TemplateGenerator::generateElementTemplate(ElementTemplate& template_) {
    std::stringstream ss;
    
    ss << "<!-- Element Template: " << template_.getName() << " -->\n";
    ss << "<div class=\"" << template_.getName() << "\">\n";
    ss << "  <!-- Template content -->\n";
    ss << "</div>";
    
    return ss.str();
}

std::string TemplateGenerator::generateVarTemplate(VarTemplate& template_) {
    std::stringstream ss;
    
    ss << "var " << template_.getName() << " = {\n";
    
    bool first = true;
    for (const auto& [key, value] : template_.getAttributes()) {
        if (!first) {
            ss << ",\n";
        }
        ss << "  " << key << ": \"" << value << "\"";
        first = false;
    }
    
    ss << "\n};";
    return ss.str();
}

std::string TemplateGenerator::generateAttributes(const std::unordered_map<std::string, std::string>& attributes) {
    std::stringstream ss;
    
    bool first = true;
    for (const auto& [key, value] : attributes) {
        if (!first) {
            ss << " ";
        }
        ss << key << "=\"" << value << "\"";
        first = false;
    }
    
    return ss.str();
}

} // namespace CHTL