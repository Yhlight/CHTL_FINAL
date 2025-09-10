#include "chtl_js/chtl_js_ast.h"
#include <sstream>

namespace chtl {
namespace chtl_js {

// ProgramNode实现
std::string ProgramNode::to_string() const {
    std::ostringstream oss;
    oss << "Program(";
    for (size_t i = 0; i < body.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << body[i]->to_string();
    }
    oss << ")";
    return oss.str();
}

std::string ProgramNode::to_js() const {
    std::ostringstream oss;
    for (const auto& statement : body) {
        oss << statement->to_js() << ";\n";
    }
    return oss.str();
}

// IdentifierNode实现
std::string IdentifierNode::to_string() const {
    return "Identifier(" + name + ")";
}

std::string IdentifierNode::to_js() const {
    return name;
}

// LiteralNode实现
std::string LiteralNode::to_string() const {
    return "Literal(" + value + ")";
}

std::string LiteralNode::to_js() const {
    return value;
}

// BinaryExpressionNode实现
std::string BinaryExpressionNode::to_string() const {
    std::ostringstream oss;
    oss << "BinaryExpression(" << left->to_string() << " ";
    
    switch (operator_type) {
        case CHTLJSTokenType::PLUS: oss << "+"; break;
        case CHTLJSTokenType::MINUS: oss << "-"; break;
        case CHTLJSTokenType::MULTIPLY: oss << "*"; break;
        case CHTLJSTokenType::DIVIDE: oss << "/"; break;
        case CHTLJSTokenType::MODULO: oss << "%"; break;
        case CHTLJSTokenType::POWER: oss << "**"; break;
        case CHTLJSTokenType::ASSIGN: oss << "="; break;
        case CHTLJSTokenType::EQUAL: oss << "=="; break;
        case CHTLJSTokenType::NOT_EQUAL: oss << "!="; break;
        case CHTLJSTokenType::LESS: oss << "<"; break;
        case CHTLJSTokenType::LESS_EQUAL: oss << "<="; break;
        case CHTLJSTokenType::GREATER: oss << ">"; break;
        case CHTLJSTokenType::GREATER_EQUAL: oss << ">="; break;
        case CHTLJSTokenType::AND: oss << "&&"; break;
        case CHTLJSTokenType::OR: oss << "||"; break;
        case CHTLJSTokenType::NOT: oss << "!"; break;
        case CHTLJSTokenType::COLON: oss << ":"; break;
        case CHTLJSTokenType::QUESTION: oss << "?"; break;
        default: oss << "?"; break;
    }
    
    oss << " " << right->to_string() << ")";
    return oss.str();
}

std::string BinaryExpressionNode::to_js() const {
    std::ostringstream oss;
    oss << "(" << left->to_js() << " ";
    
    switch (operator_type) {
        case CHTLJSTokenType::PLUS: oss << "+"; break;
        case CHTLJSTokenType::MINUS: oss << "-"; break;
        case CHTLJSTokenType::MULTIPLY: oss << "*"; break;
        case CHTLJSTokenType::DIVIDE: oss << "/"; break;
        case CHTLJSTokenType::MODULO: oss << "%"; break;
        case CHTLJSTokenType::POWER: oss << "**"; break;
        case CHTLJSTokenType::ASSIGN: oss << "="; break;
        case CHTLJSTokenType::EQUAL: oss << "=="; break;
        case CHTLJSTokenType::NOT_EQUAL: oss << "!="; break;
        case CHTLJSTokenType::LESS: oss << "<"; break;
        case CHTLJSTokenType::LESS_EQUAL: oss << "<="; break;
        case CHTLJSTokenType::GREATER: oss << ">"; break;
        case CHTLJSTokenType::GREATER_EQUAL: oss << ">="; break;
        case CHTLJSTokenType::AND: oss << "&&"; break;
        case CHTLJSTokenType::OR: oss << "||"; break;
        case CHTLJSTokenType::NOT: oss << "!"; break;
        case CHTLJSTokenType::COLON: oss << ":"; break;
        case CHTLJSTokenType::QUESTION: oss << "?"; break;
        default: oss << "?"; break;
    }
    
    oss << " " << right->to_js() << ")";
    return oss.str();
}

// CallExpressionNode实现
std::string CallExpressionNode::to_string() const {
    std::ostringstream oss;
    oss << "CallExpression(" << callee->to_string() << "(";
    for (size_t i = 0; i < arguments.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << arguments[i]->to_string();
    }
    oss << "))";
    return oss.str();
}

std::string CallExpressionNode::to_js() const {
    std::ostringstream oss;
    oss << callee->to_js() << "(";
    for (size_t i = 0; i < arguments.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << arguments[i]->to_js();
    }
    oss << ")";
    return oss.str();
}

// MemberExpressionNode实现
std::string MemberExpressionNode::to_string() const {
    std::ostringstream oss;
    oss << "MemberExpression(" << object->to_string();
    if (computed) {
        oss << "[" << property->to_string() << "]";
    } else {
        oss << "." << property->to_string();
    }
    oss << ")";
    return oss.str();
}

std::string MemberExpressionNode::to_js() const {
    std::ostringstream oss;
    oss << object->to_js();
    if (computed) {
        oss << "[" << property->to_js() << "]";
    } else {
        oss << "." << property->to_js();
    }
    return oss.str();
}

// VariableDeclarationNode实现
std::string VariableDeclarationNode::to_string() const {
    std::ostringstream oss;
    switch (kind) {
        case CHTLJSTokenType::VAR: oss << "var "; break;
        case CHTLJSTokenType::LET: oss << "let "; break;
        case CHTLJSTokenType::CONST: oss << "const "; break;
        default: oss << "var "; break;
    }
    
    for (size_t i = 0; i < declarations.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << declarations[i]->to_string();
    }
    return oss.str();
}

std::string VariableDeclarationNode::to_js() const {
    std::ostringstream oss;
    switch (kind) {
        case CHTLJSTokenType::VAR: oss << "var "; break;
        case CHTLJSTokenType::LET: oss << "let "; break;
        case CHTLJSTokenType::CONST: oss << "const "; break;
        default: oss << "var "; break;
    }
    
    for (size_t i = 0; i < declarations.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << declarations[i]->to_js();
    }
    return oss.str();
}

// VariableDeclaratorNode实现
std::string VariableDeclaratorNode::to_string() const {
    std::ostringstream oss;
    oss << id->to_string();
    if (init) {
        oss << " = " << init->to_string();
    }
    return oss.str();
}

std::string VariableDeclaratorNode::to_js() const {
    std::ostringstream oss;
    oss << id->to_js();
    if (init) {
        oss << " = " << init->to_js();
    }
    return oss.str();
}

// FunctionDeclarationNode实现
std::string FunctionDeclarationNode::to_string() const {
    std::ostringstream oss;
    if (async) oss << "async ";
    oss << "function " << id->to_string() << "(";
    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << params[i]->to_string();
    }
    oss << ") " << body->to_string();
    return oss.str();
}

std::string FunctionDeclarationNode::to_js() const {
    std::ostringstream oss;
    if (async) oss << "async ";
    oss << "function " << id->to_js() << "(";
    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << params[i]->to_js();
    }
    oss << ") " << body->to_js();
    return oss.str();
}

// BlockStatementNode实现
std::string BlockStatementNode::to_string() const {
    std::ostringstream oss;
    oss << "BlockStatement{";
    for (const auto& statement : body) {
        oss << statement->to_string() << "; ";
    }
    oss << "}";
    return oss.str();
}

std::string BlockStatementNode::to_js() const {
    std::ostringstream oss;
    oss << "{\n";
    for (const auto& statement : body) {
        oss << "  " << statement->to_js() << ";\n";
    }
    oss << "}";
    return oss.str();
}

// CHTL JS特有语法节点实现

// FileloaderNode实现
std::string FileloaderNode::to_string() const {
    std::ostringstream oss;
    oss << "Fileloader([";
    for (size_t i = 0; i < load_files.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << load_files[i];
    }
    oss << "])";
    return oss.str();
}

std::string FileloaderNode::to_js() const {
    std::ostringstream oss;
    oss << "Promise.all([\n";
    for (size_t i = 0; i < load_files.size(); ++i) {
        if (i > 0) oss << ",\n";
        oss << "  import('" << load_files[i] << "')";
    }
    oss << "\n]).then(() => {\n";
    oss << "  console.log('All files loaded');\n";
    oss << "});";
    return oss.str();
}

// ListenNode实现
std::string ListenNode::to_string() const {
    std::ostringstream oss;
    oss << "Listen(event=" << event;
    if (selector) oss << ", selector=" << selector->to_string();
    if (handler) oss << ", handler=" << handler->to_string();
    oss << ")";
    return oss.str();
}

std::string ListenNode::to_js() const {
    std::ostringstream oss;
    oss << "document.addEventListener('" << event << "', ";
    if (selector) {
        oss << "function(event) {\n";
        oss << "  if (event.target.matches(" << selector->to_js() << ")) {\n";
        oss << "    " << handler->to_js() << ";\n";
        oss << "  }\n";
        oss << "}";
    } else {
        oss << handler->to_js();
    }
    oss << ");";
    return oss.str();
}

// DelegateNode实现
std::string DelegateNode::to_string() const {
    std::ostringstream oss;
    oss << "Delegate(parent=" << (parent_selector ? parent_selector->to_string() : "null");
    oss << ", targets=[";
    for(size_t i = 0; i < target_selectors.size(); ++i) {
        if(i > 0) oss << ", ";
        oss << target_selectors[i]->to_string();
    }
    oss << "])";
    return oss.str();
}

std::string DelegateNode::to_js() const {
    std::ostringstream oss;
    oss << "((parent) => {\n";
    oss << "  if (!parent) return;\n";
    oss << "  const targets = [" <<  (target_selectors.empty() ? "" : target_selectors[0]->to_js());
    for(size_t i = 1; i < target_selectors.size(); ++i) {
        oss << ", " << target_selectors[i]->to_js();
    }
    oss << "];\n";

    for(const auto& pair : handlers) {
        oss << "  parent.addEventListener('" << pair.first << "', (event) => {\n";
        oss << "    for (const target of targets) {\n";
        oss << "      if (event.target.matches(target)) {\n";
        oss << "        (" << pair.second->to_js() << ")(event);\n";
        oss << "        break;\n";
        oss << "      }\n";
        oss << "    }\n";
        oss << "  });\n";
    }
    oss << "})(" << (parent_selector ? parent_selector->to_js() : "document") << ");";
    return oss.str();
}


// AnimateNode实现
std::string AnimateNode::to_string() const {
    std::ostringstream oss;
    oss << "Animate(target=" << (target ? target->to_string() : "null");
    oss << ", duration=" << duration;
    oss << ", easing=" << easing << ")";
    return oss.str();
}

std::string AnimateNode::to_js() const {
    // This generates a self-executing anonymous function to encapsulate the animation logic.
    std::ostringstream oss;
    oss << "(() => {\n";
    oss << "  const targetElement = " << (target ? target->to_js() : "null") << ";\n";
    oss << "  if (!targetElement) return;\n";
    oss << "  const duration = " << duration << ";\n";
    oss << "  const delay = " << delay << ";\n";
    oss << "  const loop = " << loop << ";\n";
    oss << "  const callback = " << (callback ? callback->to_js() : "null") << ";\n\n";

    oss << "  const keyframes = [];\n";
    if (begin) {
        // This is a simplification. A real implementation would parse the object expression.
        oss << "  keyframes.push({ at: 0, styles: " << begin->to_js() << " });\n";
    }
    for (const auto& frame : when) {
        oss << "  keyframes.push(" << frame->to_js() << ");\n";
    }
    if (end) {
        oss << "  keyframes.push({ at: 1, styles: " << end->to_js() << " });\n";
    }
    oss << "  keyframes.sort((a, b) => a.at - b.at);\n\n";

    oss << "  function animate(time) {\n";
    oss << "    let timeFraction = (time - startTime) / duration;\n";
    oss << "    if (timeFraction > 1) timeFraction = 1;\n\n";

    oss << "    let currentFrame = keyframes[0];\n";
    oss << "    let nextFrame = keyframes[0];\n";
    oss << "    for (let i = 0; i < keyframes.length; i++) {\n";
    oss << "      if (timeFraction >= keyframes[i].at) {\n";
    oss << "        currentFrame = keyframes[i];\n";
    oss << "        nextFrame = keyframes[i+1] || keyframes[i];\n";
    oss << "      }\n";
    oss << "    }\n\n";

    oss << "    const frameFraction = (timeFraction - currentFrame.at) / (nextFrame.at - currentFrame.at);\n\n";

    oss << "    for (const prop in currentFrame.styles) {\n";
    oss << "      const startValue = parseFloat(currentFrame.styles[prop]);\n";
    oss << "      const endValue = parseFloat(nextFrame.styles[prop]);\n";
    oss << "      const currentValue = startValue + (endValue - startValue) * frameFraction;\n";
    // This is a simplification, assumes all properties are numeric and in 'px'
    oss << "      targetElement.style[prop] = currentValue + (prop !== 'opacity' ? 'px' : '');\n";
    oss << "    }\n\n";

    oss << "    if (timeFraction < 1) {\n";
    oss << "      requestAnimationFrame(animate);\n";
    oss << "    } else {\n";
    oss << "      if (loopCount > 0 || loop === -1) {\n";
    oss << "        if (loop !== -1) loopCount--;\n";
    oss << "        startTime = performance.now();\n";
    oss << "        requestAnimationFrame(animate);\n";
    oss << "      } else if (callback) {\n";
    oss << "        callback();\n";
    oss << "      }\n";
    oss << "    }\n";
    oss << "  }\n\n";

    oss << "  let loopCount = loop;\n";
    oss << "  let startTime;\n";
    oss << "  setTimeout(() => {\n";
    oss << "    startTime = performance.now();\n";
    oss << "    requestAnimationFrame(animate);\n";
    oss << "  }, delay);\n";

    oss << "})();";
    return oss.str();
}

// VirNode实现
std::string VirNode::to_string() const {
    std::ostringstream oss;
    oss << "Vir(" << name << " = " << value->to_string() << ")";
    return oss.str();
}

std::string VirNode::to_js() const {
    std::ostringstream oss;
    oss << "const " << name << " = " << value->to_js() << ";";
    return oss.str();
}

// SelectorNode实现
std::string SelectorNode::to_string() const {
    return "Selector(" + selector + ")";
}

std::string SelectorNode::to_js() const {
    if (is_self_ref) {
        return "this";
    }
    
    // 移除 {{ 和 }}
    std::string clean_selector = selector;
    if (clean_selector.length() >= 4 && 
        clean_selector.substr(0, 2) == "{{" && 
        clean_selector.substr(clean_selector.length() - 2) == "}}") {
        clean_selector = clean_selector.substr(2, clean_selector.length() - 4);
    }
    
    return "document.querySelector('" + clean_selector + "')";
}

// RouterNode实现
std::string RouterNode::to_string() const {
    std::ostringstream oss;
    oss << "Router(path=" << path;
    if (handler) oss << ", handler=" << handler->to_string();
    oss << ")";
    return oss.str();
}

std::string RouterNode::to_js() const {
    std::ostringstream oss;
    oss << "const router = {\n";
    oss << "  path: '" << path << "',\n";
    oss << "  handler: " << (handler ? handler->to_js() : "null") << "\n";
    oss << "};";
    return oss.str();
}

// INeverAwayNode实现
std::string INeverAwayNode::to_string() const {
    std::ostringstream oss;
    oss << "INeverAway{";
    for (const auto& pair : functions) {
        oss << pair.first << ": " << pair.second->to_string() << ", ";
    }
    oss << "}";
    return oss.str();
}

std::string INeverAwayNode::to_js() const {
    std::ostringstream oss;
    oss << "const iNeverAway = {\n";
    for (const auto& pair : functions) {
        oss << "  " << pair.first << ": " << pair.second->to_js() << ",\n";
    }
    oss << "};";
    return oss.str();
}

// UtilNode实现
std::string UtilNode::to_string() const {
    std::ostringstream oss;
    oss << "Util(condition=" << condition->to_string();
    if (change) oss << ", change=" << change->to_string();
    if (then) oss << ", then=" << then->to_string();
    oss << ")";
    return oss.str();
}

std::string UtilNode::to_js() const {
    std::ostringstream oss;
    oss << "const util = {\n";
    oss << "  condition: " << condition->to_js() << ",\n";
    if (change) {
        oss << "  change: " << change->to_js() << ",\n";
    }
    if (then) {
        oss << "  then: " << then->to_js() << "\n";
    }
    oss << "};";
    return oss.str();
}

} // namespace chtl_js
} // namespace chtl