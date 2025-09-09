#include "CHTLJSGenerator.h"
#include <regex>
#include <algorithm>

namespace CHTL {

CHTLJSGenerator::CHTLJSGenerator() : currentIndent(0) {}

std::string CHTLJSGenerator::generate(std::shared_ptr<BaseNode> ast) {
    output.str("");
    currentIndent = 0;
    
    if (ast->type == NodeType::Program) {
        for (const auto& child : ast->children) {
            output << generateNode(child);
        }
    } else {
        output << generateNode(ast);
    }
    
    return output.str();
}

std::string CHTLJSGenerator::generateNode(std::shared_ptr<BaseNode> node, int indent) {
    currentIndent = indent;
    
    switch (node->type) {
        case NodeType::Script:
            return generateScriptNode(std::dynamic_pointer_cast<ScriptNode>(node), indent);
        case NodeType::FunctionCall:
            return generateFunctionCall(node, indent);
        case NodeType::ObjectLiteral:
            return generateObjectLiteral(node, indent);
        case NodeType::ArrayLiteral:
            return generateArrayLiteral(node, indent);
        case NodeType::ConditionalExpression:
            return generateConditionalExpression(node, indent);
        case NodeType::BinaryExpression:
            return generateBinaryExpression(node, indent);
        case NodeType::UnaryExpression:
            return generateUnaryExpression(node, indent);
        case NodeType::Property:
            return generateProperty(node, indent);
        case NodeType::Program:
            {
                std::string result;
                for (const auto& child : node->children) {
                    result += generateNode(child, indent);
                }
                return result;
            }
        default:
            return generateExpression(node, indent);
    }
}

std::string CHTLJSGenerator::generateScriptNode(std::shared_ptr<ScriptNode> scriptNode, int indent) {
    if (!scriptNode) return "";
    
    std::string result;
    std::string indentStr = getIndent(indent);
    
    switch (scriptNode->getScriptType()) {
        case ScriptType::FILELOADER:
            result = generateFileLoader(scriptNode, indent);
            break;
        case ScriptType::LISTEN:
            result = generateListen(scriptNode, indent);
            break;
        case ScriptType::DELEGATE:
            result = generateDelegate(scriptNode, indent);
            break;
        case ScriptType::ANIMATE:
            result = generateAnimate(scriptNode, indent);
            break;
        case ScriptType::VIR:
            result = generateVir(scriptNode, indent);
            break;
        case ScriptType::ROUTER:
            result = generateRouter(scriptNode, indent);
            break;
        case ScriptType::UTIL:
            result = generateUtil(scriptNode, indent);
            break;
        case ScriptType::INLINE:
        case ScriptType::GLOBAL:
        case ScriptType::CHTL_JS:
            result = processCHTLJSSyntax(scriptNode->getScriptContent());
            break;
        default:
            result = scriptNode->getScriptContent();
            break;
    }
    
    return result;
}

std::string CHTLJSGenerator::generateFileLoader(std::shared_ptr<ScriptNode> fileLoaderNode, int indent) {
    std::string indentStr = getIndent(indent);
    std::string result;
    
    result += indentStr + "// CHTL JS File Loader\n";
    result += indentStr + "(function() {\n";
    result += indentStr + "    const loadScript = (src) => {\n";
    result += indentStr + "        return new Promise((resolve, reject) => {\n";
    result += indentStr + "            const script = document.createElement('script');\n";
    result += indentStr + "            script.src = src;\n";
    result += indentStr + "            script.onload = resolve;\n";
    result += indentStr + "            script.onerror = reject;\n";
    result += indentStr + "            document.head.appendChild(script);\n";
    result += indentStr + "        });\n";
    result += indentStr + "    };\n\n";
    
    result += indentStr + "    const loadCSS = (href) => {\n";
    result += indentStr + "        return new Promise((resolve, reject) => {\n";
    result += indentStr + "            const link = document.createElement('link');\n";
    result += indentStr + "            link.rel = 'stylesheet';\n";
    result += indentStr + "            link.href = href;\n";
    result += indentStr + "            link.onload = resolve;\n";
    result += indentStr + "            link.onerror = reject;\n";
    result += indentStr + "            document.head.appendChild(link);\n";
    result += indentStr + "        });\n";
    result += indentStr + "    };\n\n";
    
    result += indentStr + "    const loadFiles = async () => {\n";
    for (const auto& path : fileLoaderNode->getFileLoaderPaths()) {
        if (path.ends_with(".css")) {
            result += indentStr + "        await loadCSS('" + path + "');\n";
        } else {
            result += indentStr + "        await loadScript('" + path + "');\n";
        }
    }
    result += indentStr + "    };\n\n";
    
    result += indentStr + "    loadFiles().catch(console.error);\n";
    result += indentStr + "})();\n";
    
    return result;
}

std::string CHTLJSGenerator::generateListen(std::shared_ptr<ScriptNode> listenNode, int indent) {
    std::string indentStr = getIndent(indent);
    std::string result;
    
    result += indentStr + "// Enhanced Event Listeners\n";
    for (const auto& handler : listenNode->getEventListeners()) {
        result += indentStr + "element.addEventListener('" + handler.first + "', " + handler.second + ");\n";
    }
    
    return result;
}

std::string CHTLJSGenerator::generateDelegate(std::shared_ptr<ScriptNode> delegateNode, int indent) {
    std::string indentStr = getIndent(indent);
    std::string result;
    
    result += indentStr + "// Event Delegation\n";
    for (const auto& delegate : delegateNode->getDelegates()) {
        result += indentStr + "parentElement.addEventListener('click', (e) => {\n";
        result += indentStr + "    if (e.target.matches('" + delegate.first + "')) {\n";
        result += indentStr + "        " + delegate.second + "\n";
        result += indentStr + "    }\n";
        result += indentStr + "});\n";
    }
    
    return result;
}

std::string CHTLJSGenerator::generateAnimate(std::shared_ptr<ScriptNode> animateNode, int indent) {
    std::string indentStr = getIndent(indent);
    std::string result;
    
    result += indentStr + "// CHTL JS Animation\n";
    result += indentStr + "const animation = {\n";
    result += indentStr + "    target: element,\n";
    result += indentStr + "    duration: 1000,\n";
    result += indentStr + "    easing: 'ease-in-out',\n";
    result += indentStr + "    start: {},\n";
    result += indentStr + "    end: {},\n";
    result += indentStr + "    play: function() {\n";
    result += indentStr + "        // Animation implementation\n";
    result += indentStr + "    }\n";
    result += indentStr + "};\n";
    
    return result;
}

std::string CHTLJSGenerator::generateVir(std::shared_ptr<ScriptNode> virNode, int indent) {
    std::string indentStr = getIndent(indent);
    std::string result;
    
    result += indentStr + "// Virtual Object\n";
    result += indentStr + "const virObject = {\n";
    for (const auto& vir : virNode->getVirtualObjects()) {
        result += indentStr + "    " + vir.first + ": " + vir.second + ",\n";
    }
    result += indentStr + "};\n";
    
    return result;
}

std::string CHTLJSGenerator::generateRouter(std::shared_ptr<ScriptNode> routerNode, int indent) {
    std::string indentStr = getIndent(indent);
    std::string result;
    
    result += indentStr + "// CHTL JS Router\n";
    result += indentStr + "const router = {\n";
    result += indentStr + "    routes: {\n";
    for (const auto& route : routerNode->getRoutes()) {
        result += indentStr + "        '" + route.first + "': " + route.second + ",\n";
    }
    result += indentStr + "    },\n";
    result += indentStr + "    navigate: (path) => {\n";
    result += indentStr + "        if (this.routes[path]) {\n";
    result += indentStr + "            this.routes[path]();\n";
    result += indentStr + "        }\n";
    result += indentStr + "    }\n";
    result += indentStr + "};\n";
    
    return result;
}

std::string CHTLJSGenerator::generateUtil(std::shared_ptr<ScriptNode> utilNode, int indent) {
    std::string indentStr = getIndent(indent);
    std::string result;
    
    result += indentStr + "// Util Expression\n";
    result += indentStr + processCHTLJSSyntax(utilNode->getScriptContent()) + "\n";
    
    return result;
}

std::string CHTLJSGenerator::generateSelector(std::shared_ptr<ScriptNode> selectorNode, int indent) {
    std::string indentStr = getIndent(indent);
    std::string result;
    
    for (const auto& selector : selectorNode->getSelectors()) {
        result += indentStr + generateSelectorCode(selector) + "\n";
    }
    
    return result;
}

std::string CHTLJSGenerator::generateExpression(std::shared_ptr<BaseNode> exprNode, int indent) {
    if (!exprNode) return "";
    
    switch (exprNode->type) {
        case NodeType::Identifier:
            return exprNode->getValue();
        case NodeType::Literal:
            return exprNode->getValue();
        default:
            return exprNode->getValue();
    }
}

std::string CHTLJSGenerator::generateFunctionCall(std::shared_ptr<BaseNode> funcNode, int indent) {
    std::string result = funcNode->getValue() + "(";
    
    bool first = true;
    for (const auto& child : funcNode->children) {
        if (!first) {
            result += ", ";
        }
        result += generateExpression(child, indent);
        first = false;
    }
    
    result += ")";
    return result;
}

std::string CHTLJSGenerator::generateObjectLiteral(std::shared_ptr<BaseNode> objNode, int indent) {
    std::string indentStr = getIndent(indent);
    std::string result = "{\n";
    
    bool first = true;
    for (const auto& child : objNode->children) {
        if (!first) {
            result += ",\n";
        }
        result += indentStr + "    " + generateProperty(child, indent + 1);
        first = false;
    }
    
    result += "\n" + indentStr + "}";
    return result;
}

std::string CHTLJSGenerator::generateArrayLiteral(std::shared_ptr<BaseNode> arrNode, int indent) {
    std::string result = "[";
    
    bool first = true;
    for (const auto& child : arrNode->children) {
        if (!first) {
            result += ", ";
        }
        result += generateExpression(child, indent);
        first = false;
    }
    
    result += "]";
    return result;
}

std::string CHTLJSGenerator::generateConditionalExpression(std::shared_ptr<BaseNode> condNode, int indent) {
    if (condNode->children.size() >= 3) {
        return generateExpression(condNode->children[0], indent) + " ? " +
               generateExpression(condNode->children[1], indent) + " : " +
               generateExpression(condNode->children[2], indent);
    }
    return "";
}

std::string CHTLJSGenerator::generateBinaryExpression(std::shared_ptr<BaseNode> binNode, int indent) {
    if (binNode->children.size() >= 2) {
        return generateExpression(binNode->children[0], indent) + " " + binNode->getValue() + " " +
               generateExpression(binNode->children[1], indent);
    }
    return "";
}

std::string CHTLJSGenerator::generateUnaryExpression(std::shared_ptr<BaseNode> unaryNode, int indent) {
    if (!unaryNode->children.empty()) {
        return unaryNode->getValue() + generateExpression(unaryNode->children[0], indent);
    }
    return "";
}

std::string CHTLJSGenerator::generateProperty(std::shared_ptr<BaseNode> propNode, int indent) {
    return propNode->getValue() + ": " + generateExpression(propNode->children[0], indent);
}

std::string CHTLJSGenerator::getIndent(int indent) {
    return std::string(indent * 4, ' ');
}

std::string CHTLJSGenerator::processCHTLJSSyntax(const std::string& content) {
    std::string result = content;
    
    // 处理增强选择器 {{selector}}
    result = processSelectors(result);
    
    // 处理箭头操作符 ->
    result = processArrows(result);
    
    return result;
}

std::string CHTLJSGenerator::processSelectors(const std::string& content) {
    std::string result = content;
    
    // 匹配 {{selector}} 模式
    std::regex selectorRegex(R"(\{\{([^}]+)\}\})");
    std::smatch match;
    
    while (std::regex_search(result, match, selectorRegex)) {
        std::string selector = match[1].str();
        std::string replacement = generateSelectorCode(selector);
        result = std::regex_replace(result, selectorRegex, replacement);
    }
    
    return result;
}

std::string CHTLJSGenerator::processArrows(const std::string& content) {
    std::string result = content;
    
    // 将 -> 替换为 .
    std::regex arrowRegex(R"(\->)");
    result = std::regex_replace(result, arrowRegex, ".");
    
    return result;
}

std::string CHTLJSGenerator::generateSelectorCode(const std::string& selector) {
    if (selector[0] == '.') {
        // 类选择器
        return "document.querySelector('" + selector + "')";
    } else if (selector[0] == '#') {
        // ID选择器
        return "document.getElementById('" + selector.substr(1) + "')";
    } else {
        // 标签选择器
        return "document.querySelector('" + selector + "')";
    }
}

std::string CHTLJSGenerator::generateEventHandlers(const std::map<std::string, std::string>& handlers) {
    std::string result;
    for (const auto& handler : handlers) {
        result += "element.addEventListener('" + handler.first + "', " + handler.second + ");\n";
    }
    return result;
}

std::string CHTLJSGenerator::generateAnimationConfig(const std::map<std::string, std::string>& config) {
    std::string result = "{\n";
    for (const auto& prop : config) {
        result += "    " + prop.first + ": " + prop.second + ",\n";
    }
    result += "}";
    return result;
}

std::string CHTLJSGenerator::generateRouteConfig(const std::map<std::string, std::string>& routes) {
    std::string result = "{\n";
    for (const auto& route : routes) {
        result += "    '" + route.first + "': " + route.second + ",\n";
    }
    result += "}";
    return result;
}

std::string CHTLJSGenerator::generateVirtualObjectConfig(const std::map<std::string, std::string>& config) {
    std::string result = "{\n";
    for (const auto& prop : config) {
        result += "    " + prop.first + ": " + prop.second + ",\n";
    }
    result += "}";
    return result;
}

} // namespace CHTL