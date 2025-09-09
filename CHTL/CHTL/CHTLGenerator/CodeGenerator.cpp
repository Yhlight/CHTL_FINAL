#include "CodeGenerator.hpp"
#include <CHTL/CHTLNode/ElementNode.hpp>
#include <CHTL/CHTLNode/TextNode.hpp>
#include <CHTL/CHTLNode/StyleNode.hpp>
#include <CHTL/CHTLNode/TemplateStyleNode.hpp>
#include <CHTL/CHTLNode/TemplateElementNode.hpp>
#include <CHTL/CHTLNode/TemplateVarNode.hpp>
#include <CHTL/CHTLNode/CustomStyleNode.hpp>
#include <CHTL/CHTLNode/CustomElementNode.hpp>
#include <CHTL/CHTLNode/CustomVarNode.hpp>
#include <CHTL/CHTLNode/OriginNode.hpp>
#include <CHTL/CHTLNode/ImportNode.hpp>
#include <CHTL/CHTLNode/ConfigNode.hpp>
#include <CHTL/CHTLNode/NamespaceNode.hpp>
#include <CHTL/CHTLNode/ConstraintNode.hpp>
#include <CHTL/CHTLLexer/Lexer.hpp>
#include <CHTL/CHTLParser/Parser.hpp>
#include <sstream>
#include <algorithm>
#include <fstream>

namespace CHTL {

CodeGenerator::CodeGenerator(bool defaultStruct) 
    : templateManager_(TemplateManager::getInstance()), useDefaultStruct_(defaultStruct) {}

std::string CodeGenerator::generateHTML(std::shared_ptr<BaseNode> root) {
    if (!root) return "";
    
    std::ostringstream html;
    
    if (useDefaultStruct_) {
        // 生成完整的 HTML 文档结构
        html << "<!DOCTYPE html>\n<html>\n<head>\n";
        
        // 生成 CSS
        std::string css = generateCSS(root);
        if (!css.empty()) {
            html << "<style>\n" << css << "\n</style>\n";
        }
        
        html << "</head>\n<body>\n";
        
        // 生成主体内容
        for (const auto& child : root->getChildren()) {
            if (child) {
                html << generateElementHTML(child);
            }
        }
        
        html << "</body>\n</html>\n";
    } else {
        // 生成纯净的内容，适合 SPA 页面
        for (const auto& child : root->getChildren()) {
            if (child) {
                html << generateElementHTML(child);
            }
        }
    }
    
    return html.str();
}

std::string CodeGenerator::generateCSS(std::shared_ptr<BaseNode> root) {
    if (!root) return "";
    
    std::ostringstream css;
    
    // 遍历所有节点生成 CSS
    generateStyleCSS(root);
    
    // 输出生成的 CSS
    for (const auto& [selector, properties] : generatedCSS_) {
        css << selector << " {\n";
        css << properties;
        css << "}\n\n";
    }
    
    return css.str();
}

std::string CodeGenerator::generateJavaScript(std::shared_ptr<BaseNode> root) {
    if (!root) return "";
    
    std::ostringstream js;
    
    // 遍历所有节点生成 JavaScript
    for (const auto& child : root->getChildren()) {
        if (child) {
            std::string childJS = generateOriginCode(child);
            if (!childJS.empty()) {
                js << childJS << "\n";
            }
        }
    }
    
    return js.str();
}

std::string CodeGenerator::generateOutput(std::shared_ptr<BaseNode> root) {
    std::string html = generateHTML(root);
    std::string css = generateCSS(root);
    std::string js = generateJavaScript(root);
    
    if (useDefaultStruct_) {
        return formatOutput(html, css, js);
    } else {
        // 纯净输出，只返回 HTML 内容
        return html;
    }
}

std::string CodeGenerator::generateElementHTML(std::shared_ptr<BaseNode> node) {
    if (!node) return "";
    
    switch (node->getType()) {
        case BaseNode::NodeType::ELEMENT: {
            auto elementNode = std::dynamic_pointer_cast<ElementNode>(node);
            if (!elementNode) return "";
            
            std::ostringstream html;
            html << "<" << elementNode->getTagName();
            
            // 生成属性
            std::string attributes = generateAttributes(elementNode->getAttributes());
            if (!attributes.empty()) {
                html << " " << attributes;
            }
            
            html << ">";
            
            // 生成子节点
            for (const auto& child : elementNode->getChildren()) {
                if (child) {
                    html << generateElementHTML(child);
                }
            }
            
            html << "</" << elementNode->getTagName() << ">";
            return html.str();
        }
        
        case BaseNode::NodeType::TEXT: {
            auto textNode = std::dynamic_pointer_cast<TextNode>(node);
            if (!textNode) return "";
            return textNode->getValue();
        }
        
        case BaseNode::NodeType::STYLE: {
            auto styleNode = std::dynamic_pointer_cast<StyleNode>(node);
            if (!styleNode) return "";
            
            // 生成内联样式
            std::string style = generateCSSProperties(styleNode->getCSSProperties());
            if (!style.empty()) {
                return " style=\"" + style + "\"";
            }
            return "";
        }
        
        case BaseNode::NodeType::TEMPLATE: {
            return generateTemplateCode(node);
        }
        
        case BaseNode::NodeType::CUSTOM: {
            return generateCustomCode(node);
        }
        
        case BaseNode::NodeType::ORIGIN: {
            return generateOriginCode(node);
        }
        
        case BaseNode::NodeType::IMPORT: {
            return generateImportCode(node);
        }
        
        case BaseNode::NodeType::CONFIG: {
            return generateConfigCode(node);
        }
        
        case BaseNode::NodeType::NAMESPACE: {
            return generateNamespaceCode(node);
        }
        
        case BaseNode::NodeType::OPERATOR: {
            return generateConstraintCode(node);
        }
        
        default:
            return "";
    }
}

std::string CodeGenerator::generateTextHTML(std::shared_ptr<BaseNode> node) {
    auto textNode = std::dynamic_pointer_cast<TextNode>(node);
    if (!textNode) return "";
    
    return textNode->getValue();
}

std::string CodeGenerator::generateStyleCSS(std::shared_ptr<BaseNode> node) {
    if (!node) return "";
    
    std::string css;
    
    // 处理样式节点
    if (node->getType() == BaseNode::NodeType::STYLE) {
        auto styleNode = std::dynamic_pointer_cast<StyleNode>(node);
        if (styleNode) {
            std::string selector = "." + styleNode->getAttribute("class");
            if (selector == ".") selector = "body";
            
            std::string properties = generateCSSProperties(styleNode->getCSSProperties());
            if (!properties.empty()) {
                generatedCSS_[selector] = properties;
            }
        }
    }
    
    // 递归处理子节点
    for (const auto& child : node->getChildren()) {
        if (child) {
            generateStyleCSS(child);
        }
    }
    
    return css;
}

std::string CodeGenerator::generateTemplateCode(std::shared_ptr<BaseNode> node) {
    // 模板节点需要根据类型生成相应的代码
    if (auto templateStyleNode = std::dynamic_pointer_cast<TemplateStyleNode>(node)) {
        // 合并继承的属性
        templateStyleNode->mergeInheritedProperties();
        
        // 生成样式模板的 CSS 代码
        std::string css = templateStyleNode->toCSS();
        if (!css.empty()) {
            std::string templateName = templateStyleNode->getTemplateName();
            generatedCSS_[templateName] = css;
            
            // 注册到模板管理器
            templateManager_.registerTemplate(templateName, templateStyleNode);
        }
        return "";
    } else if (auto templateElementNode = std::dynamic_pointer_cast<TemplateElementNode>(node)) {
        // 合并继承的内容
        templateElementNode->mergeInheritedContent();
        
        // 生成元素模板的 HTML 代码
        std::string html = templateElementNode->toHTML();
        std::string templateName = templateElementNode->getTemplateName();
        
        // 注册到模板管理器
        templateManager_.registerTemplate(templateName, templateElementNode);
        
        return html;
    } else if (auto templateVarNode = std::dynamic_pointer_cast<TemplateVarNode>(node)) {
        // 合并继承的变量
        templateVarNode->mergeInheritedVariables();
        
        // 变量模板不直接生成 HTML，但需要注册变量
        std::string templateName = templateVarNode->getTemplateName();
        for (const auto& [name, value] : templateVarNode->getVariables()) {
            // 注册变量到全局变量表
            globalVariables_[templateName + "." + name] = value;
        }
        
        // 注册到模板管理器
        templateManager_.registerTemplate(templateName, templateVarNode);
        
        return "";
    }
    
    return "";
}

std::string CodeGenerator::generateCustomCode(std::shared_ptr<BaseNode> node) {
    // 自定义节点需要根据类型生成相应的代码
    if (auto customStyleNode = std::dynamic_pointer_cast<CustomStyleNode>(node)) {
        // 应用特例化操作
        customStyleNode->applySpecialization();
        
        // 生成自定义样式组的 CSS 代码
        std::string css = customStyleNode->toCSS();
        if (!css.empty()) {
            std::string customName = customStyleNode->getCustomName();
            generatedCSS_[customName] = css;
            
            // 注册到模板管理器
            templateManager_.registerCustom(customName, customStyleNode);
        }
        return "";
    } else if (auto customElementNode = std::dynamic_pointer_cast<CustomElementNode>(node)) {
        // 应用特例化操作
        customElementNode->applySpecialization();
        
        // 生成自定义元素组的 HTML 代码
        std::string html = customElementNode->toHTML();
        std::string customName = customElementNode->getCustomName();
        
        // 注册到模板管理器
        templateManager_.registerCustom(customName, customElementNode);
        
        return html;
    } else if (auto customVarNode = std::dynamic_pointer_cast<CustomVarNode>(node)) {
        // 应用特例化操作
        customVarNode->applySpecialization();
        
        // 注册特例化变量到全局变量表
        std::string customName = customVarNode->getCustomName();
        for (const auto& [name, value] : customVarNode->getSpecializedVariables()) {
            globalVariables_[customName + "." + name] = value;
        }
        
        // 注册到模板管理器
        templateManager_.registerCustom(customName, customVarNode);
        return "";
    }
    
    return "";
}

std::string CodeGenerator::generateOriginCode(std::shared_ptr<BaseNode> node) {
    auto originNode = std::dynamic_pointer_cast<OriginNode>(node);
    if (!originNode) return "";
    
    std::string type = originNode->getAttribute("type");
    if (type == "@Html") {
        return originNode->getValue();
    } else if (type == "@Style") {
        // 将原始 CSS 添加到生成的 CSS 中
        std::string name = originNode->getAttribute("name");
        if (!name.empty()) {
            generatedCSS_[name] = originNode->getValue();
        }
        return "";
    } else if (type == "@JavaScript") {
        // 将原始 JavaScript 添加到生成的 JS 中
        std::string name = originNode->getAttribute("name");
        if (!name.empty()) {
            generatedJS_[name] = originNode->getValue();
        }
        return "";
    }
    
    return "";
}

std::string CodeGenerator::generateImportCode(std::shared_ptr<BaseNode> node) {
    auto importNode = std::dynamic_pointer_cast<ImportNode>(node);
    if (!importNode) return "";
    
    // 根据导入类型处理不同的文件
    std::string importType = importNode->getImportType();
    std::string importPath = importNode->getImportPath();
    std::string importName = importNode->getImportName();
    
    if (importType == "@Chtl") {
        // 导入 CHTL 文件
        std::ifstream file(importPath);
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            file.close();
            
            // 解析导入的 CHTL 文件
            Lexer lexer(content);
            Parser parser(lexer);
            auto importedAST = parser.parse();
            
            if (!parser.hasError() && importedAST) {
                // 递归生成导入文件的代码
                std::string importedHTML = generateHTML(importedAST);
                std::string importedCSS = generateCSS(importedAST);
                std::string importedJS = generateJavaScript(importedAST);
                
                // 将导入的内容添加到相应的输出中
                if (!importedCSS.empty()) {
                    generatedCSS_[importName] = importedCSS;
                }
                if (!importedJS.empty()) {
                    generatedJS_[importName] = importedJS;
                }
                
                return importedHTML;
            }
        }
    } else if (importType == "@Html") {
        // 导入 HTML 文件
        std::ifstream file(importPath);
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            file.close();
            return content;
        }
    } else if (importType == "@Style") {
        // 导入 CSS 文件
        std::ifstream file(importPath);
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            file.close();
            
            if (!importName.empty()) {
                generatedCSS_[importName] = content;
            } else {
                generatedCSS_[importPath] = content;
            }
        }
    } else if (importType == "@JavaScript") {
        // 导入 JavaScript 文件
        std::ifstream file(importPath);
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            file.close();
            
            if (!importName.empty()) {
                generatedJS_[importName] = content;
            } else {
                generatedJS_[importPath] = content;
            }
        }
    }
    
    return "";
}

std::string CodeGenerator::generateConfigCode(std::shared_ptr<BaseNode> node) {
    auto configNode = std::dynamic_pointer_cast<ConfigNode>(node);
    if (!configNode) return "";
    
    // 处理配置节点的配置项
    auto configs = configNode->getConfigurations();
    for (const auto& [key, value] : configs) {
        if (key == "use" && value == "html5") {
            // 应用 HTML5 配置
            // 这里可以设置 HTML5 相关的默认配置
        } else if (key == "template" || key == "custom") {
            // 处理关键字重定义
            // 这里可以更新词法分析器的关键字表
        }
    }
    
    // 处理 [Name] 块中的关键字重定义
    auto nameBlocks = configNode->getNameBlocks();
    for (const auto& [oldName, newName] : nameBlocks) {
        // 更新关键字映射
        keywordMappings_[oldName] = newName;
    }
    
    return "";
}

std::string CodeGenerator::generateNamespaceCode(std::shared_ptr<BaseNode> node) {
    auto namespaceNode = std::dynamic_pointer_cast<NamespaceNode>(node);
    if (!namespaceNode) return "";
    
    // 处理命名空间中的约束
    auto constraints = namespaceNode->getConstraints();
    std::string namespaceName = namespaceNode->getNamespaceName();
    
    for (const auto& constraint : constraints) {
        if (constraint->getType() == ConstraintNode::ConstraintType::PRECISE) {
            // 处理精确约束
            auto elementNames = constraint->getElementNames();
            for (const auto& elementName : elementNames) {
                // 在命名空间中限制特定元素的使用
                namespaceConstraints_[namespaceName].push_back(elementName);
                constrainedElements_.insert(elementName);
            }
        } else if (constraint->getType() == ConstraintNode::ConstraintType::TYPE) {
            // 处理类型约束
            auto typeConstraints = constraint->getTypeConstraints();
            for (const auto& typeConstraint : typeConstraints) {
                // 在命名空间中限制特定类型的使用
                namespaceTypeConstraints_[namespaceName].push_back(typeConstraint);
                constrainedTypes_.insert(typeConstraint);
            }
        }
    }
    
    // 生成命名空间中的子节点
    std::ostringstream oss;
    for (const auto& child : namespaceNode->getChildren()) {
        if (child) {
            oss << generateElementHTML(child);
        }
    }
    
    return oss.str();
}

std::string CodeGenerator::generateConstraintCode(std::shared_ptr<BaseNode> node) {
    auto constraintNode = std::dynamic_pointer_cast<ConstraintNode>(node);
    if (!constraintNode) return "";
    
    // 约束节点用于验证和限制，不直接生成 HTML
    // 但需要记录约束信息用于验证
    
    if (constraintNode->getType() == ConstraintNode::ConstraintType::PRECISE) {
        // 精确约束：限制特定元素的使用
        auto elementNames = constraintNode->getElementNames();
        for (const auto& elementName : elementNames) {
            // 记录被约束的元素
            constrainedElements_.insert(elementName);
        }
    } else if (constraintNode->getType() == ConstraintNode::ConstraintType::TYPE) {
        // 类型约束：限制特定类型的使用
        auto typeConstraints = constraintNode->getTypeConstraints();
        for (const auto& typeConstraint : typeConstraints) {
            // 记录被约束的类型
            constrainedTypes_.insert(typeConstraint);
        }
    }
    
    return "";
}

std::string CodeGenerator::resolveTemplateReference(const std::string& reference) {
    // 解析模板引用
    auto styleTemplate = templateManager_.getStyleTemplate(reference);
    if (styleTemplate) {
        return styleTemplate->toCSS();
    }
    
    auto elementTemplate = templateManager_.getElementTemplate(reference);
    if (elementTemplate) {
        return elementTemplate->toHTML();
    }
    
    auto varTemplate = templateManager_.getVarTemplate(reference);
    if (varTemplate) {
        return varTemplate->toHTML();
    }
    
    return reference;
}

std::string CodeGenerator::resolveVariableReference(const std::string& reference) {
    // 首先检查全局变量表
    auto it = globalVariables_.find(reference);
    if (it != globalVariables_.end()) {
        return it->second;
    }
    
    // 解析模板变量引用
    auto varTemplate = templateManager_.getVarTemplate(reference);
    if (varTemplate) {
        return varTemplate->getVariable(reference);
    }
    
    // 解析带模板前缀的变量引用 (格式: TemplateName.VariableName)
    size_t dotPos = reference.find('.');
    if (dotPos != std::string::npos) {
        std::string templateName = reference.substr(0, dotPos);
        std::string variableName = reference.substr(dotPos + 1);
        
        auto templateVar = templateManager_.getVarTemplate(templateName);
        if (templateVar) {
            return templateVar->getVariable(variableName);
        }
    }
    
    return reference;
}

std::string CodeGenerator::resolveStyleReference(const std::string& reference) {
    // 首先检查生成的 CSS 表
    auto it = generatedCSS_.find(reference);
    if (it != generatedCSS_.end()) {
        return it->second;
    }
    
    // 解析模板样式引用
    auto styleTemplate = templateManager_.getStyleTemplate(reference);
    if (styleTemplate) {
        return styleTemplate->toCSS();
    }
    
    // 解析带模板前缀的样式引用 (格式: TemplateName.StyleName)
    size_t dotPos = reference.find('.');
    if (dotPos != std::string::npos) {
        std::string templateName = reference.substr(0, dotPos);
        std::string styleName = reference.substr(dotPos + 1);
        
        auto templateStyle = templateManager_.getStyleTemplate(templateName);
        if (templateStyle) {
            return templateStyle->toCSS();
        }
    }
    
    return reference;
}

std::string CodeGenerator::generateAttributes(const std::map<std::string, std::string>& attributes) {
    std::ostringstream attrs;
    
    for (const auto& [key, value] : attributes) {
        if (!value.empty()) {
            attrs << key << "=\"" << value << "\" ";
        } else {
            attrs << key << " ";
        }
    }
    
    std::string result = attrs.str();
    if (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    
    return result;
}

std::string CodeGenerator::generateCSSProperties(const std::map<std::string, std::string>& properties) {
    std::ostringstream css;
    
    for (const auto& [property, value] : properties) {
        // 解析变量引用
        std::string resolvedValue = resolveVariableReference(value);
        css << property << ": " << resolvedValue << "; ";
    }
    
    std::string result = css.str();
    if (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    
    return result;
}

std::string CodeGenerator::formatOutput(const std::string& html, const std::string& css, const std::string& js) {
    std::ostringstream output;
    
    output << "=== Generated HTML ===\n";
    output << html << "\n\n";
    
    if (!css.empty()) {
        output << "=== Generated CSS ===\n";
        output << css << "\n\n";
    }
    
    if (!js.empty()) {
        output << "=== Generated JavaScript ===\n";
        output << js << "\n\n";
    }
    
    return output.str();
}

} // namespace CHTL