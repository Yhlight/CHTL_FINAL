#include "CHTLJSGenerator.h"
#include <sstream>
#include <algorithm>

namespace CHTLJS {

CHTLJSGenerator::CHTLJSGenerator() {
}

CHTLJSGenerator::~CHTLJSGenerator() {
}

std::string CHTLJSGenerator::generate(const std::shared_ptr<ProgramNode>& program) {
    return generateProgram(program);
}

std::string CHTLJSGenerator::generate(const std::shared_ptr<ASTNode>& node) {
    if (!node) {
        return "";
    }
    
    switch (node->getType()) {
        case ASTNode::NodeType::PROGRAM:
            return generateProgram(std::dynamic_pointer_cast<ProgramNode>(node));
        case ASTNode::NodeType::IDENTIFIER:
            return generateIdentifier(std::dynamic_pointer_cast<IdentifierNode>(node));
        case ASTNode::NodeType::LITERAL:
            return generateLiteral(std::dynamic_pointer_cast<LiteralNode>(node));
        case ASTNode::NodeType::OBJECT_LITERAL:
            return generateObjectLiteral(std::dynamic_pointer_cast<ObjectLiteralNode>(node));
        case ASTNode::NodeType::FUNCTION_CALL:
            return generateFunctionCall(std::dynamic_pointer_cast<FunctionCallNode>(node));
        case ASTNode::NodeType::VIRTUAL_OBJECT:
            return generateVirtualObject(std::dynamic_pointer_cast<VirtualObjectNode>(node));
        case ASTNode::NodeType::LISTEN_EXPRESSION:
            return generateListen(std::dynamic_pointer_cast<ListenNode>(node));
        case ASTNode::NodeType::ANIMATE_EXPRESSION:
            return generateAnimate(std::dynamic_pointer_cast<AnimateNode>(node));
        case ASTNode::NodeType::ROUTER_EXPRESSION:
            return generateRouter(std::dynamic_pointer_cast<RouterNode>(node));
        case ASTNode::NodeType::SCRIPT_LOADER_EXPRESSION:
            return generateScriptLoader(std::dynamic_pointer_cast<ScriptLoaderNode>(node));
        case ASTNode::NodeType::DELEGATE_EXPRESSION:
            return generateDelegate(std::dynamic_pointer_cast<DelegateNode>(node));
        case ASTNode::NodeType::INEVERAWAY_EXPRESSION:
            return generateINeverAway(std::dynamic_pointer_cast<INeverAwayNode>(node));
        case ASTNode::NodeType::UTIL_EXPRESSION:
            return generateUtilExpression(std::dynamic_pointer_cast<UtilExpressionNode>(node));
        case ASTNode::NodeType::SELECTOR_EXPRESSION:
            return generateSelector(std::dynamic_pointer_cast<SelectorNode>(node));
        case ASTNode::NodeType::EVENT_BINDING:
            return generateEventBinding(std::dynamic_pointer_cast<EventBindingNode>(node));
        case ASTNode::NodeType::PROPERTY_ACCESS:
            return generatePropertyAccess(std::dynamic_pointer_cast<PropertyAccessNode>(node));
        default:
            return "/* Unknown node type */";
    }
}

void CHTLJSGenerator::setContext(std::shared_ptr<CHTLJSContext> context) {
    this->context = context;
}

std::shared_ptr<CHTLJSContext> CHTLJSGenerator::getContext() const {
    return context;
}

std::string CHTLJSGenerator::generateProgram(const std::shared_ptr<ProgramNode>& node) {
    std::ostringstream oss;
    
    for (const auto& statement : node->getStatements()) {
        std::string code = generate(statement);
        if (!code.empty()) {
            oss << code << ";\n";
        }
    }
    
    return oss.str();
}

std::string CHTLJSGenerator::generateIdentifier(const std::shared_ptr<IdentifierNode>& node) {
    return node->getName();
}

std::string CHTLJSGenerator::generateLiteral(const std::shared_ptr<LiteralNode>& node) {
    if (node->getLiteralType() == "string") {
        return "\"" + escapeString(node->getValue()) + "\"";
    }
    return node->getValue();
}

std::string CHTLJSGenerator::generateObjectLiteral(const std::shared_ptr<ObjectLiteralNode>& node) {
    std::ostringstream oss;
    oss << "{\n";
    
    const auto& properties = node->getProperties();
    size_t index = 0;
    for (const auto& pair : properties) {
        oss << indent(1) << pair.first << ": " << generate(pair.second);
        if (index < properties.size() - 1) {
            oss << ",";
        }
        oss << "\n";
        index++;
    }
    
    oss << "}";
    return oss.str();
}

std::string CHTLJSGenerator::generateFunctionCall(const std::shared_ptr<FunctionCallNode>& node) {
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

std::string CHTLJSGenerator::generateVirtualObject(const std::shared_ptr<VirtualObjectNode>& node) {
    // 虚对象在编译时会被替换为实际的函数引用
    // 这里生成一个临时的对象结构
    std::ostringstream oss;
    oss << "const " << node->getName() << " = " << generate(node->getDefinition()) << ";";
    return oss.str();
}

std::string CHTLJSGenerator::generateListen(const std::shared_ptr<ListenNode>& node) {
    std::ostringstream oss;
    oss << "Listen(" << generate(node->getEvents()) << ")";
    return oss.str();
}

std::string CHTLJSGenerator::generateAnimate(const std::shared_ptr<AnimateNode>& node) {
    return generateAnimationCode(node->getProperties());
}

std::string CHTLJSGenerator::generateRouter(const std::shared_ptr<RouterNode>& node) {
    return generateRouterCode(node->getProperties());
}

std::string CHTLJSGenerator::generateScriptLoader(const std::shared_ptr<ScriptLoaderNode>& node) {
    return generateScriptLoaderCode(node->getProperties());
}

std::string CHTLJSGenerator::generateDelegate(const std::shared_ptr<DelegateNode>& node) {
    return generateDelegateCode(node->getProperties());
}

std::string CHTLJSGenerator::generateINeverAway(const std::shared_ptr<INeverAwayNode>& node) {
    return generateINeverAwayCode(node->getFunctions());
}

std::string CHTLJSGenerator::generateUtilExpression(const std::shared_ptr<UtilExpressionNode>& node) {
    return generateUtilCode(node->getCondition(), node->getChangeBlock(), node->getThenBlock());
}

std::string CHTLJSGenerator::generateSelector(const std::shared_ptr<SelectorNode>& node) {
    return generateSelectorCode(node->getSelector());
}

std::string CHTLJSGenerator::generateEventBinding(const std::shared_ptr<EventBindingNode>& node) {
    return generateEventCode(node->getEventType(), generate(node->getHandler()));
}

std::string CHTLJSGenerator::generatePropertyAccess(const std::shared_ptr<PropertyAccessNode>& node) {
    return generate(node->getObject()) + "->" + node->getProperty();
}

std::string CHTLJSGenerator::generateSelectorCode(const std::string& selector) {
    // 处理选择器语法 {{selector}}
    if (selector.find("{{") != std::string::npos && selector.find("}}") != std::string::npos) {
        std::string cleanSelector = selector;
        cleanSelector = cleanSelector.substr(2, cleanSelector.length() - 4); // 移除 {{ }}
        
        // 根据选择器类型生成相应的DOM查询代码
        if (cleanSelector[0] == '#') {
            return "document.getElementById('" + cleanSelector.substr(1) + "')";
        } else if (cleanSelector[0] == '.') {
            return "document.getElementsByClassName('" + cleanSelector.substr(1) + "')[0]";
        } else if (cleanSelector[0] == '&') {
            return "this"; // 引用当前元素
        } else {
            return "document.querySelector('" + cleanSelector + "')";
        }
    }
    
    return "document.querySelector('" + selector + "')";
}

std::string CHTLJSGenerator::generateEventCode(const std::string& eventType, const std::string& handler) {
    std::ostringstream oss;
    oss << "element.addEventListener('" << eventType << "', " << handler << ");";
    return oss.str();
}

std::string CHTLJSGenerator::generateAnimationCode(const std::shared_ptr<ObjectLiteralNode>& properties) {
    std::ostringstream oss;
    oss << "const animation = {\n";
    
    const auto& props = properties->getProperties();
    size_t index = 0;
    for (const auto& pair : props) {
        oss << indent(1) << pair.first << ": " << generate(pair.second);
        if (index < props.size() - 1) {
            oss << ",";
        }
        oss << "\n";
        index++;
    }
    
    oss << "};\n";
    oss << "// Animation implementation would go here";
    return oss.str();
}

std::string CHTLJSGenerator::generateRouterCode(const std::shared_ptr<ObjectLiteralNode>& properties) {
    std::ostringstream oss;
    oss << "const router = {\n";
    
    const auto& props = properties->getProperties();
    size_t index = 0;
    for (const auto& pair : props) {
        oss << indent(1) << pair.first << ": " << generate(pair.second);
        if (index < props.size() - 1) {
            oss << ",";
        }
        oss << "\n";
        index++;
    }
    
    oss << "};\n";
    oss << "// Router implementation would go here";
    return oss.str();
}

std::string CHTLJSGenerator::generateScriptLoaderCode(const std::shared_ptr<ObjectLiteralNode>& properties) {
    std::ostringstream oss;
    oss << "const scriptLoader = {\n";
    
    const auto& props = properties->getProperties();
    size_t index = 0;
    for (const auto& pair : props) {
        oss << indent(1) << pair.first << ": " << generate(pair.second);
        if (index < props.size() - 1) {
            oss << ",";
        }
        oss << "\n";
        index++;
    }
    
    oss << "};\n";
    oss << "// ScriptLoader implementation would go here";
    return oss.str();
}

std::string CHTLJSGenerator::generateDelegateCode(const std::shared_ptr<ObjectLiteralNode>& properties) {
    std::ostringstream oss;
    oss << "const delegate = {\n";
    
    const auto& props = properties->getProperties();
    size_t index = 0;
    for (const auto& pair : props) {
        oss << indent(1) << pair.first << ": " << generate(pair.second);
        if (index < props.size() - 1) {
            oss << ",";
        }
        oss << "\n";
        index++;
    }
    
    oss << "};\n";
    oss << "// Delegate implementation would go here";
    return oss.str();
}

std::string CHTLJSGenerator::generateINeverAwayCode(const std::shared_ptr<ObjectLiteralNode>& functions) {
    std::ostringstream oss;
    oss << "const iNeverAway = {\n";
    
    const auto& funcs = functions->getProperties();
    size_t index = 0;
    for (const auto& pair : funcs) {
        oss << indent(1) << pair.first << ": " << generate(pair.second);
        if (index < funcs.size() - 1) {
            oss << ",";
        }
        oss << "\n";
        index++;
    }
    
    oss << "};\n";
    oss << "// iNeverAway implementation would go here";
    return oss.str();
}

std::string CHTLJSGenerator::generateUtilCode(const std::shared_ptr<ASTNode>& condition,
                                            const std::shared_ptr<ASTNode>& changeBlock,
                                            const std::shared_ptr<ASTNode>& thenBlock) {
    std::ostringstream oss;
    oss << "util(" << generate(condition) << ")\n";
    oss << "  .change(" << generate(changeBlock) << ")\n";
    oss << "  .then(" << generate(thenBlock) << ");";
    return oss.str();
}

std::string CHTLJSGenerator::escapeString(const std::string& str) {
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

std::string CHTLJSGenerator::indent(int level) {
    return std::string(level * 2, ' ');
}

bool CHTLJSGenerator::needsQuotes(const std::string& str) {
    // 简单的判断是否需要引号
    return !std::all_of(str.begin(), str.end(), [](char c) {
        return std::isalnum(c) || c == '_';
    });
}

} // namespace CHTLJS