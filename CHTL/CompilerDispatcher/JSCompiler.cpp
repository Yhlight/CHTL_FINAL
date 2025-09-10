#include "JSCompiler.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>
#include <fstream>

namespace CHTL {

// JSCompiler 实现
JSCompiler::JSCompiler() : debugMode(false), strictMode(false), minifyOutput(false), beautifyOutput(false), targetVersion("ES6") {
    initialize();
}

JSCompiler::~JSCompiler() {
    cleanup();
}

// 基本编译功能实现
JSCompileResult JSCompiler::compile(const std::string& jsCode) {
    JSCompileResult result;
    result.success = false;
    
    if (!validateInput(jsCode)) {
        result.errors.push_back("Invalid JavaScript input");
        return result;
    }
    
    try {
        // 解析AST
        auto ast = parseAST(jsCode);
        
        // 生成JavaScript
        result.javascript = generateJS(ast);
        
        // 应用优化
        if (minifyOutput) {
            result.javascript = minifyJS(result.javascript);
        } else if (beautifyOutput) {
            result.javascript = beautifyJS(result.javascript);
        }
        
        result.success = true;
        result.errors = errors;
        result.warnings = warnings;
        
    } catch (const std::exception& e) {
        result.errors.push_back("JavaScript compilation error: " + std::string(e.what()));
        result.success = false;
    }
    
    return result;
}

JSCompileResult JSCompiler::compileFile(const std::string& filePath) {
    JSCompileResult result;
    result.success = false;
    
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            result.errors.push_back("Cannot open file: " + filePath);
            return result;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string jsCode = buffer.str();
        file.close();
        
        return compile(jsCode);
        
    } catch (const std::exception& e) {
        result.errors.push_back("File reading error: " + std::string(e.what()));
        return result;
    }
}

// AST解析实现
std::shared_ptr<JSNode> JSCompiler::parseAST(const std::string& jsCode) {
    return parseASTInternal(jsCode);
}

std::shared_ptr<JSNode> JSCompiler::parseStatement(const std::string& statement) {
    return parseStatementInternal(statement);
}

std::shared_ptr<JSNode> JSCompiler::parseExpression(const std::string& expression) {
    return parseExpressionInternal(expression);
}

// JavaScript生成实现
std::string JSCompiler::generateJS(const std::shared_ptr<JSNode>& ast) {
    return generateJSInternal(ast);
}

std::string JSCompiler::generateStatement(const std::shared_ptr<JSNode>& node) {
    return generateStatementInternal(node);
}

std::string JSCompiler::generateExpression(const std::shared_ptr<JSNode>& node) {
    return generateExpressionInternal(node);
}

// JavaScript优化实现
std::string JSCompiler::optimizeJS(const std::string& js) {
    return optimizeJSInternal(js);
}

std::string JSCompiler::minifyJS(const std::string& js) {
    return minifyJSInternal(js);
}

std::string JSCompiler::beautifyJS(const std::string& js) {
    return beautifyJSInternal(js);
}

// JavaScript验证实现
bool JSCompiler::validateJS(const std::string& js) {
    return validateJSInternal(js);
}

bool JSCompiler::validateAST(const std::shared_ptr<JSNode>& ast) {
    return validateASTInternal(ast);
}

bool JSCompiler::validateStatement(const std::string& statement) {
    return validateStatementInternal(statement);
}

bool JSCompiler::validateExpression(const std::string& expression) {
    return validateExpressionInternal(expression);
}

// JavaScript转换实现
std::string JSCompiler::convertToES5(const std::string& js) {
    return convertToES5Internal(js);
}

std::string JSCompiler::convertToES6(const std::string& js) {
    return convertToES6Internal(js);
}

std::string JSCompiler::convertToTypeScript(const std::string& js) {
    return convertToTypeScriptInternal(js);
}

std::string JSCompiler::convertFromTypeScript(const std::string& ts) {
    return convertFromTypeScriptInternal(ts);
}

// JavaScript合并实现
std::string JSCompiler::mergeJS(const std::vector<std::string>& jsFiles) {
    return mergeJSInternal(jsFiles);
}

std::string JSCompiler::mergeAST(const std::vector<std::shared_ptr<JSNode>>& asts) {
    return mergeASTInternal(asts);
}

// JavaScript分割实现
std::vector<std::string> JSCompiler::splitJS(const std::string& js) {
    return splitJSInternal(js);
}

std::vector<std::shared_ptr<JSNode>> JSCompiler::splitAST(const std::shared_ptr<JSNode>& ast) {
    return splitASTInternal(ast);
}

// JavaScript过滤实现
std::string JSCompiler::filterJS(const std::string& js, const std::string& pattern) {
    return filterJSInternal(js, pattern);
}

std::vector<std::shared_ptr<JSNode>> JSCompiler::filterAST(const std::shared_ptr<JSNode>& ast, JSNodeType type) {
    return filterASTInternal(ast, type);
}

// JavaScript排序实现
std::string JSCompiler::sortJS(const std::string& js) {
    return sortJSInternal(js);
}

std::vector<std::shared_ptr<JSNode>> JSCompiler::sortAST(const std::vector<std::shared_ptr<JSNode>>& nodes) {
    return sortASTInternal(nodes);
}

// JavaScript去重实现
std::string JSCompiler::uniqueJS(const std::string& js) {
    return uniqueJSInternal(js);
}

std::vector<std::shared_ptr<JSNode>> JSCompiler::uniqueAST(const std::vector<std::shared_ptr<JSNode>>& nodes) {
    return uniqueASTInternal(nodes);
}

// JavaScript统计实现
std::map<std::string, int> JSCompiler::getStatistics(const std::string& js) {
    return getStatisticsInternal(js);
}

std::map<std::string, int> JSCompiler::getASTStatistics(const std::shared_ptr<JSNode>& ast) {
    return getASTStatisticsInternal(ast);
}

// JavaScript调试实现
std::string JSCompiler::debugJS(const std::string& js) {
    return debugJSInternal(js);
}

std::string JSCompiler::debugAST(const std::shared_ptr<JSNode>& ast) {
    return debugASTInternal(ast);
}

// 错误处理实现
void JSCompiler::clearErrors() {
    errors.clear();
}

void JSCompiler::clearWarnings() {
    warnings.clear();
}

void JSCompiler::clearMessages() {
    clearErrors();
    clearWarnings();
}

// 工具函数实现
std::string JSCompiler::nodeTypeToString(JSNodeType type) {
    return nodeTypeToStringInternal(type);
}

JSNodeType JSCompiler::stringToNodeType(const std::string& type) {
    return stringToNodeTypeInternal(type);
}

bool JSCompiler::isValidNodeType(JSNodeType type) {
    return isValidNodeTypeInternal(type);
}

bool JSCompiler::isValidJS(const std::string& js) {
    return isValidJSInternal(js);
}

// 序列化实现
std::string JSCompiler::toJSON() const {
    return toJSONInternal();
}

std::string JSCompiler::toXML() const {
    return toXMLInternal();
}

std::string JSCompiler::toYAML() const {
    return toYAMLInternal();
}

std::string JSCompiler::toString() const {
    return toStringInternal();
}

std::string JSCompiler::toDebugString() const {
    return toDebugStringInternal();
}

// 反序列化实现
bool JSCompiler::fromJSON(const std::string& json) {
    return fromJSONInternal(json);
}

bool JSCompiler::fromXML(const std::string& xml) {
    return fromXMLInternal(xml);
}

bool JSCompiler::fromYAML(const std::string& yaml) {
    return fromYAMLInternal(yaml);
}

bool JSCompiler::fromString(const std::string& str) {
    return fromStringInternal(str);
}

// 克隆实现
std::unique_ptr<JSCompiler> JSCompiler::clone() const {
    return cloneInternal();
}

// 比较实现
bool JSCompiler::equals(const JSCompiler& other) const {
    return equalsInternal(other);
}

bool JSCompiler::equals(const std::string& js) const {
    return equalsInternal(js);
}

// 格式化实现
std::string JSCompiler::format() const {
    return formatInternal();
}

std::string JSCompiler::minify() const {
    return minifyInternal();
}

std::string JSCompiler::beautify() const {
    return beautifyInternal();
}

// 内部方法实现
void JSCompiler::initialize() {
    clearMessages();
}

void JSCompiler::cleanup() {
    clearMessages();
}

bool JSCompiler::validateInput(const std::string& js) const {
    return !js.empty();
}

// AST解析内部方法实现
std::shared_ptr<JSNode> JSCompiler::parseASTInternal(const std::string& jsCode) {
    auto root = std::make_shared<JSNode>(JSNodeType::PROGRAM);
    
    // 简化实现：基本的JavaScript解析
    std::vector<std::string> lines;
    std::stringstream ss(jsCode);
    std::string line;
    
    while (std::getline(ss, line)) {
        if (!line.empty()) {
            auto statement = parseStatementInternal(line);
            if (statement) {
                root->children.push_back(statement);
            }
        }
    }
    
    return root;
}

std::shared_ptr<JSNode> JSCompiler::parseStatementInternal(const std::string& statement) {
    auto node = std::make_shared<JSNode>(JSNodeType::STATEMENT);
    node->value = statement;
    
    // 简化实现：根据关键字判断语句类型
    if (statement.find("function") != std::string::npos) {
        node->type = JSNodeType::FUNCTION;
    } else if (statement.find("var ") != std::string::npos || 
               statement.find("let ") != std::string::npos || 
               statement.find("const ") != std::string::npos) {
        node->type = JSNodeType::DECLARATION;
    } else if (statement.find("if ") != std::string::npos) {
        node->type = JSNodeType::IF_STATEMENT;
    } else if (statement.find("while ") != std::string::npos) {
        node->type = JSNodeType::WHILE_STATEMENT;
    } else if (statement.find("for ") != std::string::npos) {
        node->type = JSNodeType::FOR_STATEMENT;
    } else if (statement.find("return ") != std::string::npos) {
        node->type = JSNodeType::RETURN_STATEMENT;
    } else {
        node->type = JSNodeType::EXPRESSION_STATEMENT;
    }
    
    return node;
}

std::shared_ptr<JSNode> JSCompiler::parseExpressionInternal(const std::string& expression) {
    auto node = std::make_shared<JSNode>(JSNodeType::EXPRESSION);
    node->value = expression;
    
    // 简化实现：根据操作符判断表达式类型
    if (expression.find("+") != std::string::npos || 
        expression.find("-") != std::string::npos || 
        expression.find("*") != std::string::npos || 
        expression.find("/") != std::string::npos) {
        node->type = JSNodeType::BINARY_OPERATION;
    } else if (expression.find("(") != std::string::npos && expression.find(")") != std::string::npos) {
        node->type = JSNodeType::CALL_EXPRESSION;
    } else if (expression.find("[") != std::string::npos && expression.find("]") != std::string::npos) {
        node->type = JSNodeType::ARRAY_EXPRESSION;
    } else if (expression.find("{") != std::string::npos && expression.find("}") != std::string::npos) {
        node->type = JSNodeType::OBJECT_EXPRESSION;
    } else if (expression.find("?") != std::string::npos && expression.find(":") != std::string::npos) {
        node->type = JSNodeType::CONDITIONAL_EXPRESSION;
    } else if (expression.find("=") != std::string::npos) {
        node->type = JSNodeType::ASSIGNMENT_EXPRESSION;
    } else {
        node->type = JSNodeType::LITERAL;
    }
    
    return node;
}

// JavaScript生成内部方法实现
std::string JSCompiler::generateJSInternal(const std::shared_ptr<JSNode>& ast) {
    if (!ast) return "";
    
    std::stringstream ss;
    
    for (const auto& child : ast->children) {
        ss << generateStatementInternal(child) << "\n";
    }
    
    return ss.str();
}

std::string JSCompiler::generateStatementInternal(const std::shared_ptr<JSNode>& node) {
    if (!node) return "";
    
    switch (node->type) {
        case JSNodeType::FUNCTION:
            return "function " + node->value;
        case JSNodeType::DECLARATION:
            return node->value;
        case JSNodeType::IF_STATEMENT:
            return "if " + node->value;
        case JSNodeType::WHILE_STATEMENT:
            return "while " + node->value;
        case JSNodeType::FOR_STATEMENT:
            return "for " + node->value;
        case JSNodeType::RETURN_STATEMENT:
            return "return " + node->value;
        case JSNodeType::EXPRESSION_STATEMENT:
            return node->value;
        default:
            return node->value;
    }
}

std::string JSCompiler::generateExpressionInternal(const std::shared_ptr<JSNode>& node) {
    if (!node) return "";
    
    return node->value;
}

// JavaScript优化内部方法实现
std::string JSCompiler::optimizeJSInternal(const std::string& js) {
    std::string result = js;
    
    // 移除多余空格和换行
    result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
    
    // 移除注释
    result = std::regex_replace(result, std::regex(R"(//.*)"), "");
    result = std::regex_replace(result, std::regex(R"(/\*.*?\*/)"), "");
    
    // 移除不必要的分号
    result = std::regex_replace(result, std::regex(R"(\s*;\s*}"), "}");
    
    // 移除console.log语句（可选）
    if (removeConsoleLogs) {
        result = std::regex_replace(result, std::regex(R"(console\.log\([^)]*\);\s*)"), "");
    }
    
    // 移除未使用的变量声明
    result = removeUnusedVariables(result);
    
    // 合并连续的字符串
    result = mergeStringLiterals(result);
    
    return result;
}

// 移除未使用变量的辅助方法
std::string JSCompiler::removeUnusedVariables(const std::string& js) {
    // 简化实现：移除明显的未使用变量
    std::string result = js;
    
    // 匹配var/let/const声明
    std::regex varRegex(R"((var|let|const)\s+(\w+)\s*=\s*[^;]+;)");
    std::sregex_iterator iter(result.begin(), result.end(), varRegex);
    std::sregex_iterator end;
    
    std::vector<std::string> declarations;
    for (; iter != end; ++iter) {
        declarations.push_back(iter->str(2)); // 变量名
    }
    
    // 检查变量是否被使用
    for (const auto& varName : declarations) {
        std::regex usageRegex(varName + R"(\s*[^=])");
        if (!std::regex_search(result, usageRegex)) {
            // 变量未被使用，移除声明
            std::regex removeRegex(R"((var|let|const)\s+)" + varName + R"(\s*=\s*[^;]+;)");
            result = std::regex_replace(result, removeRegex, "");
        }
    }
    
    return result;
}

// 合并字符串字面量的辅助方法
std::string JSCompiler::mergeStringLiterals(const std::string& js) {
    // 简化实现：合并相邻的字符串字面量
    std::string result = js;
    
    // 匹配相邻的字符串字面量
    std::regex stringRegex(R"("([^"]*)"\s*\+\s*"([^"]*)")");
    std::smatch match;
    
    while (std::regex_search(result, match, stringRegex)) {
        std::string merged = "\"" + match.str(1) + match.str(2) + "\"";
        result = std::regex_replace(result, stringRegex, merged);
    }
    
    return result;
}

std::string JSCompiler::minifyJSInternal(const std::string& js) {
    std::string result = js;
    
    // 移除所有空格和换行
    result = std::regex_replace(result, std::regex(R"(\s+)"), "");
    
    // 移除注释
    result = std::regex_replace(result, std::regex(R"(//.*)"), "");
    result = std::regex_replace(result, std::regex(R"(/\*.*?\*/)"), "");
    
    return result;
}

std::string JSCompiler::beautifyJSInternal(const std::string& js) {
    std::string result = js;
    
    // 基本美化：添加换行和缩进
    result = std::regex_replace(result, std::regex(R"(\{)"), " {\n  ");
    result = std::regex_replace(result, std::regex(R"(\})"), "\n}\n");
    result = std::regex_replace(result, std::regex(R"(;)"), ";\n");
    
    return result;
}

// JavaScript验证内部方法实现
bool JSCompiler::validateJSInternal(const std::string& js) {
    // 简化实现：检查基本语法
    return !js.empty() && js.find(';') != std::string::npos;
}

bool JSCompiler::validateASTInternal(const std::shared_ptr<JSNode>& ast) {
    return ast != nullptr && ast->type != JSNodeType::UNKNOWN;
}

bool JSCompiler::validateStatementInternal(const std::string& statement) {
    return !statement.empty();
}

bool JSCompiler::validateExpressionInternal(const std::string& expression) {
    return !expression.empty();
}

// 其他内部方法的简化实现
std::string JSCompiler::convertToES5Internal(const std::string& js) {
    return js; // 简化实现
}

std::string JSCompiler::convertToES6Internal(const std::string& js) {
    return js; // 简化实现
}

std::string JSCompiler::convertToTypeScriptInternal(const std::string& js) {
    return js; // 简化实现
}

std::string JSCompiler::convertFromTypeScriptInternal(const std::string& ts) {
    return ts; // 简化实现
}

std::string JSCompiler::mergeJSInternal(const std::vector<std::string>& jsFiles) {
    std::stringstream ss;
    for (const auto& js : jsFiles) {
        ss << js << "\n";
    }
    return ss.str();
}

std::string JSCompiler::mergeASTInternal(const std::vector<std::shared_ptr<JSNode>>& asts) {
    std::stringstream ss;
    for (const auto& ast : asts) {
        ss << generateJSInternal(ast) << "\n";
    }
    return ss.str();
}

std::vector<std::string> JSCompiler::splitJSInternal(const std::string& js) {
    std::vector<std::string> result;
    std::stringstream ss(js);
    std::string line;
    
    while (std::getline(ss, line)) {
        if (!line.empty()) {
            result.push_back(line);
        }
    }
    
    return result;
}

std::vector<std::shared_ptr<JSNode>> JSCompiler::splitASTInternal(const std::shared_ptr<JSNode>& ast) {
    std::vector<std::shared_ptr<JSNode>> result;
    if (ast) {
        result = ast->children;
    }
    return result;
}

std::string JSCompiler::filterJSInternal(const std::string& js, const std::string& pattern) {
    return js; // 简化实现
}

std::vector<std::shared_ptr<JSNode>> JSCompiler::filterASTInternal(const std::shared_ptr<JSNode>& ast, JSNodeType type) {
    std::vector<std::shared_ptr<JSNode>> result;
    if (ast) {
        for (const auto& child : ast->children) {
            if (child->type == type) {
                result.push_back(child);
            }
        }
    }
    return result;
}

std::string JSCompiler::sortJSInternal(const std::string& js) {
    return js; // 简化实现
}

std::vector<std::shared_ptr<JSNode>> JSCompiler::sortASTInternal(const std::vector<std::shared_ptr<JSNode>>& nodes) {
    return nodes; // 简化实现
}

std::string JSCompiler::uniqueJSInternal(const std::string& js) {
    return js; // 简化实现
}

std::vector<std::shared_ptr<JSNode>> JSCompiler::uniqueASTInternal(const std::vector<std::shared_ptr<JSNode>>& nodes) {
    return nodes; // 简化实现
}

std::map<std::string, int> JSCompiler::getStatisticsInternal(const std::string& js) {
    std::map<std::string, int> stats;
    stats["length"] = js.length();
    stats["lines"] = std::count(js.begin(), js.end(), '\n') + 1;
    stats["functions"] = std::count(js.begin(), js.end(), 'f') + std::count(js.begin(), js.end(), 'F');
    return stats;
}

std::map<std::string, int> JSCompiler::getASTStatisticsInternal(const std::shared_ptr<JSNode>& ast) {
    std::map<std::string, int> stats;
    if (ast) {
        stats["nodes"] = ast->children.size();
    }
    return stats;
}

std::string JSCompiler::debugJSInternal(const std::string& js) {
    return "Debug JS: " + js;
}

std::string JSCompiler::debugASTInternal(const std::shared_ptr<JSNode>& ast) {
    return "Debug AST: " + (ast ? nodeTypeToStringInternal(ast->type) : "null");
}

// 工具函数内部方法实现
std::string JSCompiler::nodeTypeToStringInternal(JSNodeType type) {
    switch (type) {
        case JSNodeType::PROGRAM: return "PROGRAM";
        case JSNodeType::STATEMENT: return "STATEMENT";
        case JSNodeType::EXPRESSION: return "EXPRESSION";
        case JSNodeType::DECLARATION: return "DECLARATION";
        case JSNodeType::FUNCTION: return "FUNCTION";
        case JSNodeType::VARIABLE: return "VARIABLE";
        case JSNodeType::LITERAL: return "LITERAL";
        case JSNodeType::IDENTIFIER: return "IDENTIFIER";
        case JSNodeType::BINARY_OPERATION: return "BINARY_OPERATION";
        case JSNodeType::UNARY_OPERATION: return "UNARY_OPERATION";
        case JSNodeType::CALL_EXPRESSION: return "CALL_EXPRESSION";
        case JSNodeType::MEMBER_EXPRESSION: return "MEMBER_EXPRESSION";
        case JSNodeType::ARRAY_EXPRESSION: return "ARRAY_EXPRESSION";
        case JSNodeType::OBJECT_EXPRESSION: return "OBJECT_EXPRESSION";
        case JSNodeType::CONDITIONAL_EXPRESSION: return "CONDITIONAL_EXPRESSION";
        case JSNodeType::ASSIGNMENT_EXPRESSION: return "ASSIGNMENT_EXPRESSION";
        case JSNodeType::SEQUENCE_EXPRESSION: return "SEQUENCE_EXPRESSION";
        case JSNodeType::BLOCK_STATEMENT: return "BLOCK_STATEMENT";
        case JSNodeType::EXPRESSION_STATEMENT: return "EXPRESSION_STATEMENT";
        case JSNodeType::IF_STATEMENT: return "IF_STATEMENT";
        case JSNodeType::WHILE_STATEMENT: return "WHILE_STATEMENT";
        case JSNodeType::FOR_STATEMENT: return "FOR_STATEMENT";
        case JSNodeType::RETURN_STATEMENT: return "RETURN_STATEMENT";
        case JSNodeType::BREAK_STATEMENT: return "BREAK_STATEMENT";
        case JSNodeType::CONTINUE_STATEMENT: return "CONTINUE_STATEMENT";
        case JSNodeType::TRY_STATEMENT: return "TRY_STATEMENT";
        case JSNodeType::CATCH_STATEMENT: return "CATCH_STATEMENT";
        case JSNodeType::THROW_STATEMENT: return "THROW_STATEMENT";
        default: return "UNKNOWN";
    }
}

JSNodeType JSCompiler::stringToNodeTypeInternal(const std::string& type) {
    if (type == "PROGRAM") return JSNodeType::PROGRAM;
    if (type == "STATEMENT") return JSNodeType::STATEMENT;
    if (type == "EXPRESSION") return JSNodeType::EXPRESSION;
    if (type == "DECLARATION") return JSNodeType::DECLARATION;
    if (type == "FUNCTION") return JSNodeType::FUNCTION;
    if (type == "VARIABLE") return JSNodeType::VARIABLE;
    if (type == "LITERAL") return JSNodeType::LITERAL;
    if (type == "IDENTIFIER") return JSNodeType::IDENTIFIER;
    if (type == "BINARY_OPERATION") return JSNodeType::BINARY_OPERATION;
    if (type == "UNARY_OPERATION") return JSNodeType::UNARY_OPERATION;
    if (type == "CALL_EXPRESSION") return JSNodeType::CALL_EXPRESSION;
    if (type == "MEMBER_EXPRESSION") return JSNodeType::MEMBER_EXPRESSION;
    if (type == "ARRAY_EXPRESSION") return JSNodeType::ARRAY_EXPRESSION;
    if (type == "OBJECT_EXPRESSION") return JSNodeType::OBJECT_EXPRESSION;
    if (type == "CONDITIONAL_EXPRESSION") return JSNodeType::CONDITIONAL_EXPRESSION;
    if (type == "ASSIGNMENT_EXPRESSION") return JSNodeType::ASSIGNMENT_EXPRESSION;
    if (type == "SEQUENCE_EXPRESSION") return JSNodeType::SEQUENCE_EXPRESSION;
    if (type == "BLOCK_STATEMENT") return JSNodeType::BLOCK_STATEMENT;
    if (type == "EXPRESSION_STATEMENT") return JSNodeType::EXPRESSION_STATEMENT;
    if (type == "IF_STATEMENT") return JSNodeType::IF_STATEMENT;
    if (type == "WHILE_STATEMENT") return JSNodeType::WHILE_STATEMENT;
    if (type == "FOR_STATEMENT") return JSNodeType::FOR_STATEMENT;
    if (type == "RETURN_STATEMENT") return JSNodeType::RETURN_STATEMENT;
    if (type == "BREAK_STATEMENT") return JSNodeType::BREAK_STATEMENT;
    if (type == "CONTINUE_STATEMENT") return JSNodeType::CONTINUE_STATEMENT;
    if (type == "TRY_STATEMENT") return JSNodeType::TRY_STATEMENT;
    if (type == "CATCH_STATEMENT") return JSNodeType::CATCH_STATEMENT;
    if (type == "THROW_STATEMENT") return JSNodeType::THROW_STATEMENT;
    return JSNodeType::UNKNOWN;
}

bool JSCompiler::isValidNodeTypeInternal(JSNodeType type) {
    return type != JSNodeType::UNKNOWN;
}

bool JSCompiler::isValidJSInternal(const std::string& js) {
    return !js.empty();
}

// 序列化内部方法实现
std::string JSCompiler::toJSONInternal() const {
    return "{\"type\":\"JSCompiler\",\"debugMode\":" + std::to_string(debugMode) + "}";
}

std::string JSCompiler::toXMLInternal() const {
    return "<JSCompiler><debugMode>" + std::to_string(debugMode) + "</debugMode></JSCompiler>";
}

std::string JSCompiler::toYAMLInternal() const {
    return "type: JSCompiler\ndebugMode: " + std::to_string(debugMode);
}

std::string JSCompiler::toStringInternal() const {
    return "JSCompiler";
}

std::string JSCompiler::toDebugStringInternal() const {
    return "JSCompiler(debug=" + std::to_string(debugMode) + ")";
}

// 反序列化内部方法实现
bool JSCompiler::fromJSONInternal(const std::string& json) {
    return true; // 简化实现
}

bool JSCompiler::fromXMLInternal(const std::string& xml) {
    return true; // 简化实现
}

bool JSCompiler::fromYAMLInternal(const std::string& yaml) {
    return true; // 简化实现
}

bool JSCompiler::fromStringInternal(const std::string& str) {
    return true; // 简化实现
}

// 克隆内部方法实现
std::unique_ptr<JSCompiler> JSCompiler::cloneInternal() const {
    auto compiler = std::make_unique<JSCompiler>();
    compiler->debugMode = debugMode;
    compiler->strictMode = strictMode;
    compiler->minifyOutput = minifyOutput;
    compiler->beautifyOutput = beautifyOutput;
    compiler->targetVersion = targetVersion;
    return compiler;
}

// 比较内部方法实现
bool JSCompiler::equalsInternal(const JSCompiler& other) const {
    return debugMode == other.debugMode && strictMode == other.strictMode;
}

bool JSCompiler::equalsInternal(const std::string& js) const {
    return !js.empty();
}

// 格式化内部方法实现
std::string JSCompiler::formatInternal() const {
    return toStringInternal();
}

std::string JSCompiler::minifyInternal() const {
    return toStringInternal();
}

std::string JSCompiler::beautifyInternal() const {
    return toStringInternal();
}

} // namespace CHTL