#include "CHTL_JS/CHTLJSGenerator.h"
#include "CHTL_JS/CHTLJSParser.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

CHTLJSGenerator::CHTLJSGenerator() 
    : indent_char_("  "), minify_(false), strict_mode_(false), unique_id_counter_(0) {
}

std::string CHTLJSGenerator::generateJavaScript(std::shared_ptr<CHTLNode> ast) {
    if (!ast) return "";
    
    std::stringstream js;
    
    if (strict_mode_) {
        js << "\"use strict\";" << (minify_ ? "" : "\n\n");
    }
    
    // 生成根节点的子节点
    for (const auto& child : ast->getChildren()) {
        js << generateStatement(child, 0);
    }
    
    return js.str();
}

std::string CHTLJSGenerator::generateJavaScript(const std::string& source_code) {
    CHTLJSLexer lexer;
    CHTLJSParser parser;
    
    auto ast = parser.parse(source_code);
    return generateJavaScript(ast);
}

void CHTLJSGenerator::setIndent(const std::string& indent) {
    indent_char_ = indent;
}

void CHTLJSGenerator::setMinify(bool minify) {
    minify_ = minify;
}

void CHTLJSGenerator::setStrictMode(bool strict) {
    strict_mode_ = strict;
}

std::string CHTLJSGenerator::generateStatement(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node) return "";
    
    std::stringstream js;
    std::string indent = getIndent(indent_level);
    
    if (node->getName() == "listen_function") {
        js << generateListenFunction(node, indent_level);
    } else if (node->getName() == "animate_function") {
        js << generateAnimateFunction(node, indent_level);
    } else if (node->getName() == "delegate_function") {
        js << generateDelegateFunction(node, indent_level);
    } else if (node->getName() == "router_function") {
        js << generateRouterFunction(node, indent_level);
    } else if (node->getName() == "fileloader_function") {
        js << generateFileloaderFunction(node, indent_level);
    } else if (node->getName() == "vir_declaration") {
        js << generateVirDeclaration(node, indent_level);
    } else if (node->getName() == "ineveraway_function") {
        js << generateINeverAwayFunction(node, indent_level);
    } else if (node->getName() == "util_expression") {
        js << generateUtilExpression(node, indent_level);
    } else if (node->getName() == "function_declaration") {
        js << generateFunctionDeclaration(node, indent_level);
    } else if (node->getName() == "variable_declaration") {
        js << generateVariableDeclaration(node, indent_level);
    } else if (node->getName() == "if_statement") {
        js << generateIfStatement(node, indent_level);
    } else if (node->getName() == "for_statement") {
        js << generateForStatement(node, indent_level);
    } else if (node->getName() == "while_statement") {
        js << generateWhileStatement(node, indent_level);
    } else {
        // 默认作为表达式处理
        js << indent << generateExpression(node, indent_level);
        if (!minify_) {
            js << ";";
        }
        js << (minify_ ? "" : "\n");
    }
    
    return js.str();
}

std::string CHTLJSGenerator::generateExpression(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node) return "";
    
    if (node->getName() == "assignment") {
        auto left = generateExpression(node->getChildren()[0], indent_level);
        auto right = generateExpression(node->getChildren()[1], indent_level);
        auto op = node->getAttribute("operator");
        
        std::string op_str;
        if (std::holds_alternative<std::string>(op)) {
            op_str = std::get<std::string>(op);
        }
        
        return left + " " + op_str + " " + right;
    } else if (node->getName() == "conditional") {
        auto condition = generateExpression(node->getChildren()[0], indent_level);
        auto true_expr = generateExpression(node->getChildren()[1], indent_level);
        auto false_expr = generateExpression(node->getChildren()[2], indent_level);
        
        return condition + " ? " + true_expr + " : " + false_expr;
    } else if (node->getName() == "logical_or") {
        auto left = generateExpression(node->getChildren()[0], indent_level);
        auto right = generateExpression(node->getChildren()[1], indent_level);
        
        return left + " || " + right;
    } else if (node->getName() == "logical_and") {
        auto left = generateExpression(node->getChildren()[0], indent_level);
        auto right = generateExpression(node->getChildren()[1], indent_level);
        
        return left + " && " + right;
    } else if (node->getName() == "equality") {
        auto left = generateExpression(node->getChildren()[0], indent_level);
        auto right = generateExpression(node->getChildren()[1], indent_level);
        auto op = node->getAttribute("operator");
        
        std::string op_str;
        if (std::holds_alternative<std::string>(op)) {
            op_str = std::get<std::string>(op);
        }
        
        return left + " " + op_str + " " + right;
    } else if (node->getName() == "relational") {
        auto left = generateExpression(node->getChildren()[0], indent_level);
        auto right = generateExpression(node->getChildren()[1], indent_level);
        auto op = node->getAttribute("operator");
        
        std::string op_str;
        if (std::holds_alternative<std::string>(op)) {
            op_str = std::get<std::string>(op);
        }
        
        return left + " " + op_str + " " + right;
    } else if (node->getName() == "additive") {
        auto left = generateExpression(node->getChildren()[0], indent_level);
        auto right = generateExpression(node->getChildren()[1], indent_level);
        auto op = node->getAttribute("operator");
        
        std::string op_str;
        if (std::holds_alternative<std::string>(op)) {
            op_str = std::get<std::string>(op);
        }
        
        return left + " " + op_str + " " + right;
    } else if (node->getName() == "multiplicative") {
        auto left = generateExpression(node->getChildren()[0], indent_level);
        auto right = generateExpression(node->getChildren()[1], indent_level);
        auto op = node->getAttribute("operator");
        
        std::string op_str;
        if (std::holds_alternative<std::string>(op)) {
            op_str = std::get<std::string>(op);
        }
        
        return left + " " + op_str + " " + right;
    } else if (node->getName() == "unary") {
        auto expr = generateExpression(node->getChildren()[0], indent_level);
        auto op = node->getAttribute("operator");
        
        std::string op_str;
        if (std::holds_alternative<std::string>(op)) {
            op_str = std::get<std::string>(op);
        }
        
        return op_str + expr;
    } else if (node->getName() == "function_call") {
        return generateFunctionCall(node, indent_level);
    } else if (node->getName() == "member_access") {
        return generateMemberAccess(node, indent_level);
    } else if (node->getName() == "array_access") {
        return generateArrayAccess(node, indent_level);
    } else if (node->getName() == "object_literal") {
        return generateObjectLiteral(node, indent_level);
    } else if (node->getName() == "array_literal") {
        return generateArrayLiteral(node, indent_level);
    } else if (node->getName() == "identifier") {
        return node->getName();
    } else if (node->getType() == NodeType::TEXT) {
        return "\"" + escapeJavaScript(node->getTextContent()) + "\"";
    }
    
    return "";
}

std::string CHTLJSGenerator::generateFunctionCall(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node) return "";
    
    std::string name = node->getName();
    std::stringstream js;
    
    js << name << "(";
    
    bool first = true;
    for (const auto& child : node->getChildren()) {
        if (!first) {
            js << ", ";
        }
        first = false;
        js << generateExpression(child, indent_level);
    }
    
    js << ")";
    return js.str();
}

std::string CHTLJSGenerator::generateMemberAccess(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node) return "";
    
    std::string object = node->getName();
    std::string member = node->getTextContent();
    auto op = node->getAttribute("operator");
    
    std::string op_str;
    if (std::holds_alternative<std::string>(op)) {
        op_str = std::get<std::string>(op);
    }
    
    if (op_str == "->") {
        // CHTL JS特有的箭头操作符，转换为点操作符
        return object + "." + member;
    } else {
        return object + op_str + member;
    }
}

std::string CHTLJSGenerator::generateArrayAccess(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node) return "";
    
    std::string array = node->getName();
    std::string index = generateExpression(node->getChildren()[0], indent_level);
    
    return array + "[" + index + "]";
}

std::string CHTLJSGenerator::generateObjectLiteral(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node) return "";
    
    std::stringstream js;
    std::string indent = getIndent(indent_level);
    
    js << "{" << (minify_ ? "" : "\n");
    
    bool first = true;
    for (const auto& child : node->getChildren()) {
        if (!first) {
            js << "," << (minify_ ? " " : "\n");
        }
        first = false;
        
        if (!minify_) {
            js << indent;
        }
        
        std::string key = child->getName();
        std::string value = generateExpression(child->getChildren()[0], indent_level + 1);
        
        js << key << ": " << value;
    }
    
    if (!minify_) {
        js << "\n" << getIndent(indent_level - 1);
    }
    js << "}";
    
    return js.str();
}

std::string CHTLJSGenerator::generateArrayLiteral(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node) return "";
    
    std::stringstream js;
    
    js << "[";
    
    bool first = true;
    for (const auto& child : node->getChildren()) {
        if (!first) {
            js << ", ";
        }
        first = false;
        js << generateExpression(child, indent_level);
    }
    
    js << "]";
    return js.str();
}

std::string CHTLJSGenerator::generateFunctionDeclaration(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node) return "";
    
    std::stringstream js;
    std::string indent = getIndent(indent_level);
    
    js << indent << "function " << node->getName() << "(";
    
    // 生成参数
    bool first = true;
    for (const auto& child : node->getChildren()) {
        if (child->getName() == "parameter") {
            if (!first) {
                js << ", ";
            }
            first = false;
            js << child->getName();
        }
    }
    
    js << ") {" << (minify_ ? "" : "\n");
    
    // 生成函数体
    for (const auto& child : node->getChildren()) {
        if (child->getName() != "parameter") {
            js << generateStatement(child, indent_level + 1);
        }
    }
    
    js << indent << "}" << (minify_ ? "" : "\n");
    
    return js.str();
}

std::string CHTLJSGenerator::generateVariableDeclaration(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node) return "";
    
    std::stringstream js;
    std::string indent = getIndent(indent_level);
    
    auto type = node->getAttribute("type");
    std::string type_str;
    if (std::holds_alternative<std::string>(type)) {
        type_str = std::get<std::string>(type);
    }
    
    js << indent << type_str << " " << node->getName();
    
    if (!node->getChildren().empty()) {
        js << " = " << generateExpression(node->getChildren()[0], indent_level);
    }
    
    js << ";" << (minify_ ? "" : "\n");
    
    return js.str();
}

std::string CHTLJSGenerator::generateIfStatement(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node) return "";
    
    std::stringstream js;
    std::string indent = getIndent(indent_level);
    
    auto condition = generateExpression(node->getChildren()[0], indent_level);
    js << indent << "if (" << condition << ") {" << (minify_ ? "" : "\n");
    
    // 生成if体
    for (size_t i = 1; i < node->getChildren().size(); i++) {
        js << generateStatement(node->getChildren()[i], indent_level + 1);
    }
    
    js << indent << "}" << (minify_ ? "" : "\n");
    
    return js.str();
}

std::string CHTLJSGenerator::generateForStatement(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node) return "";
    
    std::stringstream js;
    std::string indent = getIndent(indent_level);
    
    js << indent << "for (";
    
    // 生成初始化
    if (!node->getChildren().empty()) {
        js << generateExpression(node->getChildren()[0], indent_level);
    }
    js << "; ";
    
    // 生成条件
    if (node->getChildren().size() > 1) {
        js << generateExpression(node->getChildren()[1], indent_level);
    }
    js << "; ";
    
    // 生成更新
    if (node->getChildren().size() > 2) {
        js << generateExpression(node->getChildren()[2], indent_level);
    }
    
    js << ") {" << (minify_ ? "" : "\n");
    
    // 生成循环体
    for (size_t i = 3; i < node->getChildren().size(); i++) {
        js << generateStatement(node->getChildren()[i], indent_level + 1);
    }
    
    js << indent << "}" << (minify_ ? "" : "\n");
    
    return js.str();
}

std::string CHTLJSGenerator::generateWhileStatement(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node) return "";
    
    std::stringstream js;
    std::string indent = getIndent(indent_level);
    
    auto condition = generateExpression(node->getChildren()[0], indent_level);
    js << indent << "while (" << condition << ") {" << (minify_ ? "" : "\n");
    
    // 生成循环体
    for (size_t i = 1; i < node->getChildren().size(); i++) {
        js << generateStatement(node->getChildren()[i], indent_level + 1);
    }
    
    js << indent << "}" << (minify_ ? "" : "\n");
    
    return js.str();
}

std::string CHTLJSGenerator::generateCHTLJSFunction(std::shared_ptr<CHTLNode> node, int indent_level) {
    // 通用的CHTL JS函数生成
    return generateObjectLiteral(node, indent_level);
}

std::string CHTLJSGenerator::generateListenFunction(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node) return "";
    
    std::stringstream js;
    std::string indent = getIndent(indent_level);
    
    // 生成事件监听器代码
    js << indent << "// Event listener generated from CHTL JS" << (minify_ ? "" : "\n");
    
    for (const auto& child : node->getChildren()) {
        if (child->getName() == "event_handler") {
            std::string event = child->getName();
            std::string handler = generateExpression(child->getChildren()[0], indent_level);
            
            js << indent << "element.addEventListener('" << event << "', " << handler << ");" << (minify_ ? "" : "\n");
        }
    }
    
    return js.str();
}

std::string CHTLJSGenerator::generateAnimateFunction(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node) return "";
    
    std::stringstream js;
    std::string indent = getIndent(indent_level);
    
    // 生成动画代码
    js << indent << "// Animation generated from CHTL JS" << (minify_ ? "" : "\n");
    js << indent << "const animation = {" << (minify_ ? "" : "\n");
    
    for (const auto& child : node->getChildren()) {
        if (child->getName() == "property_value") {
            std::string property = child->getName();
            std::string value = generateExpression(child->getChildren()[0], indent_level + 1);
            
            js << getIndent(indent_level + 1) << property << ": " << value << "," << (minify_ ? "" : "\n");
        }
    }
    
    js << indent << "};" << (minify_ ? "" : "\n");
    
    return js.str();
}

std::string CHTLJSGenerator::generateDelegateFunction(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node) return "";
    
    std::stringstream js;
    std::string indent = getIndent(indent_level);
    
    // 生成事件委托代码
    js << indent << "// Event delegation generated from CHTL JS" << (minify_ ? "" : "\n");
    
    return js.str();
}

std::string CHTLJSGenerator::generateRouterFunction(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node) return "";
    
    std::stringstream js;
    std::string indent = getIndent(indent_level);
    
    // 生成路由代码
    js << indent << "// Router generated from CHTL JS" << (minify_ ? "" : "\n");
    
    return js.str();
}

std::string CHTLJSGenerator::generateFileloaderFunction(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node) return "";
    
    std::stringstream js;
    std::string indent = getIndent(indent_level);
    
    // 生成文件加载器代码
    js << indent << "// File loader generated from CHTL JS" << (minify_ ? "" : "\n");
    
    return js.str();
}

std::string CHTLJSGenerator::generateVirDeclaration(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node) return "";
    
    std::stringstream js;
    std::string indent = getIndent(indent_level);
    
    std::string name = node->getName();
    std::string unique_id = generateUniqueId("vir_");
    
    // 注册虚对象
    virtual_objects_[name] = unique_id;
    
    js << indent << "// Virtual object: " << name << (minify_ ? "" : "\n");
    js << indent << "const " << unique_id << " = " << generateExpression(node->getChildren()[0], indent_level) << ";" << (minify_ ? "" : "\n");
    
    return js.str();
}

std::string CHTLJSGenerator::generateINeverAwayFunction(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node) return "";
    
    std::stringstream js;
    std::string indent = getIndent(indent_level);
    
    // 生成iNeverAway函数代码
    js << indent << "// iNeverAway function generated from CHTL JS" << (minify_ ? "" : "\n");
    
    return js.str();
}

std::string CHTLJSGenerator::generateUtilExpression(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node) return "";
    
    std::stringstream js;
    std::string indent = getIndent(indent_level);
    
    auto condition = generateExpression(node->getChildren()[0], indent_level);
    auto change_expr = generateExpression(node->getChildren()[1], indent_level);
    auto then_expr = generateExpression(node->getChildren()[2], indent_level);
    
    // 生成util表达式代码
    js << indent << "// Util expression: " << condition << (minify_ ? "" : "\n");
    js << indent << "if (" << condition << ") {" << (minify_ ? "" : "\n");
    js << getIndent(indent_level + 1) << change_expr << ";" << (minify_ ? "" : "\n");
    js << indent << "} else {" << (minify_ ? "" : "\n");
    js << getIndent(indent_level + 1) << then_expr << ";" << (minify_ ? "" : "\n");
    js << indent << "}" << (minify_ ? "" : "\n");
    
    return js.str();
}

std::string CHTLJSGenerator::generateEnhancedSelector(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node) return "";
    
    std::string selector = node->getName();
    return processEnhancedSelector(selector);
}

std::string CHTLJSGenerator::processEnhancedSelector(const std::string& selector) {
    // 处理增强选择器 {{selector}}
    std::string result = selector;
    
    // 替换增强选择器语法
    std::regex selector_regex(R"(\{\{([^}]+)\}\})");
    result = std::regex_replace(result, selector_regex, "document.querySelector('$1')");
    
    return result;
}

std::string CHTLJSGenerator::processVirtualObject(std::shared_ptr<CHTLNode> node) {
    // 处理虚对象
    return generateExpression(node, 0);
}

std::string CHTLJSGenerator::processReactiveValues(const std::string& content) {
    // 处理响应式值 $variable$
    std::string result = content;
    
    std::regex reactive_regex(R"(\$([^$]+)\$)");
    result = std::regex_replace(result, reactive_regex, "reactive_values['$1']");
    
    return result;
}

std::string CHTLJSGenerator::getIndent(int level) const {
    if (minify_) return "";
    
    std::string result;
    for (int i = 0; i < level; i++) {
        result += indent_char_;
    }
    return result;
}

std::string CHTLJSGenerator::escapeJavaScript(const std::string& text) const {
    std::string result = text;
    
    // 替换JavaScript特殊字符
    size_t pos = 0;
    while ((pos = result.find('\\', pos)) != std::string::npos) {
        result.replace(pos, 1, "\\\\");
        pos += 2;
    }
    pos = 0;
    while ((pos = result.find('"', pos)) != std::string::npos) {
        result.replace(pos, 1, "\\\"");
        pos += 2;
    }
    pos = 0;
    while ((pos = result.find('\n', pos)) != std::string::npos) {
        result.replace(pos, 1, "\\n");
        pos += 2;
    }
    pos = 0;
    while ((pos = result.find('\r', pos)) != std::string::npos) {
        result.replace(pos, 1, "\\r");
        pos += 2;
    }
    pos = 0;
    while ((pos = result.find('\t', pos)) != std::string::npos) {
        result.replace(pos, 1, "\\t");
        pos += 2;
    }
    
    return result;
}

std::string CHTLJSGenerator::generateUniqueId(const std::string& prefix) {
    return prefix + std::to_string(unique_id_counter_++);
}

} // namespace CHTL