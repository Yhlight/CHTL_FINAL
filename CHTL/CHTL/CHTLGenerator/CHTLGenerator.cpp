#include "CHTLGenerator.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

CHTLGenerator::CHTLGenerator() 
    : debugMode(false), strictMode(false), generateDefaultStructure(true), 
      outputFormat("html5"), indentLevel(0), indentString("  ") {
    styleProcessor = std::make_unique<CHTLStyleProcessor>();
}

std::string CHTLGenerator::generate(std::shared_ptr<BaseNode> ast) {
    reset();
    
    if (!ast) {
        addError("AST为空");
        return "";
    }
    
    try {
        // 收集样式、脚本、模板等
        collectStyles(ast);
        collectScripts(ast);
        collectTemplates(ast);
        collectCustoms(ast);
        collectVariables(ast);
        
        // 生成HTML
        std::string html = generateHTML(ast);
        
        if (debugMode) {
            std::cout << "生成完成，HTML长度: " << html.length() << std::endl;
        }
        
        return html;
        
    } catch (const std::exception& e) {
        addError("生成过程中发生错误: " + std::string(e.what()));
        return "";
    }
}

std::string CHTLGenerator::generateHTML(std::shared_ptr<BaseNode> ast) {
    std::ostringstream oss;
    
    if (generateDefaultStructure) {
        oss << "<!DOCTYPE html>\n";
        oss << "<html lang=\"zh-CN\">\n";
        oss << "<head>\n";
        oss << "    <meta charset=\"UTF-8\">\n";
        oss << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
        oss << "    <title>CHTL Document</title>\n";
        
        // 添加全局样式
        if (!globalStyles.empty()) {
            oss << "    <style>\n";
            for (const auto& style : globalStyles) {
                oss << "        " << style.first << ": " << style.second << ";\n";
            }
            oss << "    </style>\n";
        }
        
        oss << "</head>\n";
        oss << "<body>\n";
    }
    
    // 生成主体内容
    if (ast->getNodeType() == NodeType::ELEMENT) {
        auto element = std::static_pointer_cast<ElementNode>(ast);
        oss << generateElement(element);
    } else {
        // 遍历子节点
        for (const auto& child : ast->getChildren()) {
            oss << generateNode(child);
        }
    }
    
    if (generateDefaultStructure) {
        // 添加全局脚本
        if (!globalScript.empty()) {
            oss << "    <script>\n";
            oss << globalScript << "\n";
            oss << "    </script>\n";
        }
        
        oss << "</body>\n";
        oss << "</html>\n";
    }
    
    return oss.str();
}

std::string CHTLGenerator::generateCSS(std::shared_ptr<BaseNode> ast) {
    std::ostringstream oss;
    
    // 生成全局样式
    for (const auto& style : globalStyles) {
        oss << style.first << ": " << style.second << ";\n";
    }
    
    // 生成局部样式
    for (const auto& styleNode : localStyles) {
        oss << generateStyle(styleNode) << "\n";
    }
    
    return oss.str();
}

std::string CHTLGenerator::generateJavaScript(std::shared_ptr<BaseNode> ast) {
    std::ostringstream oss;
    
    // 生成全局脚本
    oss << globalScript << "\n";
    
    // 生成局部脚本
    for (const auto& scriptNode : localScripts) {
        oss << generateScript(scriptNode) << "\n";
    }
    
    return oss.str();
}

std::string CHTLGenerator::generateNode(std::shared_ptr<BaseNode> node) const {
    if (!node) {
        return "";
    }
    
    switch (node->getNodeType()) {
        case NodeType::ELEMENT: {
            auto element = std::static_pointer_cast<ElementNode>(node);
            return generateElement(element);
        }
        case NodeType::TEXT: {
            auto textNode = std::static_pointer_cast<TextNode>(node);
            return generateText(textNode);
        }
        case NodeType::COMMENT: {
            auto commentNode = std::static_pointer_cast<CommentNode>(node);
            return generateComment(commentNode);
        }
        case NodeType::STYLE: {
            auto styleNode = std::static_pointer_cast<StyleNode>(node);
            return generateStyle(styleNode);
        }
        case NodeType::SCRIPT: {
            auto scriptNode = std::static_pointer_cast<ScriptNode>(node);
            return generateScript(scriptNode);
        }
        case NodeType::TEMPLATE: {
            return generateTemplate(node);
        }
        case NodeType::CUSTOM: {
            return generateCustom(node);
        }
        case NodeType::ORIGIN: {
            return generateOrigin(node);
        }
        case NodeType::IMPORT: {
            return generateImport(node);
        }
        case NodeType::NAMESPACE: {
            return generateNamespace(node);
        }
        case NodeType::CONFIGURATION: {
            return generateConfiguration(node);
        }
        case NodeType::USE: {
            return generateUse(node);
        }
        default:
            return "";
    }
}

std::string CHTLGenerator::generateElement(std::shared_ptr<ElementNode> element) const {
    if (!element) {
        return "";
    }
    
    std::ostringstream oss;
    
    // 开始标签
    oss << getIndent() << "<" << element->getTagName();
    
    // 属性
    std::string attrs = processAttributes(element->getAttributes());
    if (!attrs.empty()) {
        oss << " " << attrs;
    }
    
    // 自闭合或void元素
    if (element->isSelfClosingTag() || element->isVoidElementTag()) {
        oss << " />";
        return oss.str();
    }
    
    oss << ">";
    
    // 子节点
    if (!element->getChildren().empty()) {
        bool hasBlockChildren = false;
        for (const auto& child : element->getChildren()) {
            if (child->getNodeType() == NodeType::ELEMENT) {
                auto childElement = std::static_pointer_cast<ElementNode>(child);
                if (childElement->isBlockElementTag(childElement->getTagName())) {
                    hasBlockChildren = true;
                    break;
                }
            }
        }
        
        if (hasBlockChildren) {
            oss << "\n";
            increaseIndent();
            for (const auto& child : element->getChildren()) {
                oss << generateNode(child) << "\n";
            }
            decreaseIndent();
            oss << getIndent();
        } else {
            for (const auto& child : element->getChildren()) {
                oss << generateNode(child);
            }
        }
    }
    
    // 结束标签
    oss << "</" << element->getTagName() << ">";
    
    return oss.str();
}

std::string CHTLGenerator::generateText(std::shared_ptr<TextNode> textNode) const {
    if (!textNode) {
        return "";
    }
    
    if (textNode->isWhitespace()) {
        return textNode->getText();
    }
    
    return textNode->getEscapedText();
}

std::string CHTLGenerator::generateComment(std::shared_ptr<CommentNode> commentNode) const {
    if (!commentNode) {
        return "";
    }
    
    return commentNode->toHTML();
}

std::string CHTLGenerator::generateStyle(std::shared_ptr<StyleNode> styleNode) const {
    if (!styleNode) {
        return "";
    }
    
    if (styleNode->isLocalStyleBlock()) {
        return processLocalStyle(styleNode);
    } else {
        return processGlobalStyle(styleNode);
    }
}

std::string CHTLGenerator::generateScript(std::shared_ptr<ScriptNode> scriptNode) const {
    if (!scriptNode) {
        return "";
    }
    
    if (scriptNode->isLocalScriptBlock()) {
        return processLocalScript(scriptNode);
    } else {
        return processGlobalScript(scriptNode);
    }
}

std::string CHTLGenerator::generateTemplate(std::shared_ptr<BaseNode> templateNode) const {
    if (!templateNode) {
        return "";
    }
    
    return processTemplate(templateNode);
}

std::string CHTLGenerator::generateCustom(std::shared_ptr<BaseNode> customNode) const {
    if (!customNode) {
        return "";
    }
    
    return processCustom(customNode);
}

std::string CHTLGenerator::generateOrigin(std::shared_ptr<BaseNode> originNode) const {
    if (!originNode) {
        return "";
    }
    
    std::string type = originNode->getAttribute("type");
    std::string content = originNode->getValue();
    
    if (type == "Html") {
        return content;
    } else if (type == "Style") {
        return "<style>\n" + content + "\n</style>";
    } else if (type == "JavaScript") {
        return "<script>\n" + content + "\n</script>";
    }
    
    return content;
}

std::string CHTLGenerator::generateImport(std::shared_ptr<BaseNode> importNode) const {
    if (!importNode) {
        return "";
    }
    
    // 导入节点不直接生成HTML，而是处理导入的内容
    return "";
}

std::string CHTLGenerator::generateNamespace(std::shared_ptr<BaseNode> namespaceNode) const {
    if (!namespaceNode) {
        return "";
    }
    
    // 命名空间节点不直接生成HTML，而是处理命名空间内容
    return "";
}

std::string CHTLGenerator::generateConfiguration(std::shared_ptr<BaseNode> configNode) const {
    if (!configNode) {
        return "";
    }
    
    // 配置节点不直接生成HTML，而是处理配置
    return "";
}

std::string CHTLGenerator::generateUse(std::shared_ptr<BaseNode> useNode) const {
    if (!useNode) {
        return "";
    }
    
    // use节点不直接生成HTML，而是处理use语句
    return "";
}

std::string CHTLGenerator::processLocalStyle(std::shared_ptr<StyleNode> styleNode) const {
    // 局部样式块不直接输出HTML，而是被收集到全局样式中
    if (styleProcessor) {
        return styleProcessor->processStyleBlock(styleNode, nullptr);
    }
    return "";
}

std::string CHTLGenerator::processGlobalStyle(std::shared_ptr<StyleNode> styleNode) const {
    if (styleProcessor) {
        return styleProcessor->processGlobalStyleBlock(styleNode);
    }
    return generateCSS(styleNode);
}

std::string CHTLGenerator::generateCSS(std::shared_ptr<StyleNode> styleNode) const {
    std::ostringstream oss;
    
    // 输出样式规则
    for (const auto& rule : styleNode->getStyleRules()) {
        std::string selector = rule->getAttribute("selector");
        if (!selector.empty()) {
            oss << selector << " {\n";
            
            for (const auto& attr : rule->getAttributes()) {
                if (attr.first != "type" && attr.first != "selector") {
                    oss << "    " << attr.first << ": " << attr.second << ";\n";
                }
            }
            
            oss << "}\n";
        }
    }
    
    // 输出内联样式属性
    if (!styleNode->getStyleProperties().empty()) {
        oss << "/* Inline styles */\n";
        for (const auto& prop : styleNode->getStyleProperties()) {
            oss << prop.first << ": " << prop.second << ";\n";
        }
    }
    
    return oss.str();
}

std::string CHTLGenerator::processStyleProperties(const std::map<std::string, std::string>& properties) const {
    std::ostringstream oss;
    bool first = true;
    
    for (const auto& prop : properties) {
        if (!first) oss << "; ";
        oss << prop.first << ": " << prop.second;
        first = false;
    }
    
    return oss.str();
}

std::string CHTLGenerator::processStyleRules(const std::vector<std::shared_ptr<BaseNode>>& rules) const {
    std::ostringstream oss;
    
    for (const auto& rule : rules) {
        std::string selector = rule->getAttribute("selector");
        if (!selector.empty()) {
            oss << selector << " {\n";
            
            for (const auto& attr : rule->getAttributes()) {
                if (attr.first != "type" && attr.first != "selector") {
                    oss << "    " << attr.first << ": " << attr.second << ";\n";
                }
            }
            
            oss << "}\n";
        }
    }
    
    return oss.str();
}

std::string CHTLGenerator::processLocalScript(std::shared_ptr<ScriptNode> scriptNode) const {
    // 局部脚本块不直接输出HTML，而是被收集到全局脚本中
    return "";
}

std::string CHTLGenerator::processGlobalScript(std::shared_ptr<ScriptNode> scriptNode) const {
    return generateJavaScript(scriptNode);
}

std::string CHTLGenerator::generateJavaScript(std::shared_ptr<ScriptNode> scriptNode) const {
    return scriptNode->toJavaScript();
}

std::string CHTLGenerator::processTemplate(std::shared_ptr<BaseNode> templateNode) const {
    // 模板处理
    return "";
}

std::string CHTLGenerator::processStyleTemplate(std::shared_ptr<BaseNode> templateNode) const {
    // 样式模板处理
    return "";
}

std::string CHTLGenerator::processElementTemplate(std::shared_ptr<BaseNode> templateNode) const {
    // 元素模板处理
    return "";
}

std::string CHTLGenerator::processVariableTemplate(std::shared_ptr<BaseNode> templateNode) const {
    // 变量模板处理
    return "";
}

std::string CHTLGenerator::processCustom(std::shared_ptr<BaseNode> customNode) const {
    // 自定义处理
    return "";
}

std::string CHTLGenerator::processCustomStyle(std::shared_ptr<BaseNode> customNode) const {
    // 自定义样式处理
    return "";
}

std::string CHTLGenerator::processCustomElement(std::shared_ptr<BaseNode> customNode) const {
    // 自定义元素处理
    return "";
}

std::string CHTLGenerator::processCustomVariable(std::shared_ptr<BaseNode> customNode) const {
    // 自定义变量处理
    return "";
}

std::string CHTLGenerator::processAttributes(const std::map<std::string, std::string>& attributes) const {
    std::ostringstream oss;
    bool first = true;
    
    for (const auto& attr : attributes) {
        if (!first) oss << " ";
        oss << attr.first;
        if (!attr.second.empty()) {
            oss << "=\"" << processAttributeValue(attr.second) << "\"";
        }
        first = false;
    }
    
    return oss.str();
}

std::string CHTLGenerator::processAttributeValue(const std::string& value) const {
    // 简单的属性值处理
    return value;
}

std::string CHTLGenerator::processExpression(const std::string& expression) const {
    // 简化的表达式处理
    return expression;
}

std::string CHTLGenerator::processConditionalExpression(const std::string& expression) const {
    // 简化的条件表达式处理
    return expression;
}

std::string CHTLGenerator::processArithmeticExpression(const std::string& expression) const {
    // 简化的算术表达式处理
    return expression;
}

std::string CHTLGenerator::processPropertyReference(const std::string& reference) const {
    // 简化的属性引用处理
    return reference;
}

std::string CHTLGenerator::processSelector(const std::string& selector) const {
    // 简化的选择器处理
    return selector;
}

std::string CHTLGenerator::processClassSelector(const std::string& className) const {
    return "." + className;
}

std::string CHTLGenerator::processIdSelector(const std::string& id) const {
    return "#" + id;
}

std::string CHTLGenerator::processPseudoClassSelector(const std::string& pseudoClass) const {
    return ":" + pseudoClass;
}

std::string CHTLGenerator::processPseudoElementSelector(const std::string& pseudoElement) const {
    return "::" + pseudoElement;
}

std::string CHTLGenerator::processPath(const std::string& path) const {
    // 简化的路径处理
    return path;
}

std::string CHTLGenerator::resolvePath(const std::string& path) const {
    // 简化的路径解析
    return path;
}

bool CHTLGenerator::validateHTML(const std::string& html) const {
    // 简单的HTML验证
    return !html.empty() && 
           html.find("<html") != std::string::npos && 
           html.find("</html>") != std::string::npos;
}

bool CHTLGenerator::validateCSS(const std::string& css) const {
    // 简单的CSS验证
    return true;
}

bool CHTLGenerator::validateJavaScript(const std::string& js) const {
    // 简单的JavaScript验证
    return true;
}

void CHTLGenerator::collectStyles(std::shared_ptr<BaseNode> ast) {
    if (!ast) {
        return;
    }
    
    // 遍历AST收集样式
    ast->traverse([this](std::shared_ptr<BaseNode> node) {
        if (node->getNodeType() == NodeType::STYLE) {
            auto styleNode = std::static_pointer_cast<StyleNode>(node);
            if (styleNode->isLocalStyleBlock()) {
                localStyles.push_back(styleNode);
            } else {
                // 处理全局样式
                for (const auto& prop : styleNode->getStyleProperties()) {
                    globalStyles[prop.first] = prop.second;
                }
            }
        }
    });
}

void CHTLGenerator::collectScripts(std::shared_ptr<BaseNode> ast) {
    if (!ast) {
        return;
    }
    
    // 遍历AST收集脚本
    ast->traverse([this](std::shared_ptr<BaseNode> node) {
        if (node->getNodeType() == NodeType::SCRIPT) {
            auto scriptNode = std::static_pointer_cast<ScriptNode>(node);
            if (scriptNode->isLocalScriptBlock()) {
                localScripts.push_back(scriptNode);
            } else {
                globalScript += scriptNode->getScriptContent() + "\n";
            }
        }
    });
}

void CHTLGenerator::collectTemplates(std::shared_ptr<BaseNode> ast) {
    if (!ast) {
        return;
    }
    
    // 遍历AST收集模板
    ast->traverse([this](std::shared_ptr<BaseNode> node) {
        if (node->getNodeType() == NodeType::TEMPLATE) {
            templates[node->getName()] = node;
        }
    });
}

void CHTLGenerator::collectCustoms(std::shared_ptr<BaseNode> ast) {
    if (!ast) {
        return;
    }
    
    // 遍历AST收集自定义
    ast->traverse([this](std::shared_ptr<BaseNode> node) {
        if (node->getNodeType() == NodeType::CUSTOM) {
            customs[node->getName()] = node;
        }
    });
}

void CHTLGenerator::collectVariables(std::shared_ptr<BaseNode> ast) {
    if (!ast) {
        return;
    }
    
    // 遍历AST收集变量
    ast->traverse([this](std::shared_ptr<BaseNode> node) {
        if (node->getNodeType() == NodeType::CONFIGURATION) {
            for (const auto& attr : node->getAttributes()) {
                variables[attr.first] = attr.second;
            }
        }
    });
}

void CHTLGenerator::addError(const std::string& error) {
    errors.push_back(error);
}

void CHTLGenerator::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

void CHTLGenerator::clearMessages() {
    errors.clear();
    warnings.clear();
}

void CHTLGenerator::reset() {
    indentLevel = 0;
    globalStyles.clear();
    localStyles.clear();
    localScripts.clear();
    globalScript.clear();
    templates.clear();
    customs.clear();
    variables.clear();
    clearMessages();
}

bool CHTLGenerator::validate() const {
    return errors.empty();
}

std::string CHTLGenerator::getIndent() const {
    return std::string(indentLevel * indentString.length(), ' ');
}

void CHTLGenerator::increaseIndent() {
    indentLevel++;
}

void CHTLGenerator::decreaseIndent() {
    if (indentLevel > 0) {
        indentLevel--;
    }
}

std::string CHTLGenerator::getDebugInfo() const {
    std::ostringstream oss;
    oss << "CHTL生成器调试信息:\n";
    oss << "调试模式: " << (debugMode ? "开启" : "关闭") << "\n";
    oss << "严格模式: " << (strictMode ? "开启" : "关闭") << "\n";
    oss << "生成默认结构: " << (generateDefaultStructure ? "是" : "否") << "\n";
    oss << "输出格式: " << outputFormat << "\n";
    oss << "缩进级别: " << indentLevel << "\n";
    oss << "全局样式数: " << globalStyles.size() << "\n";
    oss << "局部样式数: " << localStyles.size() << "\n";
    oss << "局部脚本数: " << localScripts.size() << "\n";
    oss << "模板数: " << templates.size() << "\n";
    oss << "自定义数: " << customs.size() << "\n";
    oss << "变量数: " << variables.size() << "\n";
    oss << "错误数: " << errors.size() << "\n";
    oss << "警告数: " << warnings.size() << "\n";
    return oss.str();
}

} // namespace CHTL