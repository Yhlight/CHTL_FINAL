#include "CHTLGenerator.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace CHTL {

CHTLGenerator::CHTLGenerator() : classCounter(0), idCounter(0) {
}

CHTLGenerator::~CHTLGenerator() {
}

std::string CHTLGenerator::generate(const std::shared_ptr<ProgramNode>& program) {
    return generateProgram(program);
}

std::string CHTLGenerator::generate(const std::shared_ptr<CHTLASTNode>& node) {
    if (!node) {
        return "";
    }
    
    switch (node->getType()) {
        case CHTLASTNode::NodeType::PROGRAM:
            return generateProgram(std::dynamic_pointer_cast<ProgramNode>(node));
        case CHTLASTNode::NodeType::ELEMENT:
            return generateElement(std::dynamic_pointer_cast<ElementNode>(node));
        case CHTLASTNode::NodeType::TEXT:
            return generateText(std::dynamic_pointer_cast<TextNode>(node));
        case CHTLASTNode::NodeType::ATTRIBUTE:
            return generateAttribute(std::dynamic_pointer_cast<AttributeNode>(node));
        case CHTLASTNode::NodeType::STYLE_BLOCK:
            return generateStyleBlock(std::dynamic_pointer_cast<StyleBlockNode>(node));
        case CHTLASTNode::NodeType::STYLE_RULE:
            return generateStyleRule(std::dynamic_pointer_cast<StyleRuleNode>(node));
        case CHTLASTNode::NodeType::STYLE_SELECTOR:
            return generateStyleSelector(std::dynamic_pointer_cast<StyleSelectorNode>(node));
        case CHTLASTNode::NodeType::STYLE_PROPERTY:
            return generateStyleProperty(std::dynamic_pointer_cast<StylePropertyNode>(node));
        case CHTLASTNode::NodeType::SCRIPT_BLOCK:
            return generateScriptBlock(std::dynamic_pointer_cast<ScriptBlockNode>(node));
        case CHTLASTNode::NodeType::TEMPLATE_STYLE:
        case CHTLASTNode::NodeType::TEMPLATE_ELEMENT:
        case CHTLASTNode::NodeType::TEMPLATE_VAR:
            return generateTemplate(std::dynamic_pointer_cast<TemplateNode>(node));
        case CHTLASTNode::NodeType::CUSTOM_STYLE:
        case CHTLASTNode::NodeType::CUSTOM_ELEMENT:
        case CHTLASTNode::NodeType::CUSTOM_VAR:
            return generateCustom(std::dynamic_pointer_cast<CustomNode>(node));
        case CHTLASTNode::NodeType::ORIGIN_HTML:
        case CHTLASTNode::NodeType::ORIGIN_STYLE:
        case CHTLASTNode::NodeType::ORIGIN_JAVASCRIPT:
            return generateOrigin(std::dynamic_pointer_cast<OriginNode>(node));
        case CHTLASTNode::NodeType::IMPORT_HTML:
        case CHTLASTNode::NodeType::IMPORT_STYLE:
        case CHTLASTNode::NodeType::IMPORT_JAVASCRIPT:
        case CHTLASTNode::NodeType::IMPORT_CHTL:
        case CHTLASTNode::NodeType::IMPORT_CJMOD:
            return generateImport(std::dynamic_pointer_cast<ImportNode>(node));
        case CHTLASTNode::NodeType::CONFIGURATION:
            return generateConfiguration(std::dynamic_pointer_cast<ConfigurationNode>(node));
        case CHTLASTNode::NodeType::NAMESPACE:
            return generateNamespace(std::dynamic_pointer_cast<NamespaceNode>(node));
        case CHTLASTNode::NodeType::INFO:
            return generateInfo(std::dynamic_pointer_cast<InfoNode>(node));
        case CHTLASTNode::NodeType::EXPORT:
            return generateExport(std::dynamic_pointer_cast<ExportNode>(node));
        case CHTLASTNode::NodeType::LITERAL:
            return generateLiteral(std::dynamic_pointer_cast<LiteralNode>(node));
        case CHTLASTNode::NodeType::IDENTIFIER:
            return generateIdentifier(std::dynamic_pointer_cast<IdentifierNode>(node));
        case CHTLASTNode::NodeType::EXPRESSION:
            return generateExpression(std::dynamic_pointer_cast<ExpressionNode>(node));
        case CHTLASTNode::NodeType::BINARY_EXPRESSION:
            return generateBinaryExpression(std::dynamic_pointer_cast<BinaryExpressionNode>(node));
        case CHTLASTNode::NodeType::CONDITIONAL_EXPRESSION:
            return generateConditionalExpression(std::dynamic_pointer_cast<ConditionalExpressionNode>(node));
        case CHTLASTNode::NodeType::PROPERTY_REFERENCE:
            return generatePropertyReference(std::dynamic_pointer_cast<PropertyReferenceNode>(node));
        case CHTLASTNode::NodeType::FUNCTION_CALL:
            return generateFunctionCall(std::dynamic_pointer_cast<FunctionCallNode>(node));
        case CHTLASTNode::NodeType::USE_STATEMENT:
            return generateUseStatement(std::dynamic_pointer_cast<UseStatementNode>(node));
        case CHTLASTNode::NodeType::EXCEPT_CLAUSE:
            return generateExceptClause(std::dynamic_pointer_cast<ExceptClauseNode>(node));
        default:
            return "/* Unknown node type */";
    }
}

std::string CHTLGenerator::generateGlobalStyles() const {
    std::ostringstream oss;
    for (const auto& style : globalStyles) {
        oss << style << "\n";
    }
    return oss.str();
}

void CHTLGenerator::addGlobalStyle(const std::string& style) {
    globalStyles.push_back(style);
}

void CHTLGenerator::setConfiguration(const std::map<std::string, std::string>& config) {
    configuration = config;
}

std::string CHTLGenerator::getConfiguration(const std::string& key) const {
    auto it = configuration.find(key);
    return (it != configuration.end()) ? it->second : "";
}

std::string CHTLGenerator::generateProgram(const std::shared_ptr<ProgramNode>& node) {
    std::ostringstream oss;
    
    // 生成HTML5声明
    if (getConfiguration("HTML5") == "true") {
        oss << "<!DOCTYPE html>\n";
    }
    
    for (const auto& statement : node->getStatements()) {
        std::string code = generate(statement);
        if (!code.empty()) {
            oss << code << "\n";
        }
    }
    
    return oss.str();
}

std::string CHTLGenerator::generateElement(const std::shared_ptr<ElementNode>& node) {
    std::ostringstream oss;
    
    // 保存当前元素上下文
    std::string oldElementId = currentElementId;
    std::string oldElementClass = currentElementClass;
    
    // 设置当前元素上下文
    for (const auto& attr : node->getAttributes()) {
        auto attrNode = std::dynamic_pointer_cast<AttributeNode>(attr);
        if (attrNode && attrNode->getName() == "id") {
            currentElementId = attrNode->getValue()->generateCode();
        } else if (attrNode && attrNode->getName() == "class") {
            currentElementClass = attrNode->getValue()->generateCode();
        }
    }
    
    oss << "<" << node->getTagName();
    
    // 生成属性
    for (const auto& attr : node->getAttributes()) {
        std::string attrCode = generate(attr);
        if (!attrCode.empty()) {
            oss << " " << attrCode;
        }
    }
    
    // 检查是否是自闭合标签
    std::set<std::string> selfClosingTags = {
        "br", "hr", "img", "input", "meta", "link", "area", "base", "col", 
        "embed", "source", "track", "wbr"
    };
    
    if (selfClosingTags.find(node->getTagName()) != selfClosingTags.end()) {
        oss << " />";
    } else {
        oss << ">";
        
        // 生成子元素
        for (const auto& child : node->getChildren()) {
            oss << generate(child);
        }
        
        // 生成样式块
        if (node->getStyleBlock()) {
            oss << processStyleBlock(std::dynamic_pointer_cast<StyleBlockNode>(node->getStyleBlock()), currentElementId);
        }
        
        // 生成脚本块
        if (node->getScriptBlock()) {
            oss << generate(node->getScriptBlock());
        }
        
        oss << "</" << node->getTagName() << ">";
    }
    
    // 恢复元素上下文
    currentElementId = oldElementId;
    currentElementClass = oldElementClass;
    
    return oss.str();
}

std::string CHTLGenerator::generateText(const std::shared_ptr<TextNode>& node) {
    return escapeHtml(node->getContent());
}

std::string CHTLGenerator::generateAttribute(const std::shared_ptr<AttributeNode>& node) {
    if (node->getValue()) {
        return node->getName() + "=\"" + escapeHtml(node->getValue()->generateCode()) + "\"";
    }
    return node->getName();
}

std::string CHTLGenerator::generateStyleBlock(const std::shared_ptr<StyleBlockNode>& node) {
    return processStyleBlock(node, currentElementId);
}

std::string CHTLGenerator::generateStyleRule(const std::shared_ptr<StyleRuleNode>& node) {
    return processStyleRule(node, currentElementId);
}

std::string CHTLGenerator::generateStyleSelector(const std::shared_ptr<StyleSelectorNode>& node) {
    return resolveSelector(node->getSelector(), currentElementId);
}

std::string CHTLGenerator::generateStyleProperty(const std::shared_ptr<StylePropertyNode>& node) {
    return processStyleProperty(node);
}

std::string CHTLGenerator::generateScriptBlock(const std::shared_ptr<ScriptBlockNode>& node) {
    return "<script>\n" + node->getContent() + "\n</script>";
}

std::string CHTLGenerator::generateTemplate(const std::shared_ptr<TemplateNode>& node) {
    // 模板在编译时被处理，这里返回空字符串
    return "";
}

std::string CHTLGenerator::generateCustom(const std::shared_ptr<CustomNode>& node) {
    // 自定义在编译时被处理，这里返回空字符串
    return "";
}

std::string CHTLGenerator::generateOrigin(const std::shared_ptr<OriginNode>& node) {
    return node->getContent();
}

std::string CHTLGenerator::generateImport(const std::shared_ptr<ImportNode>& node) {
    // 导入在编译时被处理，这里返回空字符串
    return "";
}

std::string CHTLGenerator::generateConfiguration(const std::shared_ptr<ConfigurationNode>& node) {
    // 配置在编译时被处理，这里返回空字符串
    return "";
}

std::string CHTLGenerator::generateNamespace(const std::shared_ptr<NamespaceNode>& node) {
    // 命名空间在编译时被处理，这里返回空字符串
    return "";
}

std::string CHTLGenerator::generateInfo(const std::shared_ptr<InfoNode>& node) {
    // 信息在编译时被处理，这里返回空字符串
    return "";
}

std::string CHTLGenerator::generateExport(const std::shared_ptr<ExportNode>& node) {
    // 导出在编译时被处理，这里返回空字符串
    return "";
}

std::string CHTLGenerator::generateLiteral(const std::shared_ptr<LiteralNode>& node) {
    if (node->getLiteralType() == "string") {
        return "\"" + escapeHtml(node->getValue()) + "\"";
    }
    return node->getValue();
}

std::string CHTLGenerator::generateIdentifier(const std::shared_ptr<IdentifierNode>& node) {
    return resolveVariable(node->getName());
}

std::string CHTLGenerator::generateExpression(const std::shared_ptr<ExpressionNode>& node) {
    if (node->getExpression()) {
        return generate(node->getExpression());
    }
    return "";
}

std::string CHTLGenerator::generateBinaryExpression(const std::shared_ptr<BinaryExpressionNode>& node) {
    if (node->getLeft() && node->getRight()) {
        return processArithmeticExpression(node);
    }
    return "";
}

std::string CHTLGenerator::generateConditionalExpression(const std::shared_ptr<ConditionalExpressionNode>& node) {
    return processConditionalExpression(node);
}

std::string CHTLGenerator::generatePropertyReference(const std::shared_ptr<PropertyReferenceNode>& node) {
    return processPropertyExpression(node);
}

std::string CHTLGenerator::generateFunctionCall(const std::shared_ptr<FunctionCallNode>& node) {
    std::ostringstream oss;
    oss << node->getFunctionName() << "(";
    
    const auto& arguments = node->getArguments();
    for (size_t i = 0; i < arguments.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << generate(arguments[i]);
    }
    
    oss << ")";
    return oss.str();
}

std::string CHTLGenerator::generateUseStatement(const std::shared_ptr<UseStatementNode>& node) {
    return "use " + node->getTarget() + ";";
}

std::string CHTLGenerator::generateExceptClause(const std::shared_ptr<ExceptClauseNode>& node) {
    std::ostringstream oss;
    oss << "except ";
    const auto& exceptions = node->getExceptions();
    for (size_t i = 0; i < exceptions.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << generate(exceptions[i]);
    }
    return oss.str();
}

std::string CHTLGenerator::processStyleBlock(const std::shared_ptr<StyleBlockNode>& node, const std::string& elementId) {
    std::ostringstream oss;
    
    if (node->isInline()) {
        // 内联样式
        oss << " style=\"";
        for (const auto& prop : node->getProperties()) {
            oss << generate(prop) << "; ";
        }
        oss << "\"";
    } else {
        // 样式块 - 添加到全局样式
        for (const auto& rule : node->getRules()) {
            std::string ruleCode = processStyleRule(std::dynamic_pointer_cast<StyleRuleNode>(rule), elementId);
            addGlobalStyle(ruleCode);
        }
    }
    
    return oss.str();
}

std::string CHTLGenerator::processStyleRule(const std::shared_ptr<StyleRuleNode>& node, const std::string& elementId) {
    std::ostringstream oss;
    
    if (node->getSelector()) {
        std::string selector = resolveSelector(node->getSelector()->generateCode(), elementId);
        oss << selector << " {\n";
        
        for (const auto& prop : node->getProperties()) {
            oss << "  " << processStyleProperty(std::dynamic_pointer_cast<StylePropertyNode>(prop)) << ";\n";
        }
        
        oss << "}";
    }
    
    return oss.str();
}

std::string CHTLGenerator::processStyleProperty(const std::shared_ptr<StylePropertyNode>& node) {
    std::ostringstream oss;
    oss << node->getName() << ": ";
    
    if (node->getValue()) {
        oss << processStyleValue(node->getValue());
    }
    
    return oss.str();
}

std::string CHTLGenerator::processStyleValue(const std::shared_ptr<CHTLASTNode>& value) {
    return generate(value);
}

std::string CHTLGenerator::processArithmeticExpression(const std::shared_ptr<BinaryExpressionNode>& node) {
    std::string left = generate(node->getLeft());
    std::string right = generate(node->getRight());
    std::string op = node->getOperator();
    
    if (isArithmeticOperator(op)) {
        return "calc(" + left + " " + op + " " + right + ")";
    }
    
    return left + " " + op + " " + right;
}

std::string CHTLGenerator::processPropertyExpression(const std::shared_ptr<PropertyReferenceNode>& node) {
    // 实现属性引用逻辑
    return node->getSelector() + "." + node->getProperty();
}

std::string CHTLGenerator::processConditionalExpression(const std::shared_ptr<ConditionalExpressionNode>& node) {
    std::string condition = generate(node->getCondition());
    std::string trueExpr = generate(node->getTrueExpression());
    std::string falseExpr = generate(node->getFalseExpression());
    
    return "(" + condition + " ? " + trueExpr + " : " + falseExpr + ")";
}

std::string CHTLGenerator::resolveSelector(const std::string& selector, const std::string& elementId) {
    if (selector == "&") {
        // 上下文推导
        if (!currentElementId.empty()) {
            return "#" + currentElementId;
        } else if (!currentElementClass.empty()) {
            return "." + currentElementClass;
        }
        return "";
    } else if (selector[0] == '.') {
        // 类选择器
        std::string className = selector.substr(1);
        if (className.empty()) {
            className = generateUniqueClass("auto");
        }
        return "." + className;
    } else if (selector[0] == '#') {
        // ID选择器
        std::string id = selector.substr(1);
        if (id.empty()) {
            id = generateUniqueId("auto");
        }
        return "#" + id;
    }
    
    return selector;
}

std::string CHTLGenerator::generateUniqueClass(const std::string& baseName) {
    return baseName + "_" + std::to_string(++classCounter);
}

std::string CHTLGenerator::generateUniqueId(const std::string& baseName) {
    return baseName + "_" + std::to_string(++idCounter);
}

std::string CHTLGenerator::resolveVariable(const std::string& name) {
    auto it = variables.find(name);
    return (it != variables.end()) ? it->second : name;
}

std::string CHTLGenerator::processVariableReference(const std::string& name) {
    return resolveVariable(name);
}

std::string CHTLGenerator::processTemplateReference(const std::string& name) {
    auto it = templates.find(name);
    if (it != templates.end()) {
        return generate(it->second);
    }
    return "";
}

std::string CHTLGenerator::processTemplateStyle(const std::string& name) {
    return processTemplateReference(name);
}

std::string CHTLGenerator::processTemplateElement(const std::string& name) {
    return processTemplateReference(name);
}

std::string CHTLGenerator::processTemplateVar(const std::string& name) {
    return processTemplateReference(name);
}

std::string CHTLGenerator::processCustomReference(const std::string& name) {
    auto it = customs.find(name);
    if (it != customs.end()) {
        return generate(it->second);
    }
    return "";
}

std::string CHTLGenerator::processCustomStyle(const std::string& name) {
    return processCustomReference(name);
}

std::string CHTLGenerator::processCustomElement(const std::string& name) {
    return processCustomReference(name);
}

std::string CHTLGenerator::processCustomVar(const std::string& name) {
    return processCustomReference(name);
}

std::string CHTLGenerator::escapeHtml(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '&': result += "&amp;"; break;
            case '"': result += "&quot;"; break;
            case '\'': result += "&#39;"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string CHTLGenerator::escapeCss(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string CHTLGenerator::escapeJs(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            case '\'': result += "\\'"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string CHTLGenerator::indent(int level) {
    return std::string(level * 2, ' ');
}

bool CHTLGenerator::isArithmeticOperator(const std::string& op) const {
    return op == "+" || op == "-" || op == "*" || op == "/" || op == "%" || op == "**";
}

bool CHTLGenerator::needsQuotes(const std::string& str) {
    return !std::all_of(str.begin(), str.end(), [](char c) {
        return std::isalnum(c) || c == '_' || c == '-';
    });
}

} // namespace CHTL