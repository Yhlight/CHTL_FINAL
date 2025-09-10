#include "chtl/template_system.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace chtl {
namespace template_system {

// TemplateParser 实现
TemplateParser::TemplateParser(const std::string& input) : input_(input), position_(0) {}

std::vector<std::shared_ptr<TemplateInstance>> TemplateParser::parse() {
    std::vector<std::shared_ptr<TemplateInstance>> instances;
    
    while (position_ < input_.length()) {
        skipWhitespace();
        if (position_ >= input_.length()) break;
        
        if (input_.substr(position_, 6) == "[Template]") {
            position_ += 6;
            skipWhitespace();
            
            if (input_.substr(position_, 8) == "@Style") {
                position_ += 8;
                auto tmpl = parseStyleGroupTemplate();
                if (tmpl) {
                    // 创建模板实例
                    auto instance = std::make_shared<TemplateInstance>(
                        tmpl->name, tmpl->name, TemplateType::STYLE_GROUP);
                    instances.push_back(instance);
                }
            } else if (input_.substr(position_, 8) == "@Element") {
                position_ += 8;
                auto tmpl = parseElementTemplate();
                if (tmpl) {
                    auto instance = std::make_shared<TemplateInstance>(
                        tmpl->name, tmpl->name, TemplateType::ELEMENT);
                    instances.push_back(instance);
                }
            } else if (input_.substr(position_, 6) == "@Var") {
                position_ += 6;
                auto tmpl = parseVariableGroupTemplate();
                if (tmpl) {
                    auto instance = std::make_shared<TemplateInstance>(
                        tmpl->name, tmpl->name, TemplateType::VARIABLE_GROUP);
                    instances.push_back(instance);
                }
            }
        } else {
            advance();
        }
    }
    
    return instances;
}

void TemplateParser::skipWhitespace() {
    while (position_ < input_.length() && std::isspace(input_[position_])) {
        position_++;
    }
}

char TemplateParser::currentChar() const {
    if (position_ >= input_.length()) return '\0';
    return input_[position_];
}

char TemplateParser::peekChar() const {
    if (position_ + 1 >= input_.length()) return '\0';
    return input_[position_ + 1];
}

void TemplateParser::advance() {
    if (position_ < input_.length()) {
        position_++;
    }
}

std::shared_ptr<StyleGroupTemplate> TemplateParser::parseStyleGroupTemplate() {
    auto tmpl = std::make_shared<StyleGroupTemplate>();
    
    skipWhitespace();
    if (currentChar() == '{') {
        advance();
        skipWhitespace();
        
        // 解析模板名称
        tmpl->name = parseIdentifier();
        skipWhitespace();
        
        // 解析参数
        if (currentChar() == '(') {
            advance();
            tmpl->parameters = parseParameters();
            skipWhitespace();
            if (currentChar() == ')') advance();
        }
        
        // 解析继承
        if (input_.substr(position_, 7) == "inherit") {
            position_ += 7;
            skipWhitespace();
            tmpl->parentTemplate = parseIdentifier();
            tmpl->inheritanceType = InheritanceType::EXPLICIT;
        }
        
        // 解析样式属性
        while (position_ < input_.length() && currentChar() != '}') {
            skipWhitespace();
            if (currentChar() == '}') break;
            
            std::string property = parseIdentifier();
            skipWhitespace();
            
            if (currentChar() == ':') {
                advance();
                skipWhitespace();
                std::string value = parseString();
                tmpl->properties.push_back(property + ": " + value + ";");
            }
            
            skipWhitespace();
        }
        
        if (currentChar() == '}') advance();
    }
    
    return tmpl;
}

std::shared_ptr<ElementTemplate> TemplateParser::parseElementTemplate() {
    auto tmpl = std::make_shared<ElementTemplate>();
    
    skipWhitespace();
    if (currentChar() == '{') {
        advance();
        skipWhitespace();
        
        // 解析模板名称
        tmpl->name = parseIdentifier();
        skipWhitespace();
        
        // 解析参数
        if (currentChar() == '(') {
            advance();
            tmpl->parameters = parseParameters();
            skipWhitespace();
            if (currentChar() == ')') advance();
        }
        
        // 解析继承
        if (input_.substr(position_, 7) == "inherit") {
            position_ += 7;
            skipWhitespace();
            tmpl->parentTemplate = parseIdentifier();
            tmpl->inheritanceType = InheritanceType::EXPLICIT;
        }
        
        // 解析元素类型
        tmpl->elementType = parseIdentifier();
        skipWhitespace();
        
        // 解析属性
        while (position_ < input_.length() && currentChar() != '}') {
            skipWhitespace();
            if (currentChar() == '}') break;
            
            std::string attr = parseIdentifier();
            tmpl->attributes.push_back(attr);
            skipWhitespace();
        }
        
        if (currentChar() == '}') advance();
    }
    
    return tmpl;
}

std::shared_ptr<VariableGroupTemplate> TemplateParser::parseVariableGroupTemplate() {
    auto tmpl = std::make_shared<VariableGroupTemplate>();
    
    skipWhitespace();
    if (currentChar() == '{') {
        advance();
        skipWhitespace();
        
        // 解析模板名称
        tmpl->name = parseIdentifier();
        skipWhitespace();
        
        // 解析参数
        if (currentChar() == '(') {
            advance();
            tmpl->parameters = parseParameters();
            skipWhitespace();
            if (currentChar() == ')') advance();
        }
        
        // 解析继承
        if (input_.substr(position_, 7) == "inherit") {
            position_ += 7;
            skipWhitespace();
            tmpl->parentTemplate = parseIdentifier();
            tmpl->inheritanceType = InheritanceType::EXPLICIT;
        }
        
        // 解析变量
        while (position_ < input_.length() && currentChar() != '}') {
            skipWhitespace();
            if (currentChar() == '}') break;
            
            std::string varName = parseIdentifier();
            skipWhitespace();
            
            if (currentChar() == ':') {
                advance();
                skipWhitespace();
                std::string varValue = parseString();
                tmpl->variables[varName] = varValue;
            }
            
            skipWhitespace();
        }
        
        if (currentChar() == '}') advance();
    }
    
    return tmpl;
}

std::vector<TemplateParameter> TemplateParser::parseParameters() {
    std::vector<TemplateParameter> parameters;
    
    while (position_ < input_.length() && currentChar() != ')') {
        skipWhitespace();
        if (currentChar() == ')') break;
        
        parameters.push_back(parseParameter());
        skipWhitespace();
        
        if (currentChar() == ',') {
            advance();
            skipWhitespace();
        }
    }
    
    return parameters;
}

TemplateParameter TemplateParser::parseParameter() {
    std::string name = parseIdentifier();
    skipWhitespace();
    
    std::string type;
    if (currentChar() == ':') {
        advance();
        skipWhitespace();
        type = parseIdentifier();
        skipWhitespace();
    }
    
    std::string defaultValue;
    bool required = true;
    
    if (currentChar() == '=') {
        advance();
        skipWhitespace();
        defaultValue = parseString();
        required = false;
    }
    
    return TemplateParameter(name, type, defaultValue, required);
}

std::string TemplateParser::parseString() {
    std::string result;
    
    if (currentChar() == '"') {
        advance();
        while (position_ < input_.length() && currentChar() != '"') {
            result += currentChar();
            advance();
        }
        if (currentChar() == '"') advance();
    } else if (currentChar() == '\'') {
        advance();
        while (position_ < input_.length() && currentChar() != '\'') {
            result += currentChar();
            advance();
        }
        if (currentChar() == '\'') advance();
    } else {
        // 解析无引号字符串
        while (position_ < input_.length() && 
               !std::isspace(currentChar()) && 
               currentChar() != ',' && 
               currentChar() != ')' && 
               currentChar() != '}' && 
               currentChar() != ';') {
            result += currentChar();
            advance();
        }
    }
    
    return result;
}

std::string TemplateParser::parseIdentifier() {
    std::string result;
    
    while (position_ < input_.length() && 
           (std::isalnum(currentChar()) || currentChar() == '_' || currentChar() == '-')) {
        result += currentChar();
        advance();
    }
    
    return result;
}

std::string TemplateParser::parseNumber() {
    std::string result;
    
    while (position_ < input_.length() && 
           (std::isdigit(currentChar()) || currentChar() == '.' || currentChar() == '-')) {
        result += currentChar();
        advance();
    }
    
    return result;
}

bool TemplateParser::isKeyword(const std::string& word) const {
    static const std::vector<std::string> keywords = {
        "template", "style", "element", "var", "inherit", "if", "else", "for", "while"
    };
    
    return std::find(keywords.begin(), keywords.end(), word) != keywords.end();
}

bool TemplateParser::isTemplateKeyword(const std::string& word) const {
    static const std::vector<std::string> templateKeywords = {
        "Template", "Style", "Element", "Var", "inherit"
    };
    
    return std::find(templateKeywords.begin(), templateKeywords.end(), word) != templateKeywords.end();
}

// TemplateCompiler 实现
TemplateCompiler::TemplateCompiler() {}

std::string TemplateCompiler::compile(const std::vector<std::shared_ptr<TemplateInstance>>& instances) {
    std::string result;
    
    for (const auto& instance : instances) {
        switch (instance->type) {
            case TemplateType::STYLE_GROUP: {
                auto tmpl = getStyleGroupTemplate(instance->templateName);
                if (tmpl) {
                    result += compileStyleGroupTemplate(*tmpl, *instance);
                }
                break;
            }
            case TemplateType::ELEMENT: {
                auto tmpl = getElementTemplate(instance->templateName);
                if (tmpl) {
                    result += compileElementTemplate(*tmpl, *instance);
                }
                break;
            }
            case TemplateType::VARIABLE_GROUP: {
                auto tmpl = getVariableGroupTemplate(instance->templateName);
                if (tmpl) {
                    result += compileVariableGroupTemplate(*tmpl, *instance);
                }
                break;
            }
        }
    }
    
    return result;
}

std::string TemplateCompiler::compileStyleGroupTemplate(const StyleGroupTemplate& tmpl, const TemplateInstance& instance) {
    std::string result = "." + instance.instanceName + " {\n";
    
    // 处理继承
    if (!tmpl.parentTemplate.empty()) {
        auto parent = getStyleGroupTemplate(tmpl.parentTemplate);
        if (parent) {
            result += "    /* 继承自 " + tmpl.parentTemplate + " */\n";
            for (const auto& prop : parent->properties) {
                result += "    " + prop + "\n";
            }
        }
    }
    
    // 处理当前模板的属性
    for (const auto& prop : tmpl.properties) {
        std::string processedProp = resolveParameters(prop, instance.parameters);
        result += "    " + processedProp + "\n";
    }
    
    result += "}\n\n";
    return result;
}

std::string TemplateCompiler::compileElementTemplate(const ElementTemplate& tmpl, const TemplateInstance& instance) {
    std::string result = "<" + tmpl.elementType;
    
    // 处理属性
    for (const auto& attr : tmpl.attributes) {
        std::string processedAttr = resolveParameters(attr, instance.parameters);
        result += " " + processedAttr;
    }
    
    result += ">";
    
    // 处理子元素
    for (const auto& child : tmpl.children) {
        std::string processedChild = resolveParameters(child, instance.parameters);
        result += processedChild;
    }
    
    result += "</" + tmpl.elementType + ">\n";
    return result;
}

std::string TemplateCompiler::compileVariableGroupTemplate(const VariableGroupTemplate& tmpl, const TemplateInstance& instance) {
    std::string result = "/* 变量组: " + instance.instanceName + " */\n";
    
    for (const auto& var : tmpl.variables) {
        std::string processedValue = resolveParameters(var.second, instance.parameters);
        result += "var " + var.first + " = " + processedValue + ";\n";
    }
    
    result += "\n";
    return result;
}

void TemplateCompiler::addTemplate(const std::string& name, std::shared_ptr<StyleGroupTemplate> tmpl) {
    styleGroupTemplates_[name] = tmpl;
}

void TemplateCompiler::addTemplate(const std::string& name, std::shared_ptr<ElementTemplate> tmpl) {
    elementTemplates_[name] = tmpl;
}

void TemplateCompiler::addTemplate(const std::string& name, std::shared_ptr<VariableGroupTemplate> tmpl) {
    variableGroupTemplates_[name] = tmpl;
}

std::shared_ptr<StyleGroupTemplate> TemplateCompiler::getStyleGroupTemplate(const std::string& name) {
    auto it = styleGroupTemplates_.find(name);
    return (it != styleGroupTemplates_.end()) ? it->second : nullptr;
}

std::shared_ptr<ElementTemplate> TemplateCompiler::getElementTemplate(const std::string& name) {
    auto it = elementTemplates_.find(name);
    return (it != elementTemplates_.end()) ? it->second : nullptr;
}

std::shared_ptr<VariableGroupTemplate> TemplateCompiler::getVariableGroupTemplate(const std::string& name) {
    auto it = variableGroupTemplates_.find(name);
    return (it != variableGroupTemplates_.end()) ? it->second : nullptr;
}

std::string TemplateCompiler::resolveParameters(const std::string& content, const std::map<std::string, std::string>& parameters) {
    std::string result = content;
    
    for (const auto& param : parameters) {
        std::string placeholder = "{{" + param.first + "}}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), param.second);
            pos += param.second.length();
        }
    }
    
    return result;
}

std::string TemplateCompiler::processInheritance(const std::string& templateName, const std::string& content, InheritanceType type) {
    // 处理模板继承逻辑
    return content;
}

std::string TemplateCompiler::mergeTemplates(const std::string& parent, const std::string& child) {
    // 合并父模板和子模板
    return parent + "\n" + child;
}

// TemplateManager 实现
TemplateManager::TemplateManager() {}

void TemplateManager::addTemplate(const std::string& name, std::shared_ptr<StyleGroupTemplate> tmpl) {
    compiler_.addTemplate(name, tmpl);
}

void TemplateManager::addTemplate(const std::string& name, std::shared_ptr<ElementTemplate> tmpl) {
    compiler_.addTemplate(name, tmpl);
}

void TemplateManager::addTemplate(const std::string& name, std::shared_ptr<VariableGroupTemplate> tmpl) {
    compiler_.addTemplate(name, tmpl);
}

std::shared_ptr<StyleGroupTemplate> TemplateManager::getStyleGroupTemplate(const std::string& name) {
    return compiler_.getStyleGroupTemplate(name);
}

std::shared_ptr<ElementTemplate> TemplateManager::getElementTemplate(const std::string& name) {
    return compiler_.getElementTemplate(name);
}

std::shared_ptr<VariableGroupTemplate> TemplateManager::getVariableGroupTemplate(const std::string& name) {
    return compiler_.getVariableGroupTemplate(name);
}

std::string TemplateManager::generateCode(const std::vector<std::shared_ptr<TemplateInstance>>& instances) {
    return compiler_.compile(instances);
}

std::string TemplateManager::generateStyleGroupCode(const StyleGroupTemplate& tmpl, const TemplateInstance& instance) {
    return compiler_.compileStyleGroupTemplate(tmpl, instance);
}

std::string TemplateManager::generateElementCode(const ElementTemplate& tmpl, const TemplateInstance& instance) {
    return compiler_.compileElementTemplate(tmpl, instance);
}

std::string TemplateManager::generateVariableGroupCode(const VariableGroupTemplate& tmpl, const TemplateInstance& instance) {
    return compiler_.compileVariableGroupTemplate(tmpl, instance);
}

void TemplateManager::clear() {
    // 清空所有模板
}

size_t TemplateManager::getTemplateCount() const {
    return 0; // 简化实现
}

std::string TemplateManager::processTemplateInheritance(const std::string& templateName, const std::string& content, TemplateType type) {
    // 处理模板继承
    return content;
}

std::string TemplateManager::resolveTemplateDependencies(const std::string& content) {
    // 解析模板依赖
    return content;
}

std::string TemplateManager::validateTemplateParameters(const std::string& templateName, const std::map<std::string, std::string>& parameters, TemplateType type) {
    // 验证模板参数
    return "";
}

} // namespace template_system
} // namespace chtl