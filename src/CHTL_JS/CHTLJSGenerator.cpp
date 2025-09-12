#include "CHTLJSGenerator.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

CHTLJSGenerator::CHTLJSGenerator() {}

CHTLJSGenerator::~CHTLJSGenerator() {}

std::string CHTLJSGenerator::generateJavaScript(const CHTLJSNode* ast) {
    std::stringstream js;
    
    for (const auto& child : ast->children) {
        switch (child->type) {
            case CHTLJSNode::NodeType::VIRTUAL_OBJECT:
                js << generateVirtualObject(static_cast<const VirtualObjectNode*>(child.get()));
                break;
            case CHTLJSNode::NodeType::LISTEN:
                js << generateListen(static_cast<const ListenNode*>(child.get()));
                break;
            case CHTLJSNode::NodeType::ANIMATE:
                js << generateAnimate(static_cast<const AnimateNode*>(child.get()));
                break;
            case CHTLJSNode::NodeType::FILELOADER:
                js << generateFileLoader(static_cast<const FileLoaderNode*>(child.get()));
                break;
            case CHTLJSNode::NodeType::ROUTER:
                js << generateRouter(static_cast<const RouterNode*>(child.get()));
                break;
            case CHTLJSNode::NodeType::INEVERAWAY:
                js << generateINeverAway(static_cast<const INeverAwayNode*>(child.get()));
                break;
            case CHTLJSNode::NodeType::PRINTMYLOVE:
                js << generatePrintMylove(static_cast<const PrintMyloveNode*>(child.get()));
                break;
            case CHTLJSNode::NodeType::UTIL_THEN:
                js << generateUtilThen(static_cast<const UtilThenNode*>(child.get()));
                break;
            case CHTLJSNode::NodeType::ENHANCED_SELECTOR:
                js << generateEnhancedSelector(static_cast<const EnhancedSelectorNode*>(child.get()));
                break;
            case CHTLJSNode::NodeType::REACTIVE_VALUE:
                js << generateReactiveValue(static_cast<const ReactiveValueNode*>(child.get()));
                break;
            case CHTLJSNode::NodeType::EXPRESSION:
                js << generateExpression(child.get());
                break;
            case CHTLJSNode::NodeType::OBJECT_LITERAL:
                js << generateObjectLiteral(child.get());
                break;
            case CHTLJSNode::NodeType::ARRAY_LITERAL:
                js << generateArrayLiteral(child.get());
                break;
            case CHTLJSNode::NodeType::FUNCTION_CALL:
                js << generateFunctionCall(child.get());
                break;
            default:
                break;
        }
    }
    
    return js.str();
}

std::string CHTLJSGenerator::generateVirtualObject(const VirtualObjectNode* node) {
    std::stringstream js;
    
    js << "// 虚对象: " << node->name << "\n";
    js << "const " << node->name << " = {\n";
    
    for (const auto& prop : node->properties) {
        js << "    " << prop.first << ": " << prop.second << ",\n";
    }
    
    js << "};\n\n";
    
    return js.str();
}

std::string CHTLJSGenerator::generateListen(const ListenNode* node) {
    std::stringstream js;
    
    js << "// 监听器: " << node->selector << "\n";
    js << generateSelectorQuery(node->selector) << ".addEventListener('click', function() {\n";
    
    for (const auto& event : node->events) {
        js << "    // " << event.first << ": " << event.second << "\n";
    }
    
    js << "});\n\n";
    
    return js.str();
}

std::string CHTLJSGenerator::generateAnimate(const AnimateNode* node) {
    std::stringstream js;
    
    js << "// 动画\n";
    js << "const anim = {\n";
    js << "    target: " << generateSelectorQuery(node->target) << ",\n";
    js << "    duration: " << node->duration << ",\n";
    js << "    easing: '" << node->easing << "',\n";
    
    if (!node->beginState.empty()) {
        js << "    begin: {\n";
        for (const auto& prop : node->beginState) {
            js << "        " << prop.first << ": '" << prop.second << "',\n";
        }
        js << "    },\n";
    }
    
    if (!node->whenStates.empty()) {
        js << "    when: [\n";
        for (const auto& state : node->whenStates) {
            js << "        {\n";
            for (const auto& prop : state) {
                js << "            " << prop.first << ": '" << prop.second << "',\n";
            }
            js << "        },\n";
        }
        js << "    ],\n";
    }
    
    if (!node->endState.empty()) {
        js << "    end: {\n";
        for (const auto& prop : node->endState) {
            js << "        " << prop.first << ": '" << prop.second << "',\n";
        }
        js << "    },\n";
    }
    
    js << "    loop: " << node->loop << ",\n";
    js << "    direction: '" << node->direction << "',\n";
    js << "    delay: " << node->delay << ",\n";
    js << "    callback: " << node->callback << "\n";
    js << "};\n\n";
    
    return js.str();
}

std::string CHTLJSGenerator::generateFileLoader(const FileLoaderNode* node) {
    std::stringstream js;
    
    js << "// 文件加载器\n";
    js << "const fileLoader = {\n";
    js << "    files: [\n";
    
    for (const auto& file : node->files) {
        js << "        '" << file << "',\n";
    }
    
    js << "    ],\n";
    js << "    load: function() {\n";
    js << "        // 加载文件逻辑\n";
    js << "    }\n";
    js << "};\n\n";
    
    return js.str();
}

std::string CHTLJSGenerator::generateRouter(const RouterNode* node) {
    std::stringstream js;
    
    js << "// 路由\n";
    js << "const router = {\n";
    js << "    routes: [\n";
    
    for (size_t i = 0; i < node->urls.size() && i < node->pages.size(); i++) {
        js << "        { url: '" << node->urls[i] << "', page: " << generateSelectorQuery(node->pages[i]) << " },\n";
    }
    
    js << "    ],\n";
    js << "    root: " << (node->root.empty() ? "'/'" : "'" + node->root + "'") << ",\n";
    js << "    mode: '" << node->mode << "',\n";
    js << "    navigate: function(url) {\n";
    js << "        // 路由导航逻辑\n";
    js << "    }\n";
    js << "};\n\n";
    
    return js.str();
}

std::string CHTLJSGenerator::generateINeverAway(const INeverAwayNode* node) {
    std::stringstream js;
    
    js << "// iNeverAway\n";
    js << "const iNeverAway = {\n";
    
    for (const auto& func : node->functions) {
        js << "    " << func.first << ": " << func.second << ",\n";
    }
    
    js << "};\n\n";
    
    return js.str();
}

std::string CHTLJSGenerator::generatePrintMylove(const PrintMyloveNode* node) {
    std::stringstream js;
    
    js << "// printMylove\n";
    js << "const printMylove = {\n";
    js << "    url: '" << node->url << "',\n";
    js << "    mode: '" << node->mode << "',\n";
    js << "    width: '" << node->width << "',\n";
    js << "    height: '" << node->height << "',\n";
    js << "    scale: '" << node->scale << "',\n";
    js << "    execute: function() {\n";
    js << "        // 图片转字符逻辑\n";
    js << "    }\n";
    js << "};\n\n";
    
    return js.str();
}

std::string CHTLJSGenerator::generateUtilThen(const UtilThenNode* node) {
    std::stringstream js;
    
    js << "// util...then表达式\n";
    js << "if (" << node->condition << ") {\n";
    js << "    " << node->changeAction << ";\n";
    js << "} else {\n";
    js << "    " << node->thenAction << ";\n";
    js << "}\n\n";
    
    return js.str();
}

std::string CHTLJSGenerator::generateEnhancedSelector(const EnhancedSelectorNode* node) {
    return generateSelectorQuery(node->selector);
}

std::string CHTLJSGenerator::generateReactiveValue(const ReactiveValueNode* node) {
    return node->variableName;
}

std::string CHTLJSGenerator::generateExpression(const CHTLJSNode* node) {
    return node->value;
}

std::string CHTLJSGenerator::generateObjectLiteral(const CHTLJSNode* node) {
    std::stringstream js;
    
    js << "{\n";
    for (const auto& prop : node->properties) {
        js << "    " << prop.first << ": " << prop.second << ",\n";
    }
    js << "}";
    
    return js.str();
}

std::string CHTLJSGenerator::generateArrayLiteral(const CHTLJSNode* node) {
    std::stringstream js;
    
    js << "[\n";
    for (const auto& child : node->children) {
        js << "    " << child->value << ",\n";
    }
    js << "]";
    
    return js.str();
}

std::string CHTLJSGenerator::generateFunctionCall(const CHTLJSNode* node) {
    std::stringstream js;
    
    js << node->value << "(";
    
    for (size_t i = 0; i < node->children.size(); i++) {
        if (i > 0) js << ", ";
        js << node->children[i]->value;
    }
    
    js << ")";
    
    return js.str();
}

std::string CHTLJSGenerator::generateCompleteJavaScript(const CHTLJSNode* ast) {
    std::stringstream js;
    
    js << "// CHTL JS Generated Code\n";
    js << "// ====================\n\n";
    
    js << generateJavaScript(ast);
    
    return js.str();
}

std::string CHTLJSGenerator::generateSelectorQuery(const std::string& selector) {
    // 处理增强选择器 {{选择器}}
    if (selector.find("{{") != std::string::npos) {
        std::string cleanSelector = selector;
        cleanSelector = std::regex_replace(cleanSelector, std::regex("\\{\\{|\\}\\}"), "");
        
        // 根据选择器类型生成不同的查询代码
        if (cleanSelector[0] == '.') {
            return "document.querySelector('" + cleanSelector + "')";
        } else if (cleanSelector[0] == '#') {
            return "document.getElementById('" + cleanSelector.substr(1) + "')";
        } else {
            return "document.querySelector('" + cleanSelector + "')";
        }
    }
    
    return "document.querySelector('" + selector + "')";
}

std::string CHTLJSGenerator::generateEventDelegate(const std::string& parentSelector, 
                                                  const std::vector<std::string>& childSelectors,
                                                  const std::map<std::string, std::string>& events) {
    std::stringstream js;
    
    js << "// 事件委托\n";
    js << generateSelectorQuery(parentSelector) << ".addEventListener('click', function(e) {\n";
    
    for (const auto& childSelector : childSelectors) {
        js << "    if (e.target.matches('" << childSelector << "')) {\n";
        js << "        // 处理点击事件\n";
        js << "    }\n";
    }
    
    js << "});\n\n";
    
    return js.str();
}

std::string CHTLJSGenerator::generateAnimationKeyframes(const std::vector<std::map<std::string, std::string>>& whenStates) {
    std::stringstream js;
    
    js << "const keyframes = [\n";
    
    for (const auto& state : whenStates) {
        js << "    {\n";
        for (const auto& prop : state) {
            js << "        " << prop.first << ": '" << prop.second << "',\n";
        }
        js << "    },\n";
    }
    
    js << "];\n\n";
    
    return js.str();
}

std::string CHTLJSGenerator::generateRouteHandler() {
    return R"(
// 路由处理器
function handleRoute() {
    const currentPath = window.location.pathname;
    const route = routes.find(r => r.url === currentPath);
    if (route) {
        route.page.style.display = 'block';
    }
}

window.addEventListener('popstate', handleRoute);
handleRoute();
)";
}

std::string CHTLJSGenerator::generateModuleLoader(const std::vector<std::string>& files) {
    std::stringstream js;
    
    js << "// 模块加载器\n";
    js << "const moduleLoader = {\n";
    js << "    files: [\n";
    
    for (const auto& file : files) {
        js << "        '" << file << "',\n";
    }
    
    js << "    ],\n";
    js << "    load: function() {\n";
    js << "        // AMD风格模块加载\n";
    js << "    }\n";
    js << "};\n\n";
    
    return js.str();
}

std::string CHTLJSGenerator::generateReactiveListener(const std::string& variableName) {
    return "// 响应式值监听: " + variableName + "\n";
}

std::string CHTLJSGenerator::generateConditionalExpression(const std::string& condition, 
                                                          const std::string& trueValue, 
                                                          const std::string& falseValue) {
    return "(" + condition + " ? " + trueValue + " : " + falseValue + ")";
}

std::string CHTLJSGenerator::generateArithmeticExpression(const std::string& left, 
                                                         const std::string& op, 
                                                         const std::string& right) {
    return "(" + left + " " + op + " " + right + ")";
}

std::string CHTLJSGenerator::generateLogicalExpression(const std::string& left, 
                                                      const std::string& op, 
                                                      const std::string& right) {
    return "(" + left + " " + op + " " + right + ")";
}

std::string CHTLJSGenerator::generateComparisonExpression(const std::string& left, 
                                                         const std::string& op, 
                                                         const std::string& right) {
    return "(" + left + " " + op + " " + right + ")";
}

std::string CHTLJSGenerator::generatePropertyAccess(const std::string& object, const std::string& property) {
    return object + "." + property;
}

std::string CHTLJSGenerator::generateFunctionDefinition(const std::string& name, 
                                                       const std::vector<std::string>& parameters, 
                                                       const std::string& body) {
    std::stringstream js;
    
    js << "function " << name << "(";
    
    for (size_t i = 0; i < parameters.size(); i++) {
        if (i > 0) js << ", ";
        js << parameters[i];
    }
    
    js << ") {\n";
    js << body;
    js << "}\n";
    
    return js.str();
}

std::string CHTLJSGenerator::generateVariableDeclaration(const std::string& name, 
                                                        const std::string& value, 
                                                        bool isConst) {
    return (isConst ? "const " : "let ") + name + " = " + value + ";";
}

std::string CHTLJSGenerator::generateComment(const std::string& comment) {
    return "// " + comment;
}

std::string CHTLJSGenerator::generateIndent(int level) {
    return std::string(level * 4, ' ');
}

std::string CHTLJSGenerator::escapeJavaScriptString(const std::string& str) {
    std::string result = str;
    result = std::regex_replace(result, std::regex("\\\\"), "\\\\");
    result = std::regex_replace(result, std::regex("\""), "\\\"");
    result = std::regex_replace(result, std::regex("\n"), "\\n");
    result = std::regex_replace(result, std::regex("\r"), "\\r");
    result = std::regex_replace(result, std::regex("\t"), "\\t");
    return result;
}

bool CHTLJSGenerator::isValidJavaScriptIdentifier(const std::string& identifier) {
    if (identifier.empty()) return false;
    
    // 检查第一个字符
    if (!std::isalpha(identifier[0]) && identifier[0] != '_' && identifier[0] != '$') {
        return false;
    }
    
    // 检查其余字符
    for (size_t i = 1; i < identifier.length(); i++) {
        if (!std::isalnum(identifier[i]) && identifier[i] != '_' && identifier[i] != '$') {
            return false;
        }
    }
    
    return true;
}

std::string CHTLJSGenerator::generateUniqueIdentifier(const std::string& prefix) {
    static size_t counter = 0;
    return prefix + std::to_string(++counter);
}

} // namespace CHTL