#include "CHTLJSGenerator.h"
#include "../CHTLJSLexer/CHTLJSLexer.h"
#include "../CHTLJSParser/CHTLJSParser.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace CHTLJS {

CHTLJSGenerator::CHTLJSGenerator(bool minifyOutput) 
    : indentLevel(0), minify(minifyOutput) {}

void CHTLJSGenerator::indent() {
    if (!minify) {
        for (int i = 0; i < indentLevel; ++i) {
            output += "    ";
        }
    }
}

void CHTLJSGenerator::newline() {
    if (!minify) {
        output += "\n";
    }
}

void CHTLJSGenerator::addIndent() {
    indentLevel++;
}

void CHTLJSGenerator::removeIndent() {
    if (indentLevel > 0) {
        indentLevel--;
    }
}

std::string CHTLJSGenerator::escapeString(const std::string& str) const {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string CHTLJSGenerator::formatNumber(const std::string& num) const {
    // 简单的数字格式化，可以扩展
    return num;
}

std::string CHTLJSGenerator::formatBoolean(const std::string& boolVal) const {
    return boolVal;
}

std::string CHTLJSGenerator::generate(std::shared_ptr<ASTNode> ast) {
    output.clear();
    indentLevel = 0;
    
    if (!ast) {
        return "";
    }
    
    return generateProgram(ast);
}

std::string CHTLJSGenerator::generate(const std::string& source) {
    CHTLJSLexer lexer(source);
    auto tokens = lexer.tokenize();
    CHTLJSParser parser(tokens);
    auto ast = parser.parse();
    return generate(ast);
}

std::string CHTLJSGenerator::generateProgram(std::shared_ptr<ASTNode> node) {
    if (!node || node->type != ASTNodeType::PROGRAM) {
        return "";
    }
    
    for (const auto& child : node->children) {
        if (child) {
            std::string code = generateStatement(child);
            if (!code.empty()) {
                indent();
                output += code;
                if (child->type != ASTNodeType::BLOCK_STATEMENT) {
                    output += ";";
                }
                newline();
            }
        }
    }
    
    return output;
}

std::string CHTLJSGenerator::generateStatement(std::shared_ptr<ASTNode> node) {
    if (!node) return "";
    
    switch (node->type) {
        case ASTNodeType::VARIABLE_DECLARATION:
            return generateVariableDeclaration(node);
        case ASTNodeType::FUNCTION_DECLARATION:
            return generateFunctionDeclaration(node);
        case ASTNodeType::CLASS_DECLARATION:
            return generateClassDeclaration(node);
        case ASTNodeType::EXPRESSION_STATEMENT:
            return generateExpressionStatement(node);
        case ASTNodeType::BLOCK_STATEMENT:
            return generateBlockStatement(node);
        case ASTNodeType::IF_STATEMENT:
            return generateIfStatement(node);
        case ASTNodeType::WHILE_STATEMENT:
            return generateWhileStatement(node);
        case ASTNodeType::FOR_STATEMENT:
            return generateForStatement(node);
        case ASTNodeType::RETURN_STATEMENT:
            return generateReturnStatement(node);
        case ASTNodeType::BREAK_STATEMENT:
            return generateBreakStatement(node);
        case ASTNodeType::CONTINUE_STATEMENT:
            return generateContinueStatement(node);
        case ASTNodeType::TRY_STATEMENT:
            return generateTryStatement(node);
        case ASTNodeType::THROW_STATEMENT:
            return generateThrowStatement(node);
        case ASTNodeType::VIR_DECLARATION:
            return generateVirDeclaration(node);
        default:
            return generateExpression(node);
    }
}

std::string CHTLJSGenerator::generateExpression(std::shared_ptr<ASTNode> node) {
    if (!node) return "";
    
    switch (node->type) {
        case ASTNodeType::IDENTIFIER:
            return generateIdentifier(node);
        case ASTNodeType::LITERAL:
            return generateLiteral(node);
        case ASTNodeType::BINARY_EXPRESSION:
            return generateBinaryExpression(node);
        case ASTNodeType::UNARY_EXPRESSION:
            return generateUnaryExpression(node);
        case ASTNodeType::ASSIGNMENT_EXPRESSION:
            return generateAssignmentExpression(node);
        case ASTNodeType::CALL_EXPRESSION:
            return generateCallExpression(node);
        case ASTNodeType::MEMBER_EXPRESSION:
            return generateMemberExpression(node);
        case ASTNodeType::ARRAY_EXPRESSION:
            return generateArrayExpression(node);
        case ASTNodeType::OBJECT_EXPRESSION:
            return generateObjectExpression(node);
        case ASTNodeType::CONDITIONAL_EXPRESSION:
            return generateConditionalExpression(node);
        case ASTNodeType::LISTEN_EXPRESSION:
            return generateListenExpression(node);
        case ASTNodeType::ANIMATE_EXPRESSION:
            return generateAnimateExpression(node);
        case ASTNodeType::ROUTER_EXPRESSION:
            return generateRouterExpression(node);
        case ASTNodeType::SCRIPT_LOADER_EXPRESSION:
            return generateScriptLoaderExpression(node);
        case ASTNodeType::DELEGATE_EXPRESSION:
            return generateDelegateExpression(node);
        case ASTNodeType::SELECTOR_EXPRESSION:
            return generateSelectorExpression(node);
        case ASTNodeType::REACTIVE_VALUE_EXPRESSION:
            return generateReactiveValueExpression(node);
        default:
            return "";
    }
}

std::string CHTLJSGenerator::generateVariableDeclaration(std::shared_ptr<ASTNode> node) {
    auto varDecl = std::dynamic_pointer_cast<VariableDeclarationNode>(node);
    if (!varDecl) return "";
    
    std::string result = varDecl->variableType + " " + varDecl->name;
    if (varDecl->initializer) {
        result += " = " + generateExpression(varDecl->initializer);
    }
    return result;
}

std::string CHTLJSGenerator::generateFunctionDeclaration(std::shared_ptr<ASTNode> node) {
    auto funcDecl = std::dynamic_pointer_cast<FunctionDeclarationNode>(node);
    if (!funcDecl) return "";
    
    std::string result = "function " + funcDecl->name + "(";
    result += generateParameterList(funcDecl->parameters);
    result += ")";
    
    if (funcDecl->body) {
        result += " " + generateStatement(funcDecl->body);
    }
    
    return result;
}

std::string CHTLJSGenerator::generateClassDeclaration(std::shared_ptr<ASTNode> node) {
    // 简化实现
    return "class " + node->token.value + " {}";
}

std::string CHTLJSGenerator::generateIdentifier(std::shared_ptr<ASTNode> node) {
    auto identifier = std::dynamic_pointer_cast<IdentifierNode>(node);
    if (!identifier) return "";
    
    return identifier->name;
}

std::string CHTLJSGenerator::generateLiteral(std::shared_ptr<ASTNode> node) {
    auto literal = std::dynamic_pointer_cast<LiteralNode>(node);
    if (!literal) return "";
    
    switch (literal->literalType) {
        case TokenType::STRING:
            return "\"" + escapeString(literal->value) + "\"";
        case TokenType::NUMBER:
            return formatNumber(literal->value);
        case TokenType::BOOLEAN:
            return formatBoolean(literal->value);
        default:
            return literal->value;
    }
}

std::string CHTLJSGenerator::generateBinaryExpression(std::shared_ptr<ASTNode> node) {
    auto binary = std::dynamic_pointer_cast<BinaryExpressionNode>(node);
    if (!binary) return "";
    
    std::string left = binary->left ? generateExpression(binary->left) : "";
    std::string right = binary->right ? generateExpression(binary->right) : "";
    std::string op = getOperatorString(binary->operatorType);
    
    return left + " " + op + " " + right;
}

std::string CHTLJSGenerator::generateUnaryExpression(std::shared_ptr<ASTNode> node) {
    // 简化实现
    return "!" + generateExpression(node->getChild(0));
}

std::string CHTLJSGenerator::generateAssignmentExpression(std::shared_ptr<ASTNode> node) {
    auto binary = std::dynamic_pointer_cast<BinaryExpressionNode>(node);
    if (!binary) return "";
    
    std::string left = binary->left ? generateExpression(binary->left) : "";
    std::string right = binary->right ? generateExpression(binary->right) : "";
    std::string op = getOperatorString(binary->operatorType);
    
    return left + " " + op + " " + right;
}

std::string CHTLJSGenerator::generateCallExpression(std::shared_ptr<ASTNode> node) {
    auto call = std::dynamic_pointer_cast<CallExpressionNode>(node);
    if (!call) return "";
    
    std::string result = generateExpression(call->callee) + "(";
    result += generateArgumentList(call->arguments);
    result += ")";
    
    return result;
}

std::string CHTLJSGenerator::generateMemberExpression(std::shared_ptr<ASTNode> node) {
    auto member = std::dynamic_pointer_cast<MemberExpressionNode>(node);
    if (!member) return "";
    
    std::string result = generateExpression(member->object);
    
    if (member->computed) {
        result += "[" + generateExpression(member->property) + "]";
    } else {
        result += "." + generateExpression(member->property);
    }
    
    return result;
}

std::string CHTLJSGenerator::generateArrayExpression(std::shared_ptr<ASTNode> node) {
    std::string result = "[";
    
    for (size_t i = 0; i < node->children.size(); ++i) {
        if (i > 0) result += ", ";
        result += generateExpression(node->children[i]);
    }
    
    result += "]";
    return result;
}

std::string CHTLJSGenerator::generateObjectExpression(std::shared_ptr<ASTNode> node) {
    std::string result = "{";
    
    if (!minify) {
        result += "\n";
        addIndent();
    }
    
    for (size_t i = 0; i < node->children.size(); ++i) {
        if (i > 0) {
            result += ",";
            if (!minify) result += "\n";
        }
        
        if (!minify) indent();
        result += generateProperty(node->children[i]);
    }
    
    if (!minify) {
        removeIndent();
        result += "\n";
        indent();
    }
    
    result += "}";
    return result;
}

std::string CHTLJSGenerator::generateConditionalExpression(std::shared_ptr<ASTNode> node) {
    auto condition = node->getProperty("condition");
    auto thenBranch = node->getProperty("then");
    auto elseBranch = node->getProperty("else");
    
    std::string result = generateExpression(condition) + " ? ";
    result += generateExpression(thenBranch) + " : ";
    result += generateExpression(elseBranch);
    
    return result;
}

std::string CHTLJSGenerator::generateExpressionStatement(std::shared_ptr<ASTNode> node) {
    if (node->children.empty()) return "";
    return generateExpression(node->children[0]);
}

std::string CHTLJSGenerator::generateBlockStatement(std::shared_ptr<ASTNode> node) {
    std::string result = "{";
    
    if (!minify) {
        result += "\n";
        addIndent();
    }
    
    for (const auto& child : node->children) {
        if (child) {
            if (!minify) indent();
            result += generateStatement(child);
            if (child->type != ASTNodeType::BLOCK_STATEMENT) {
                result += ";";
            }
            if (!minify) result += "\n";
        }
    }
    
    if (!minify) {
        removeIndent();
        indent();
    }
    
    result += "}";
    return result;
}

std::string CHTLJSGenerator::generateIfStatement(std::shared_ptr<ASTNode> node) {
    auto condition = node->getProperty("condition");
    auto thenBranch = node->getProperty("then");
    auto elseBranch = node->getProperty("else");
    
    std::string result = "if (" + generateExpression(condition) + ") ";
    result += generateStatement(thenBranch);
    
    if (elseBranch) {
        result += " else " + generateStatement(elseBranch);
    }
    
    return result;
}

std::string CHTLJSGenerator::generateWhileStatement(std::shared_ptr<ASTNode> node) {
    auto condition = node->getProperty("condition");
    auto body = node->getProperty("body");
    
    std::string result = "while (" + generateExpression(condition) + ") ";
    result += generateStatement(body);
    
    return result;
}

std::string CHTLJSGenerator::generateForStatement(std::shared_ptr<ASTNode> node) {
    auto init = node->getProperty("init");
    auto condition = node->getProperty("condition");
    auto increment = node->getProperty("increment");
    auto body = node->getProperty("body");
    
    std::string result = "for (";
    result += init ? generateExpression(init) : "";
    result += "; ";
    result += condition ? generateExpression(condition) : "";
    result += "; ";
    result += increment ? generateExpression(increment) : "";
    result += ") ";
    result += generateStatement(body);
    
    return result;
}

std::string CHTLJSGenerator::generateReturnStatement(std::shared_ptr<ASTNode> node) {
    auto value = node->getProperty("value");
    std::string result = "return";
    if (value) {
        result += " " + generateExpression(value);
    }
    return result;
}

std::string CHTLJSGenerator::generateBreakStatement(std::shared_ptr<ASTNode> node) {
    return "break";
}

std::string CHTLJSGenerator::generateContinueStatement(std::shared_ptr<ASTNode> node) {
    return "continue";
}

std::string CHTLJSGenerator::generateTryStatement(std::shared_ptr<ASTNode> node) {
    auto tryBlock = node->getProperty("try");
    auto catchBlock = node->getProperty("catch");
    auto finallyBlock = node->getProperty("finally");
    
    std::string result = "try " + generateStatement(tryBlock);
    
    if (catchBlock) {
        result += " catch (e) " + generateStatement(catchBlock);
    }
    
    if (finallyBlock) {
        result += " finally " + generateStatement(finallyBlock);
    }
    
    return result;
}

std::string CHTLJSGenerator::generateThrowStatement(std::shared_ptr<ASTNode> node) {
    auto value = node->getProperty("value");
    std::string result = "throw ";
    if (value) {
        result += generateExpression(value);
    }
    return result;
}

// CHTL JS 特有生成方法
std::string CHTLJSGenerator::generateVirDeclaration(std::shared_ptr<ASTNode> node) {
    auto vir = std::dynamic_pointer_cast<VirDeclarationNode>(node);
    if (!vir) return "";
    
    std::string result = "const " + vir->name + " = ";
    if (vir->value) {
        result += generateExpression(vir->value);
    }
    return result;
}

std::string CHTLJSGenerator::generateListenExpression(std::shared_ptr<ASTNode> node) {
    auto listen = std::dynamic_pointer_cast<ListenExpressionNode>(node);
    if (!listen) return "";
    
    std::string result = "Listen({";
    
    if (!minify) {
        result += "\n";
        addIndent();
    }
    
    bool first = true;
    for (const auto& pair : listen->events) {
        if (!first) {
            result += ",";
            if (!minify) result += "\n";
        }
        
        if (!minify) indent();
        result += pair.first + ": ";
        if (pair.second) {
            result += generateExpression(pair.second);
        }
        first = false;
    }
    
    if (!minify) {
        removeIndent();
        result += "\n";
        indent();
    }
    
    result += "})";
    return result;
}

std::string CHTLJSGenerator::generateAnimateExpression(std::shared_ptr<ASTNode> node) {
    auto animate = std::dynamic_pointer_cast<AnimateExpressionNode>(node);
    if (!animate) return "";
    
    std::string result = "Animate({";
    
    if (!minify) {
        result += "\n";
        addIndent();
    }
    
    bool first = true;
    
    if (animate->target) {
        if (!first) result += ",";
        if (!minify) result += "\n";
        if (!minify) indent();
        result += "target: " + generateExpression(animate->target);
        first = false;
    }
    
    if (animate->duration) {
        if (!first) result += ",";
        if (!minify) result += "\n";
        if (!minify) indent();
        result += "duration: " + generateExpression(animate->duration);
        first = false;
    }
    
    if (animate->easing) {
        if (!first) result += ",";
        if (!minify) result += "\n";
        if (!minify) indent();
        result += "easing: " + generateExpression(animate->easing);
        first = false;
    }
    
    if (animate->begin) {
        if (!first) result += ",";
        if (!minify) result += "\n";
        if (!minify) indent();
        result += "begin: " + generateExpression(animate->begin);
        first = false;
    }
    
    if (!animate->when.empty()) {
        if (!first) result += ",";
        if (!minify) result += "\n";
        if (!minify) indent();
        result += "when: [";
        for (size_t i = 0; i < animate->when.size(); ++i) {
            if (i > 0) result += ", ";
            result += generateExpression(animate->when[i]);
        }
        result += "]";
        first = false;
    }
    
    if (animate->end) {
        if (!first) result += ",";
        if (!minify) result += "\n";
        if (!minify) indent();
        result += "end: " + generateExpression(animate->end);
        first = false;
    }
    
    if (animate->loop) {
        if (!first) result += ",";
        if (!minify) result += "\n";
        if (!minify) indent();
        result += "loop: " + generateExpression(animate->loop);
        first = false;
    }
    
    if (animate->direction) {
        if (!first) result += ",";
        if (!minify) result += "\n";
        if (!minify) indent();
        result += "direction: " + generateExpression(animate->direction);
        first = false;
    }
    
    if (animate->delay) {
        if (!first) result += ",";
        if (!minify) result += "\n";
        if (!minify) indent();
        result += "delay: " + generateExpression(animate->delay);
        first = false;
    }
    
    if (animate->callback) {
        if (!first) result += ",";
        if (!minify) result += "\n";
        if (!minify) indent();
        result += "callback: " + generateExpression(animate->callback);
        first = false;
    }
    
    if (!minify) {
        removeIndent();
        result += "\n";
        indent();
    }
    
    result += "})";
    return result;
}

std::string CHTLJSGenerator::generateRouterExpression(std::shared_ptr<ASTNode> node) {
    std::string result = "Router({";
    
    if (!minify) {
        result += "\n";
        addIndent();
    }
    
    auto keys = node->getPropertyKeys();
    for (size_t i = 0; i < keys.size(); ++i) {
        if (i > 0) {
            result += ",";
            if (!minify) result += "\n";
        }
        
        if (!minify) indent();
        auto value = node->getProperty(keys[i]);
        result += keys[i] + ": ";
        if (value) {
            result += generateExpression(value);
        }
    }
    
    if (!minify) {
        removeIndent();
        result += "\n";
        indent();
    }
    
    result += "})";
    return result;
}

std::string CHTLJSGenerator::generateScriptLoaderExpression(std::shared_ptr<ASTNode> node) {
    std::string result = "ScriptLoader({";
    
    if (!minify) {
        result += "\n";
        addIndent();
    }
    
    auto keys = node->getPropertyKeys();
    for (size_t i = 0; i < keys.size(); ++i) {
        if (i > 0) {
            result += ",";
            if (!minify) result += "\n";
        }
        
        if (!minify) indent();
        auto value = node->getProperty(keys[i]);
        result += keys[i] + ": ";
        if (value) {
            result += generateExpression(value);
        }
    }
    
    if (!minify) {
        removeIndent();
        result += "\n";
        indent();
    }
    
    result += "})";
    return result;
}

std::string CHTLJSGenerator::generateDelegateExpression(std::shared_ptr<ASTNode> node) {
    std::string result = "Delegate({";
    
    if (!minify) {
        result += "\n";
        addIndent();
    }
    
    auto keys = node->getPropertyKeys();
    for (size_t i = 0; i < keys.size(); ++i) {
        if (i > 0) {
            result += ",";
            if (!minify) result += "\n";
        }
        
        if (!minify) indent();
        auto value = node->getProperty(keys[i]);
        result += keys[i] + ": ";
        if (value) {
            result += generateExpression(value);
        }
    }
    
    if (!minify) {
        removeIndent();
        result += "\n";
        indent();
    }
    
    result += "})";
    return result;
}

std::string CHTLJSGenerator::generateSelectorExpression(std::shared_ptr<ASTNode> node) {
    auto selector = std::dynamic_pointer_cast<SelectorExpressionNode>(node);
    if (!selector) return "";
    
    return "{{" + selector->selector + "}}";
}

std::string CHTLJSGenerator::generateReactiveValueExpression(std::shared_ptr<ASTNode> node) {
    auto reactive = std::dynamic_pointer_cast<ReactiveValueExpressionNode>(node);
    if (!reactive) return "";
    
    return "$" + reactive->variableName + "$";
}

std::string CHTLJSGenerator::generateProperty(std::shared_ptr<ASTNode> node) {
    if (!node || node->type != ASTNodeType::PROPERTY) return "";
    
    auto key = node->getProperty("key");
    auto value = node->getProperty("value");
    
    std::string result = generateExpression(key) + ": ";
    if (value) {
        result += generateExpression(value);
    }
    
    return result;
}

std::string CHTLJSGenerator::generateParameterList(const std::vector<std::string>& params) {
    std::string result;
    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0) result += ", ";
        result += params[i];
    }
    return result;
}

std::string CHTLJSGenerator::generateArgumentList(const std::vector<std::shared_ptr<ASTNode>>& args) {
    std::string result;
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) result += ", ";
        result += generateExpression(args[i]);
    }
    return result;
}

std::string CHTLJSGenerator::getOperatorString(TokenType type) const {
    switch (type) {
        case TokenType::PLUS: return "+";
        case TokenType::MINUS: return "-";
        case TokenType::MULTIPLY: return "*";
        case TokenType::DIVIDE: return "/";
        case TokenType::MODULO: return "%";
        case TokenType::POWER: return "**";
        case TokenType::EQUAL: return "==";
        case TokenType::NOT_EQUAL: return "!=";
        case TokenType::LESS: return "<";
        case TokenType::GREATER: return ">";
        case TokenType::LESS_EQUAL: return "<=";
        case TokenType::GREATER_EQUAL: return ">=";
        case TokenType::AND: return "&&";
        case TokenType::OR: return "||";
        case TokenType::ASSIGN: return "=";
        case TokenType::ARROW: return "->";
        default: return "?";
    }
}

bool CHTLJSGenerator::needsParentheses(std::shared_ptr<ASTNode> node, TokenType parentOp) const {
    // 简化实现，可以根据需要扩展
    return false;
}

int CHTLJSGenerator::getOperatorPrecedence(TokenType type) const {
    switch (type) {
        case TokenType::OR: return 1;
        case TokenType::AND: return 2;
        case TokenType::EQUAL:
        case TokenType::NOT_EQUAL: return 3;
        case TokenType::LESS:
        case TokenType::GREATER:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER_EQUAL: return 4;
        case TokenType::PLUS:
        case TokenType::MINUS: return 5;
        case TokenType::MULTIPLY:
        case TokenType::DIVIDE:
        case TokenType::MODULO: return 6;
        case TokenType::POWER: return 7;
        default: return 0;
    }
}

void CHTLJSGenerator::setMinify(bool minifyOutput) {
    minify = minifyOutput;
}

bool CHTLJSGenerator::isMinify() const {
    return minify;
}

void CHTLJSGenerator::reset() {
    output.clear();
    indentLevel = 0;
}

std::string CHTLJSGenerator::getOutput() const {
    return output;
}

} // namespace CHTLJS