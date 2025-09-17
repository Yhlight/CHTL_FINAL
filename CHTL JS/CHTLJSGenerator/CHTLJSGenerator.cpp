#include "CHTLJSGenerator.h"
#include <sstream>
#include <random>
#include <ctime>

namespace CHTLJS {

CHTLJSGenerator::CHTLJSGenerator(std::shared_ptr<CHTLJSContext> context)
    : context_(context), indentation_level_(0) {
}

CHTLJSGenerator::~CHTLJSGenerator() {
}

std::string CHTLJSGenerator::generate(std::shared_ptr<CHTLJSBaseNode> ast) {
    if (!ast) {
        return "";
    }
    
    switch (ast->getType()) {
        case CHTLJSBaseNode::NodeType::SCRIPT_LOADER:
            return generateScriptLoader(ast);
        case CHTLJSBaseNode::NodeType::LISTEN:
            return generateListen(ast);
        case CHTLJSBaseNode::NodeType::ANIMATE:
            return generateAnimate(ast);
        case CHTLJSBaseNode::NodeType::ROUTER:
            return generateRouter(ast);
        case CHTLJSBaseNode::NodeType::VIR:
            return generateVir(ast);
        case CHTLJSBaseNode::NodeType::UTIL_THEN:
            return generateUtilThen(ast);
        case CHTLJSBaseNode::NodeType::PRINTMYLOVE:
            return generatePrintMylove(ast);
        case CHTLJSBaseNode::NodeType::INEVERAWAY:
            return generateINeverAway(ast);
        case CHTLJSBaseNode::NodeType::RESPONSIVE_GET:
            return generateResponsiveGet(ast);
        case CHTLJSBaseNode::NodeType::RESPONSIVE_SET:
            return generateResponsiveSet(ast);
        case CHTLJSBaseNode::NodeType::CHTL_SELECTOR:
            return generateCHTLSelector(ast);
        case CHTLJSBaseNode::NodeType::CHTL_RESPONSIVE:
            return generateCHTLResponsive(ast);
        case CHTLJSBaseNode::NodeType::IDENTIFIER:
            return generateIdentifier(ast);
        case CHTLJSBaseNode::NodeType::LITERAL:
            return generateLiteral(ast);
        case CHTLJSBaseNode::NodeType::BINARY_EXPRESSION:
            return generateBinaryExpression(ast);
        case CHTLJSBaseNode::NodeType::UNARY_EXPRESSION:
            return generateUnaryExpression(ast);
        case CHTLJSBaseNode::NodeType::ASSIGNMENT:
            return generateAssignment(ast);
        case CHTLJSBaseNode::NodeType::FUNCTION_CALL:
            return generateFunctionCall(ast);
        case CHTLJSBaseNode::NodeType::BLOCK_STATEMENT:
            return generateBlock(ast);
        case CHTLJSBaseNode::NodeType::IF_STATEMENT:
            return generateIfStatement(ast);
        case CHTLJSBaseNode::NodeType::WHILE_STATEMENT:
            return generateWhileStatement(ast);
        case CHTLJSBaseNode::NodeType::FOR_STATEMENT:
            return generateForStatement(ast);
        case CHTLJSBaseNode::NodeType::RETURN_STATEMENT:
            return generateReturnStatement(ast);
        case CHTLJSBaseNode::NodeType::VARIABLE_DECLARATION:
            return generateVariableDeclaration(ast);
        case CHTLJSBaseNode::NodeType::FUNCTION_DECLARATION:
            return generateFunctionDeclaration(ast);
        default:
            return ast->generateCode();
    }
}

std::string CHTLJSGenerator::generateDocument(const std::vector<std::shared_ptr<CHTLJSBaseNode>>& statements) {
    std::stringstream ss;
    
    // 生成CHTL JS运行时系统
    ss << generateReactiveSystem() << "\n\n";
    ss << generateVirtualObjectSystem() << "\n\n";
    ss << generateEventSystem() << "\n\n";
    ss << generateAnimationSystem() << "\n\n";
    ss << generateRouterSystem() << "\n\n";
    ss << generateModuleSystem() << "\n\n";
    
    // 生成主代码
    ss << "// CHTL JS Generated Code\n";
    ss << "(function() {\n";
    increaseIndentation();
    
    for (const auto& statement : statements) {
        ss << getIndentString() << generate(statement) << ";\n";
    }
    
    decreaseIndentation();
    ss << "})();\n";
    
    return ss.str();
}

std::string CHTLJSGenerator::generateScriptLoader(std::shared_ptr<CHTLJSBaseNode> node) {
    auto scriptLoader = std::dynamic_pointer_cast<ScriptLoaderNode>(node);
    if (!scriptLoader) return "";
    
    std::stringstream ss;
    ss << "CHTLJS_ModuleLoader.load(\"" << scriptLoader->getModuleName() << "\", \"" 
       << scriptLoader->getModulePath() << "\"";
    
    if (scriptLoader->getCallback()) {
        ss << ", " << generate(scriptLoader->getCallback());
    }
    
    ss << ")";
    return ss.str();
}

std::string CHTLJSGenerator::generateListen(std::shared_ptr<CHTLJSBaseNode> node) {
    auto listen = std::dynamic_pointer_cast<ListenNode>(node);
    if (!listen) return "";
    
    std::stringstream ss;
    ss << "CHTLJS_EventSystem.listen(\"" << listen->getSelector() << "\", {\n";
    increaseIndentation();
    
    for (const auto& event : listen->getEvents()) {
        ss << getIndentString() << event.first << ": " << generate(event.second) << ",\n";
    }
    
    decreaseIndentation();
    ss << getIndentString() << "})";
    return ss.str();
}

std::string CHTLJSGenerator::generateAnimate(std::shared_ptr<CHTLJSBaseNode> node) {
    auto animate = std::dynamic_pointer_cast<AnimateNode>(node);
    if (!animate) return "";
    
    std::stringstream ss;
    ss << "CHTLJS_AnimationSystem.animate(\"" << animate->getTarget() << "\", {\n";
    increaseIndentation();
    ss << getIndentString() << "duration: \"" << animate->getDuration() << "\",\n";
    ss << getIndentString() << "easing: \"" << animate->getEasing() << "\",\n";
    ss << getIndentString() << "properties: {\n";
    increaseIndentation();
    
    for (const auto& prop : animate->getProperties()) {
        ss << getIndentString() << prop.first << ": \"" << prop.second << "\",\n";
    }
    
    decreaseIndentation();
    ss << getIndentString() << "}";
    
    if (animate->getCallback()) {
        ss << ",\n" << getIndentString() << "callback: " << generate(animate->getCallback());
    }
    
    decreaseIndentation();
    ss << "\n" << getIndentString() << "})";
    return ss.str();
}

std::string CHTLJSGenerator::generateRouter(std::shared_ptr<CHTLJSBaseNode> node) {
    auto router = std::dynamic_pointer_cast<RouterNode>(node);
    if (!router) return "";
    
    std::stringstream ss;
    ss << "CHTLJS_RouterSystem.router({\n";
    increaseIndentation();
    
    for (const auto& route : router->getRoutes()) {
        ss << getIndentString() << "\"" << route.first << "\": " << generate(route.second) << ",\n";
    }
    
    if (router->getDefaultRoute()) {
        ss << getIndentString() << "default: " << generate(router->getDefaultRoute()) << "\n";
    }
    
    decreaseIndentation();
    ss << getIndentString() << "})";
    return ss.str();
}

std::string CHTLJSGenerator::generateVir(std::shared_ptr<CHTLJSBaseNode> node) {
    auto vir = std::dynamic_pointer_cast<VirNode>(node);
    if (!vir) return "";
    
    std::stringstream ss;
    ss << "CHTLJS_VirtualObjectSystem.create(\"" << vir->getObjectName() << "\", {\n";
    increaseIndentation();
    
    if (!vir->getProperties().empty()) {
        ss << getIndentString() << "properties: {\n";
        increaseIndentation();
        for (const auto& prop : vir->getProperties()) {
            ss << getIndentString() << prop.first << ": " << generate(prop.second) << ",\n";
        }
        decreaseIndentation();
        ss << getIndentString() << "},\n";
    }
    
    if (!vir->getMethods().empty()) {
        ss << getIndentString() << "methods: {\n";
        increaseIndentation();
        for (const auto& method : vir->getMethods()) {
            ss << getIndentString() << method.first << ": " << generate(method.second) << ",\n";
        }
        decreaseIndentation();
        ss << getIndentString() << "}\n";
    }
    
    decreaseIndentation();
    ss << getIndentString() << "})";
    return ss.str();
}

std::string CHTLJSGenerator::generateUtilThen(std::shared_ptr<CHTLJSBaseNode> node) {
    auto utilThen = std::dynamic_pointer_cast<UtilThenNode>(node);
    if (!utilThen) return "";
    
    std::stringstream ss;
    if (utilThen->getTarget()) {
        ss << generate(utilThen->getTarget());
    }
    ss << ".util.then({\n";
    increaseIndentation();
    
    for (const auto& op : utilThen->getOperations()) {
        ss << getIndentString() << op.first << ": " << generate(op.second) << ",\n";
    }
    
    decreaseIndentation();
    ss << getIndentString() << "})";
    return ss.str();
}

std::string CHTLJSGenerator::generatePrintMylove(std::shared_ptr<CHTLJSBaseNode> node) {
    auto printMylove = std::dynamic_pointer_cast<PrintMyloveNode>(node);
    if (!printMylove) return "";
    
    std::stringstream ss;
    ss << "CHTLJS_ImageProcessor.printMylove({\n";
    increaseIndentation();
    ss << getIndentString() << "image: \"" << printMylove->getImagePath() << "\",\n";
    ss << getIndentString() << "width: " << printMylove->getWidth() << ",\n";
    ss << getIndentString() << "height: " << printMylove->getHeight() << ",\n";
    ss << getIndentString() << "charset: \"" << printMylove->getCharSet() << "\"\n";
    decreaseIndentation();
    ss << getIndentString() << "})";
    return ss.str();
}

std::string CHTLJSGenerator::generateINeverAway(std::shared_ptr<CHTLJSBaseNode> node) {
    auto iNeverAway = std::dynamic_pointer_cast<INeverAwayNode>(node);
    if (!iNeverAway) return "";
    
    std::stringstream ss;
    ss << "CHTLJS_FunctionOverloader.create(\"" << iNeverAway->getFunctionName() << "\", [\n";
    increaseIndentation();
    
    for (const auto& overload : iNeverAway->getOverloads()) {
        ss << getIndentString() << "{\n";
        increaseIndentation();
        ss << getIndentString() << "parameters: [";
        for (size_t i = 0; i < overload.first.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "\"" << overload.first[i] << "\"";
        }
        ss << "],\n";
        ss << getIndentString() << "body: " << generate(overload.second) << "\n";
        decreaseIndentation();
        ss << getIndentString() << "},\n";
    }
    
    decreaseIndentation();
    ss << getIndentString() << "])";
    return ss.str();
}

std::string CHTLJSGenerator::generateResponsiveGet(std::shared_ptr<CHTLJSBaseNode> node) {
    auto responsiveGet = std::dynamic_pointer_cast<ResponsiveGetNode>(node);
    if (!responsiveGet) return "";
    
    return "CHTLJS_ReactiveSystem.get(\"" + responsiveGet->getVariableName() + "\")";
}

std::string CHTLJSGenerator::generateResponsiveSet(std::shared_ptr<CHTLJSBaseNode> node) {
    auto responsiveSet = std::dynamic_pointer_cast<ResponsiveSetNode>(node);
    if (!responsiveSet) return "";
    
    std::stringstream ss;
    ss << "CHTLJS_ReactiveSystem.set(\"" << responsiveSet->getVariableName() << "\", ";
    if (responsiveSet->getValue()) {
        ss << generate(responsiveSet->getValue());
    }
    ss << ")";
    return ss.str();
}

std::string CHTLJSGenerator::generateCHTLSelector(std::shared_ptr<CHTLJSBaseNode> node) {
    auto selector = std::dynamic_pointer_cast<CHTLSelectorNode>(node);
    if (!selector) return "";
    
    return "document.querySelector(\"" + escapeString(selector->getSelector()) + "\")";
}

std::string CHTLJSGenerator::generateCHTLResponsive(std::shared_ptr<CHTLJSBaseNode> node) {
    auto responsive = std::dynamic_pointer_cast<CHTLResponsiveNode>(node);
    if (!responsive) return "";
    
    return "CHTLJS_ReactiveSystem.get(\"" + responsive->getVariableName() + "\")";
}

// 传统节点生成方法
std::string CHTLJSGenerator::generateIdentifier(std::shared_ptr<CHTLJSBaseNode> node) {
    return node->getAttribute("name");
}

std::string CHTLJSGenerator::generateLiteral(std::shared_ptr<CHTLJSBaseNode> node) {
    return node->getAttribute("value");
}

std::string CHTLJSGenerator::generateBinaryExpression(std::shared_ptr<CHTLJSBaseNode> node) {
    auto children = node->getChildren();
    if (children.size() < 3) return "";
    
    std::stringstream ss;
    ss << "(" << generate(children[0]) << " " << children[1]->getAttribute("operator") 
       << " " << generate(children[2]) << ")";
    return ss.str();
}

std::string CHTLJSGenerator::generateUnaryExpression(std::shared_ptr<CHTLJSBaseNode> node) {
    auto children = node->getChildren();
    if (children.size() < 2) return "";
    
    return node->getAttribute("operator") + generate(children[1]);
}

std::string CHTLJSGenerator::generateAssignment(std::shared_ptr<CHTLJSBaseNode> node) {
    auto children = node->getChildren();
    if (children.size() < 3) return "";
    
    return generate(children[0]) + " " + children[1]->getAttribute("operator") + " " + generate(children[2]);
}

std::string CHTLJSGenerator::generateFunctionCall(std::shared_ptr<CHTLJSBaseNode> node) {
    auto children = node->getChildren();
    if (children.empty()) return "";
    
    std::stringstream ss;
    ss << generate(children[0]) << "(";
    
    for (size_t i = 1; i < children.size(); ++i) {
        if (i > 1) ss << ", ";
        ss << generate(children[i]);
    }
    
    ss << ")";
    return ss.str();
}

std::string CHTLJSGenerator::generateBlock(std::shared_ptr<CHTLJSBaseNode> node) {
    return generateBlockContent(node->getChildren());
}

std::string CHTLJSGenerator::generateIfStatement(std::shared_ptr<CHTLJSBaseNode> node) {
    auto children = node->getChildren();
    if (children.empty()) return "";
    
    std::stringstream ss;
    ss << "if (" << generate(children[0]) << ") {\n";
    increaseIndentation();
    ss << generateBlockContent(std::vector<std::shared_ptr<CHTLJSBaseNode>>(children.begin() + 1, children.end()));
    decreaseIndentation();
    ss << getIndentString() << "}";
    return ss.str();
}

std::string CHTLJSGenerator::generateWhileStatement(std::shared_ptr<CHTLJSBaseNode> node) {
    auto children = node->getChildren();
    if (children.empty()) return "";
    
    std::stringstream ss;
    ss << "while (" << generate(children[0]) << ") {\n";
    increaseIndentation();
    ss << generateBlockContent(std::vector<std::shared_ptr<CHTLJSBaseNode>>(children.begin() + 1, children.end()));
    decreaseIndentation();
    ss << getIndentString() << "}";
    return ss.str();
}

std::string CHTLJSGenerator::generateForStatement(std::shared_ptr<CHTLJSBaseNode> node) {
    auto children = node->getChildren();
    if (children.size() < 4) return "";
    
    std::stringstream ss;
    ss << "for (" << generate(children[0]) << "; " << generate(children[1]) 
       << "; " << generate(children[2]) << ") {\n";
    increaseIndentation();
    ss << generateBlockContent(std::vector<std::shared_ptr<CHTLJSBaseNode>>(children.begin() + 3, children.end()));
    decreaseIndentation();
    ss << getIndentString() << "}";
    return ss.str();
}

std::string CHTLJSGenerator::generateReturnStatement(std::shared_ptr<CHTLJSBaseNode> node) {
    auto children = node->getChildren();
    if (children.empty()) return "return";
    
    return "return " + generate(children[0]);
}

std::string CHTLJSGenerator::generateVariableDeclaration(std::shared_ptr<CHTLJSBaseNode> node) {
    auto children = node->getChildren();
    if (children.empty()) return "";
    
    std::stringstream ss;
    ss << "var " << generate(children[0]);
    
    if (children.size() > 1) {
        ss << " = " << generate(children[1]);
    }
    
    return ss.str();
}

std::string CHTLJSGenerator::generateFunctionDeclaration(std::shared_ptr<CHTLJSBaseNode> node) {
    auto children = node->getChildren();
    if (children.size() < 2) return "";
    
    std::stringstream ss;
    ss << "function " << generate(children[0]) << "(";
    
    for (size_t i = 1; i < children.size() - 1; ++i) {
        if (i > 1) ss << ", ";
        ss << generate(children[i]);
    }
    
    ss << ") {\n";
    increaseIndentation();
    ss << generateBlockContent(std::vector<std::shared_ptr<CHTLJSBaseNode>>(children.begin() + children.size() - 1, children.end()));
    decreaseIndentation();
    ss << getIndentString() << "}";
    return ss.str();
}

void CHTLJSGenerator::setIndentation(int level) {
    indentation_level_ = level;
}

int CHTLJSGenerator::getIndentation() const {
    return indentation_level_;
}

std::string CHTLJSGenerator::getIndentString() const {
    return std::string(indentation_level_ * 4, ' ');
}

void CHTLJSGenerator::increaseIndentation() {
    indentation_level_++;
}

void CHTLJSGenerator::decreaseIndentation() {
    if (indentation_level_ > 0) {
        indentation_level_--;
    }
}

std::string CHTLJSGenerator::generateExpression(std::shared_ptr<CHTLJSBaseNode> node) {
    return generate(node);
}

std::string CHTLJSGenerator::generateStatement(std::shared_ptr<CHTLJSBaseNode> node) {
    return generate(node);
}

std::string CHTLJSGenerator::generateBlockContent(const std::vector<std::shared_ptr<CHTLJSBaseNode>>& statements) {
    std::stringstream ss;
    for (const auto& statement : statements) {
        ss << getIndentString() << generate(statement) << ";\n";
    }
    return ss.str();
}

std::string CHTLJSGenerator::generateReactiveSystem() {
    return R"(
// CHTL JS Reactive System
var CHTLJS_ReactiveSystem = (function() {
    var reactiveVars = {};
    var watchers = {};
    
    return {
        get: function(name) {
            return reactiveVars[name];
        },
        set: function(name, value) {
            reactiveVars[name] = value;
            if (watchers[name]) {
                watchers[name].forEach(function(callback) {
                    callback(value);
                });
            }
        },
        watch: function(name, callback) {
            if (!watchers[name]) {
                watchers[name] = [];
            }
            watchers[name].push(callback);
        }
    };
})();
)";
}

std::string CHTLJSGenerator::generateVirtualObjectSystem() {
    return R"(
// CHTL JS Virtual Object System
var CHTLJS_VirtualObjectSystem = (function() {
    var virtualObjects = {};
    
    return {
        create: function(name, config) {
            var obj = {
                properties: config.properties || {},
                methods: config.methods || {}
            };
            virtualObjects[name] = obj;
            return obj;
        },
        get: function(name) {
            return virtualObjects[name];
        }
    };
})();
)";
}

std::string CHTLJSGenerator::generateEventSystem() {
    return R"(
// CHTL JS Event System
var CHTLJS_EventSystem = (function() {
    return {
        listen: function(selector, events) {
            var elements = document.querySelectorAll(selector);
            elements.forEach(function(element) {
                Object.keys(events).forEach(function(eventType) {
                    element.addEventListener(eventType, events[eventType]);
                });
            });
        }
    };
})();
)";
}

std::string CHTLJSGenerator::generateAnimationSystem() {
    return R"(
// CHTL JS Animation System
var CHTLJS_AnimationSystem = (function() {
    return {
        animate: function(target, config) {
            var element = document.querySelector(target);
            if (element) {
                element.style.transition = 'all ' + config.duration + ' ' + config.easing;
                Object.keys(config.properties).forEach(function(prop) {
                    element.style[prop] = config.properties[prop];
                });
                if (config.callback) {
                    setTimeout(config.callback, parseInt(config.duration) * 1000);
                }
            }
        }
    };
})();
)";
}

std::string CHTLJSGenerator::generateRouterSystem() {
    return R"(
// CHTL JS Router System
var CHTLJS_RouterSystem = (function() {
    var routes = {};
    var currentRoute = null;
    
    return {
        router: function(routeConfig) {
            routes = routeConfig;
            window.addEventListener('hashchange', function() {
                var hash = window.location.hash.slice(1);
                if (routes[hash]) {
                    routes[hash]();
                } else if (routes.default) {
                    routes.default();
                }
            });
        }
    };
})();
)";
}

std::string CHTLJSGenerator::generateModuleSystem() {
    return R"(
// CHTL JS Module System
var CHTLJS_ModuleLoader = (function() {
    var modules = {};
    var loadedModules = {};
    
    return {
        load: function(name, path, callback) {
            if (loadedModules[name]) {
                if (callback) callback(loadedModules[name]);
                return;
            }
            
            var script = document.createElement('script');
            script.src = path;
            script.onload = function() {
                loadedModules[name] = modules[name];
                if (callback) callback(loadedModules[name]);
            };
            document.head.appendChild(script);
        },
        register: function(name, module) {
            modules[name] = module;
        }
    };
})();
)";
}

std::string CHTLJSGenerator::escapeString(const std::string& str) const {
    std::string result = str;
    // 简单的字符串转义
    size_t pos = 0;
    while ((pos = result.find("\"", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\\"");
        pos += 2;
    }
    return result;
}

std::string CHTLJSGenerator::generateUniqueId() const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(1000, 9999);
    return "chtljs_" + std::to_string(dis(gen));
}

bool CHTLJSGenerator::isCHTLJSSpecific(std::shared_ptr<CHTLJSBaseNode> node) const {
    if (!node) return false;
    
    return node->getType() == CHTLJSBaseNode::NodeType::SCRIPT_LOADER ||
           node->getType() == CHTLJSBaseNode::NodeType::LISTEN ||
           node->getType() == CHTLJSBaseNode::NodeType::ANIMATE ||
           node->getType() == CHTLJSBaseNode::NodeType::ROUTER ||
           node->getType() == CHTLJSBaseNode::NodeType::VIR ||
           node->getType() == CHTLJSBaseNode::NodeType::UTIL_THEN ||
           node->getType() == CHTLJSBaseNode::NodeType::PRINTMYLOVE ||
           node->getType() == CHTLJSBaseNode::NodeType::INEVERAWAY ||
           node->getType() == CHTLJSBaseNode::NodeType::RESPONSIVE_GET ||
           node->getType() == CHTLJSBaseNode::NodeType::RESPONSIVE_SET ||
           node->getType() == CHTLJSBaseNode::NodeType::CHTL_SELECTOR ||
           node->getType() == CHTLJSBaseNode::NodeType::CHTL_RESPONSIVE;
}

} // namespace CHTLJS