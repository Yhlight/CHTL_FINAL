#include "chtl/custom_system.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace chtl {
namespace custom_system {

// CustomParser 实现
CustomParser::CustomParser(const std::string& input) : input_(input), position_(0) {}

std::vector<std::shared_ptr<CustomInstance>> CustomParser::parse() {
    std::vector<std::shared_ptr<CustomInstance>> instances;
    
    while (position_ < input_.length()) {
        skipWhitespace();
        if (position_ >= input_.length()) break;
        
        if (input_.substr(position_, 8) == "[Custom]") {
            position_ += 8;
            skipWhitespace();
            
            if (input_.substr(position_, 8) == "@Style") {
                position_ += 8;
                auto custom = parseStyleGroupCustom();
                if (custom) {
                    auto instance = std::make_shared<CustomInstance>(
                        custom->name, custom->name, CustomType::STYLE_GROUP);
                    instances.push_back(instance);
                }
            } else if (input_.substr(position_, 8) == "@Element") {
                position_ += 8;
                auto custom = parseElementCustom();
                if (custom) {
                    auto instance = std::make_shared<CustomInstance>(
                        custom->name, custom->name, CustomType::ELEMENT);
                    instances.push_back(instance);
                }
            } else if (input_.substr(position_, 6) == "@Var") {
                position_ += 6;
                auto custom = parseVariableGroupCustom();
                if (custom) {
                    auto instance = std::make_shared<CustomInstance>(
                        custom->name, custom->name, CustomType::VARIABLE_GROUP);
                    instances.push_back(instance);
                }
            }
        } else {
            advance();
        }
    }
    
    return instances;
}

void CustomParser::skipWhitespace() {
    while (position_ < input_.length() && std::isspace(input_[position_])) {
        position_++;
    }
}

char CustomParser::currentChar() const {
    if (position_ >= input_.length()) return '\0';
    return input_[position_];
}

char CustomParser::peekChar() const {
    if (position_ + 1 >= input_.length()) return '\0';
    return input_[position_ + 1];
}

void CustomParser::advance() {
    if (position_ < input_.length()) {
        position_++;
    }
}

std::shared_ptr<StyleGroupCustom> CustomParser::parseStyleGroupCustom() {
    auto custom = std::make_shared<StyleGroupCustom>();
    
    skipWhitespace();
    if (currentChar() == '{') {
        advance();
        skipWhitespace();
        
        // 解析自定义名称
        custom->name = parseIdentifier();
        skipWhitespace();
        
        // 解析基础模板
        if (input_.substr(position_, 4) == "from") {
            position_ += 4;
            skipWhitespace();
            custom->baseTemplate = parseIdentifier();
            skipWhitespace();
        }
        
        // 解析无值样式组
        if (input_.substr(position_, 8) == "unvalued") {
            position_ += 8;
            custom->unvalued = true;
            skipWhitespace();
        }
        
        // 解析删除属性
        if (input_.substr(position_, 6) == "delete") {
            position_ += 6;
            skipWhitespace();
            if (currentChar() == '(') {
                advance();
                auto deletedProps = parsePropertyList();
                custom->deletedProperties.insert(deletedProps.begin(), deletedProps.end());
                skipWhitespace();
                if (currentChar() == ')') advance();
            }
        }
        
        // 解析删除继承
        if (input_.substr(position_, 6) == "remove") {
            position_ += 6;
            skipWhitespace();
            if (input_.substr(position_, 9) == "inherit") {
                position_ += 9;
                custom->deletedInheritance.insert("inherit");
            }
        }
        
        // 解析添加的属性
        while (position_ < input_.length() && currentChar() != '}') {
            skipWhitespace();
            if (currentChar() == '}') break;
            
            std::string property = parseIdentifier();
            skipWhitespace();
            
            if (currentChar() == ':') {
                advance();
                skipWhitespace();
                std::string value = parseString();
                custom->properties.push_back(property + ": " + value + ";");
            }
            
            skipWhitespace();
        }
        
        if (currentChar() == '}') advance();
    }
    
    return custom;
}

std::shared_ptr<ElementCustom> CustomParser::parseElementCustom() {
    auto custom = std::make_shared<ElementCustom>();
    
    skipWhitespace();
    if (currentChar() == '{') {
        advance();
        skipWhitespace();
        
        // 解析自定义名称
        custom->name = parseIdentifier();
        skipWhitespace();
        
        // 解析基础模板
        if (input_.substr(position_, 4) == "from") {
            position_ += 4;
            skipWhitespace();
            custom->baseTemplate = parseIdentifier();
            skipWhitespace();
        }
        
        // 解析添加元素
        if (input_.substr(position_, 3) == "add") {
            position_ += 3;
            skipWhitespace();
            if (currentChar() == '(') {
                advance();
                custom->addedElements = parseElementList();
                skipWhitespace();
                if (currentChar() == ')') advance();
            }
        }
        
        // 解析插入元素
        if (input_.substr(position_, 6) == "insert") {
            position_ += 6;
            skipWhitespace();
            if (currentChar() == '(') {
                advance();
                custom->insertedElements = parseElementList();
                skipWhitespace();
                if (currentChar() == ')') advance();
            }
        }
        
        // 解析删除元素
        if (input_.substr(position_, 6) == "delete") {
            position_ += 6;
            skipWhitespace();
            if (currentChar() == '(') {
                advance();
                auto deletedElements = parseElementList();
                custom->deletedElements.insert(deletedElements.begin(), deletedElements.end());
                skipWhitespace();
                if (currentChar() == ')') advance();
            }
        }
        
        // 解析删除继承
        if (input_.substr(position_, 6) == "remove") {
            position_ += 6;
            skipWhitespace();
            if (input_.substr(position_, 9) == "inherit") {
                position_ += 9;
                custom->deletedInheritance.insert("inherit");
            }
        }
        
        if (currentChar() == '}') advance();
    }
    
    return custom;
}

std::shared_ptr<VariableGroupCustom> CustomParser::parseVariableGroupCustom() {
    auto custom = std::make_shared<VariableGroupCustom>();
    
    skipWhitespace();
    if (currentChar() == '{') {
        advance();
        skipWhitespace();
        
        // 解析自定义名称
        custom->name = parseIdentifier();
        skipWhitespace();
        
        // 解析基础模板
        if (input_.substr(position_, 4) == "from") {
            position_ += 4;
            skipWhitespace();
            custom->baseTemplate = parseIdentifier();
            skipWhitespace();
        }
        
        // 解析添加变量
        if (input_.substr(position_, 3) == "add") {
            position_ += 3;
            skipWhitespace();
            if (currentChar() == '(') {
                advance();
                custom->addedVariables = parseVariableMap();
                skipWhitespace();
                if (currentChar() == ')') advance();
            }
        }
        
        // 解析修改变量
        if (input_.substr(position_, 6) == "modify") {
            position_ += 6;
            skipWhitespace();
            if (currentChar() == '(') {
                advance();
                custom->modifiedVariables = parseVariableMap();
                skipWhitespace();
                if (currentChar() == ')') advance();
            }
        }
        
        // 解析删除变量
        if (input_.substr(position_, 6) == "delete") {
            position_ += 6;
            skipWhitespace();
            if (currentChar() == '(') {
                advance();
                auto deletedVars = parsePropertyList();
                custom->deletedVariables.insert(deletedVars.begin(), deletedVars.end());
                skipWhitespace();
                if (currentChar() == ')') advance();
            }
        }
        
        // 解析删除继承
        if (input_.substr(position_, 6) == "remove") {
            position_ += 6;
            skipWhitespace();
            if (input_.substr(position_, 9) == "inherit") {
                position_ += 9;
                custom->deletedInheritance.insert("inherit");
            }
        }
        
        if (currentChar() == '}') advance();
    }
    
    return custom;
}

std::vector<std::string> CustomParser::parsePropertyList() {
    std::vector<std::string> properties;
    
    while (position_ < input_.length() && currentChar() != ')') {
        skipWhitespace();
        if (currentChar() == ')') break;
        
        properties.push_back(parseIdentifier());
        skipWhitespace();
        
        if (currentChar() == ',') {
            advance();
            skipWhitespace();
        }
    }
    
    return properties;
}

std::vector<std::string> CustomParser::parseElementList() {
    std::vector<std::string> elements;
    
    while (position_ < input_.length() && currentChar() != ')') {
        skipWhitespace();
        if (currentChar() == ')') break;
        
        elements.push_back(parseString());
        skipWhitespace();
        
        if (currentChar() == ',') {
            advance();
            skipWhitespace();
        }
    }
    
    return elements;
}

std::map<std::string, std::string> CustomParser::parseVariableMap() {
    std::map<std::string, std::string> variables;
    
    while (position_ < input_.length() && currentChar() != ')') {
        skipWhitespace();
        if (currentChar() == ')') break;
        
        std::string key = parseIdentifier();
        skipWhitespace();
        
        if (currentChar() == ':') {
            advance();
            skipWhitespace();
            std::string value = parseString();
            variables[key] = value;
        }
        
        skipWhitespace();
        
        if (currentChar() == ',') {
            advance();
            skipWhitespace();
        }
    }
    
    return variables;
}

std::string CustomParser::parseString() {
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

std::string CustomParser::parseIdentifier() {
    std::string result;
    
    while (position_ < input_.length() && 
           (std::isalnum(currentChar()) || currentChar() == '_' || currentChar() == '-')) {
        result += currentChar();
        advance();
    }
    
    return result;
}

bool CustomParser::isKeyword(const std::string& word) const {
    static const std::vector<std::string> keywords = {
        "custom", "style", "element", "var", "from", "add", "insert", "delete", "remove", "unvalued"
    };
    
    return std::find(keywords.begin(), keywords.end(), word) != keywords.end();
}

bool CustomParser::isCustomKeyword(const std::string& word) const {
    static const std::vector<std::string> customKeywords = {
        "Custom", "Style", "Element", "Var", "from", "add", "insert", "delete", "remove", "unvalued"
    };
    
    return std::find(customKeywords.begin(), customKeywords.end(), word) != customKeywords.end();
}

// CustomCompiler 实现
CustomCompiler::CustomCompiler() {}

std::string CustomCompiler::compile(const std::vector<std::shared_ptr<CustomInstance>>& instances) {
    std::string result;
    
    for (const auto& instance : instances) {
        switch (instance->type) {
            case CustomType::STYLE_GROUP: {
                auto custom = getStyleGroupCustom(instance->customName);
                if (custom) {
                    result += compileStyleGroupCustom(*custom, *instance);
                }
                break;
            }
            case CustomType::ELEMENT: {
                auto custom = getElementCustom(instance->customName);
                if (custom) {
                    result += compileElementCustom(*custom, *instance);
                }
                break;
            }
            case CustomType::VARIABLE_GROUP: {
                auto custom = getVariableGroupCustom(instance->customName);
                if (custom) {
                    result += compileVariableGroupCustom(*custom, *instance);
                }
                break;
            }
        }
    }
    
    return result;
}

std::string CustomCompiler::compileStyleGroupCustom(const StyleGroupCustom& custom, const CustomInstance& instance) {
    std::string result = "." + instance.instanceName + " {\n";
    
    // 处理无值样式组
    if (custom.unvalued) {
        result += "    /* 无值样式组 */\n";
    }
    
    // 处理基础模板（如果有）
    if (!custom.baseTemplate.empty()) {
        result += "    /* 基于模板: " + custom.baseTemplate + " */\n";
    }
    
    // 处理删除的属性
    if (!custom.deletedProperties.empty()) {
        result += "    /* 删除的属性: ";
        for (const auto& prop : custom.deletedProperties) {
            result += prop + " ";
        }
        result += "*/\n";
    }
    
    // 处理删除的继承
    if (!custom.deletedInheritance.empty()) {
        result += "    /* 删除的继承: ";
        for (const auto& inh : custom.deletedInheritance) {
            result += inh + " ";
        }
        result += "*/\n";
    }
    
    // 处理添加的属性
    for (const auto& prop : custom.properties) {
        std::string processedProp = resolveParameters(prop, instance.parameters);
        result += "    " + processedProp + "\n";
    }
    
    result += "}\n\n";
    return result;
}

std::string CustomCompiler::compileElementCustom(const ElementCustom& custom, const CustomInstance& instance) {
    std::string result = "/* 元素自定义: " + instance.instanceName + " */\n";
    
    // 处理基础模板（如果有）
    if (!custom.baseTemplate.empty()) {
        result += "/* 基于模板: " + custom.baseTemplate + " */\n";
    }
    
    // 处理添加的元素
    if (!custom.addedElements.empty()) {
        result += "/* 添加的元素 */\n";
        for (const auto& element : custom.addedElements) {
            std::string processedElement = resolveParameters(element, instance.parameters);
            result += processedElement + "\n";
        }
    }
    
    // 处理插入的元素
    if (!custom.insertedElements.empty()) {
        result += "/* 插入的元素 */\n";
        for (const auto& element : custom.insertedElements) {
            std::string processedElement = resolveParameters(element, instance.parameters);
            result += processedElement + "\n";
        }
    }
    
    // 处理删除的元素
    if (!custom.deletedElements.empty()) {
        result += "/* 删除的元素: ";
        for (const auto& element : custom.deletedElements) {
            result += element + " ";
        }
        result += "*/\n";
    }
    
    // 处理删除的继承
    if (!custom.deletedInheritance.empty()) {
        result += "/* 删除的继承: ";
        for (const auto& inh : custom.deletedInheritance) {
            result += inh + " ";
        }
        result += "*/\n";
    }
    
    result += "\n";
    return result;
}

std::string CustomCompiler::compileVariableGroupCustom(const VariableGroupCustom& custom, const CustomInstance& instance) {
    std::string result = "/* 变量组自定义: " + instance.instanceName + " */\n";
    
    // 处理基础模板（如果有）
    if (!custom.baseTemplate.empty()) {
        result += "/* 基于模板: " + custom.baseTemplate + " */\n";
    }
    
    // 处理添加的变量
    if (!custom.addedVariables.empty()) {
        result += "/* 添加的变量 */\n";
        for (const auto& var : custom.addedVariables) {
            std::string processedValue = resolveParameters(var.second, instance.parameters);
            result += "var " + var.first + " = " + processedValue + ";\n";
        }
    }
    
    // 处理修改的变量
    if (!custom.modifiedVariables.empty()) {
        result += "/* 修改的变量 */\n";
        for (const auto& var : custom.modifiedVariables) {
            std::string processedValue = resolveParameters(var.second, instance.parameters);
            result += "var " + var.first + " = " + processedValue + ";\n";
        }
    }
    
    // 处理删除的变量
    if (!custom.deletedVariables.empty()) {
        result += "/* 删除的变量: ";
        for (const auto& var : custom.deletedVariables) {
            result += var + " ";
        }
        result += "*/\n";
    }
    
    // 处理删除的继承
    if (!custom.deletedInheritance.empty()) {
        result += "/* 删除的继承: ";
        for (const auto& inh : custom.deletedInheritance) {
            result += inh + " ";
        }
        result += "*/\n";
    }
    
    result += "\n";
    return result;
}

void CustomCompiler::addCustom(const std::string& name, std::shared_ptr<StyleGroupCustom> custom) {
    styleGroupCustoms_[name] = custom;
}

void CustomCompiler::addCustom(const std::string& name, std::shared_ptr<ElementCustom> custom) {
    elementCustoms_[name] = custom;
}

void CustomCompiler::addCustom(const std::string& name, std::shared_ptr<VariableGroupCustom> custom) {
    variableGroupCustoms_[name] = custom;
}

std::shared_ptr<StyleGroupCustom> CustomCompiler::getStyleGroupCustom(const std::string& name) {
    auto it = styleGroupCustoms_.find(name);
    return (it != styleGroupCustoms_.end()) ? it->second : nullptr;
}

std::shared_ptr<ElementCustom> CustomCompiler::getElementCustom(const std::string& name) {
    auto it = elementCustoms_.find(name);
    return (it != elementCustoms_.end()) ? it->second : nullptr;
}

std::shared_ptr<VariableGroupCustom> CustomCompiler::getVariableGroupCustom(const std::string& name) {
    auto it = variableGroupCustoms_.find(name);
    return (it != variableGroupCustoms_.end()) ? it->second : nullptr;
}

std::string CustomCompiler::resolveParameters(const std::string& content, const std::map<std::string, std::string>& parameters) {
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

std::string CustomCompiler::processCustomOperations(const std::string& baseContent, const std::string& customContent, CustomType type) {
    // 处理自定义操作
    return baseContent + "\n" + customContent;
}

std::string CustomCompiler::mergeCustomizations(const std::string& base, const std::string& custom) {
    // 合并自定义化
    return base + "\n" + custom;
}

// CustomManager 实现
CustomManager::CustomManager() {}

void CustomManager::addCustom(const std::string& name, std::shared_ptr<StyleGroupCustom> custom) {
    compiler_.addCustom(name, custom);
}

void CustomManager::addCustom(const std::string& name, std::shared_ptr<ElementCustom> custom) {
    compiler_.addCustom(name, custom);
}

void CustomManager::addCustom(const std::string& name, std::shared_ptr<VariableGroupCustom> custom) {
    compiler_.addCustom(name, custom);
}

std::shared_ptr<StyleGroupCustom> CustomManager::getStyleGroupCustom(const std::string& name) {
    return compiler_.getStyleGroupCustom(name);
}

std::shared_ptr<ElementCustom> CustomManager::getElementCustom(const std::string& name) {
    return compiler_.getElementCustom(name);
}

std::shared_ptr<VariableGroupCustom> CustomManager::getVariableGroupCustom(const std::string& name) {
    return compiler_.getVariableGroupCustom(name);
}

std::string CustomManager::generateCode(const std::vector<std::shared_ptr<CustomInstance>>& instances) {
    return compiler_.compile(instances);
}

std::string CustomManager::generateStyleGroupCode(const StyleGroupCustom& custom, const CustomInstance& instance) {
    return compiler_.compileStyleGroupCustom(custom, instance);
}

std::string CustomManager::generateElementCode(const ElementCustom& custom, const CustomInstance& instance) {
    return compiler_.compileElementCustom(custom, instance);
}

std::string CustomManager::generateVariableGroupCode(const VariableGroupCustom& custom, const CustomInstance& instance) {
    return compiler_.compileVariableGroupCustom(custom, instance);
}

void CustomManager::clear() {
    // 清空所有自定义
}

size_t CustomManager::getCustomCount() const {
    return 0; // 简化实现
}

std::string CustomManager::processCustomInheritance(const std::string& customName, const std::string& content, CustomType type) {
    // 处理自定义继承
    return content;
}

std::string CustomManager::resolveCustomDependencies(const std::string& content) {
    // 解析自定义依赖
    return content;
}

std::string CustomManager::validateCustomParameters(const std::string& customName, const std::map<std::string, std::string>& parameters, CustomType type) {
    // 验证自定义参数
    return "";
}

} // namespace custom_system
} // namespace chtl