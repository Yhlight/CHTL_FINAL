#include "chtl_js/chtl_js_compiler.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace chtl {
namespace chtl_js {

// CHTL JS编译器实现
CHTLJSCompiler::CHTLJSCompiler(const CHTLJSCompileOptions& options) 
    : options_(options) {}

CHTLJSCompileResult CHTLJSCompiler::compile(const std::string& source) {
    reset();
    
    // 词法分析和解析
    auto lex_parse_result = lex_and_parse(source);
    if (!lex_parse_result.success) {
        return lex_parse_result;
    }
    
    // 代码生成
    auto generate_result = generate_js(ast_);
    if (!generate_result.success) {
        return generate_result;
    }
    
    return generate_result;
}

CHTLJSCompileResult CHTLJSCompiler::compile_file(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        CHTLJSCompileResult result;
        result.success = false;
        result.errors.push_back("Cannot open file: " + file_path);
        return result;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return compile(buffer.str());
}

void CHTLJSCompiler::set_options(const CHTLJSCompileOptions& options) {
    options_ = options;
}

CHTLJSCompileOptions CHTLJSCompiler::get_options() const {
    return options_;
}

void CHTLJSCompiler::reset() {
    errors_.clear();
    warnings_.clear();
    selectors_.clear();
    dependencies_.clear();
    ast_.reset();
}

CHTLJSCompileResult CHTLJSCompiler::lex_and_parse(const std::string& source) {
    CHTLJSCompileResult result;
    
    try {
        lexer_ = std::make_unique<CHTLJSLexer>(source);
        parser_ = std::make_unique<CHTLJSParser>(source);
        
        ast_ = parser_->parse();
        
        if (ast_) {
            result.success = true;
        } else {
            result.success = false;
            result.errors.push_back("Failed to parse CHTL JS code");
        }
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back("Parse error: " + std::string(e.what()));
    }
    
    return result;
}

CHTLJSCompileResult CHTLJSCompiler::generate_js(const std::shared_ptr<ProgramNode>& ast) {
    CHTLJSCompileResult result;
    
    if (!ast) {
        result.success = false;
        result.errors.push_back("No AST to generate code from");
        return result;
    }
    
    try {
        // 提取选择器和依赖
        selectors_ = extract_selectors(ast);
        dependencies_ = extract_dependencies(ast);
        
        // 生成JavaScript代码
        std::string js_code = generate_program(ast);
        
        // 添加选择器自动添加代码
        if (options_.enable_selector_auto_add && !selectors_.empty()) {
            js_code = generate_auto_selector_code() + "\n" + js_code;
        }
        
        // 添加依赖加载代码
        if (!dependencies_.empty()) {
            js_code = generate_dependency_loader(dependencies_) + "\n" + js_code;
        }
        
        // 代码优化
        if (options_.minify_output) {
            js_code = minify_js(js_code);
        } else {
            js_code = optimize_js(js_code);
        }
        
        result.js_code = js_code;
        result.selectors = selectors_;
        result.dependencies = dependencies_;
        result.success = true;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back("Code generation error: " + std::string(e.what()));
    }
    
    return result;
}

std::string CHTLJSCompiler::generate_program(const std::shared_ptr<ProgramNode>& node) {
    std::string result;
    
    for (const auto& statement : node->body) {
        result += generate_statement(statement) + ";\n";
    }
    
    return result;
}

std::string CHTLJSCompiler::generate_statement(const std::shared_ptr<CHTLJSASTNode>& node) {
    if (!node) return "";
    
    switch (node->type) {
        case CHTLJSASTNodeType::VARIABLE_DECLARATION:
            return generate_variable_declaration(std::static_pointer_cast<VariableDeclarationNode>(node));
        case CHTLJSASTNodeType::FUNCTION_DECLARATION:
            return generate_function_declaration(std::static_pointer_cast<FunctionDeclarationNode>(node));
        case CHTLJSASTNodeType::BLOCK_STATEMENT:
            return generate_block_statement(std::static_pointer_cast<BlockStatementNode>(node));
        case CHTLJSASTNodeType::FILELOADER_EXPRESSION:
            return generate_fileloader(std::static_pointer_cast<FileloaderNode>(node));
        case CHTLJSASTNodeType::LISTEN_EXPRESSION:
            return generate_listen(std::static_pointer_cast<ListenNode>(node));
        case CHTLJSASTNodeType::ANIMATE_EXPRESSION:
            return generate_animate(std::static_pointer_cast<AnimateNode>(node));
        case CHTLJSASTNodeType::VIR_DECLARATION:
            return generate_vir(std::static_pointer_cast<VirNode>(node));
        case CHTLJSASTNodeType::ROUTER_EXPRESSION:
            return generate_router(std::static_pointer_cast<RouterNode>(node));
        case CHTLJSASTNodeType::INEVERAWAY_EXPRESSION:
            return generate_ineveraway(std::static_pointer_cast<INeverAwayNode>(node));
        case CHTLJSASTNodeType::UTIL_EXPRESSION:
            return generate_util(std::static_pointer_cast<UtilNode>(node));
        default:
            return generate_expression(node);
    }
}

std::string CHTLJSCompiler::generate_expression(const std::shared_ptr<CHTLJSASTNode>& node) {
    if (!node) return "";
    
    switch (node->type) {
        case CHTLJSASTNodeType::IDENTIFIER:
            return generate_identifier(std::static_pointer_cast<IdentifierNode>(node));
        case CHTLJSASTNodeType::LITERAL:
            return generate_literal(std::static_pointer_cast<LiteralNode>(node));
        case CHTLJSASTNodeType::BINARY_EXPRESSION:
            return generate_binary_expression(std::static_pointer_cast<BinaryExpressionNode>(node));
        case CHTLJSASTNodeType::CALL_EXPRESSION:
            return generate_call_expression(std::static_pointer_cast<CallExpressionNode>(node));
        case CHTLJSASTNodeType::MEMBER_EXPRESSION:
            return generate_member_expression(std::static_pointer_cast<MemberExpressionNode>(node));
        case CHTLJSASTNodeType::SELECTOR_EXPRESSION:
            return generate_selector(std::static_pointer_cast<SelectorNode>(node));
        default:
            return "/* Unknown expression type */";
    }
}

std::string CHTLJSCompiler::generate_identifier(const std::shared_ptr<IdentifierNode>& node) {
    return node->name;
}

std::string CHTLJSCompiler::generate_literal(const std::shared_ptr<LiteralNode>& node) {
    switch (node->literal_type) {
        case CHTLJSTokenType::STRING_LITERAL:
            return node->value;
        case CHTLJSTokenType::NUMBER_LITERAL:
            return node->value;
        case CHTLJSTokenType::BOOLEAN_LITERAL:
            return node->value;
        default:
            return node->value;
    }
}

std::string CHTLJSCompiler::generate_binary_expression(const std::shared_ptr<BinaryExpressionNode>& node) {
    std::string left = generate_expression(node->left);
    std::string op = generate_operator(node->operator_type);
    std::string right = generate_expression(node->right);
    
    return "(" + left + " " + op + " " + right + ")";
}

std::string CHTLJSCompiler::generate_call_expression(const std::shared_ptr<CallExpressionNode>& node) {
    std::string callee = generate_expression(node->callee);
    std::string args;
    
    for (size_t i = 0; i < node->arguments.size(); ++i) {
        if (i > 0) args += ", ";
        args += generate_expression(node->arguments[i]);
    }
    
    return callee + "(" + args + ")";
}

std::string CHTLJSCompiler::generate_member_expression(const std::shared_ptr<MemberExpressionNode>& node) {
    std::string object = generate_expression(node->object);
    std::string property = generate_expression(node->property);
    
    if (node->computed) {
        return object + "[" + property + "]";
    } else {
        return object + "." + property;
    }
}

std::string CHTLJSCompiler::generate_variable_declaration(const std::shared_ptr<VariableDeclarationNode>& node) {
    std::string kind;
    switch (node->kind) {
        case CHTLJSTokenType::VAR: kind = "var"; break;
        case CHTLJSTokenType::LET: kind = "let"; break;
        case CHTLJSTokenType::CONST: kind = "const"; break;
        default: kind = "var"; break;
    }
    
    std::string result = kind + " ";
    
    for (size_t i = 0; i < node->declarations.size(); ++i) {
        if (i > 0) result += ", ";
        result += generate_expression(node->declarations[i]);
    }
    
    return result;
}

std::string CHTLJSCompiler::generate_function_declaration(const std::shared_ptr<FunctionDeclarationNode>& node) {
    std::string result = "function ";
    
    if (node->async) {
        result = "async " + result;
    }
    
    result += generate_expression(node->id) + "(";
    
    for (size_t i = 0; i < node->params.size(); ++i) {
        if (i > 0) result += ", ";
        result += generate_expression(node->params[i]);
    }
    
    result += ") " + generate_statement(node->body);
    
    return result;
}

std::string CHTLJSCompiler::generate_block_statement(const std::shared_ptr<BlockStatementNode>& node) {
    std::string result = "{\n";
    
    for (const auto& statement : node->body) {
        result += "  " + generate_statement(statement) + ";\n";
    }
    
    result += "}";
    return result;
}

// CHTL JS特有语法生成
std::string CHTLJSCompiler::generate_fileloader(const std::shared_ptr<FileloaderNode>& node) {
    std::string result = "// CHTL JS Fileloader\n";
    result += "Promise.all([\n";
    
    for (size_t i = 0; i < node->load_files.size(); ++i) {
        if (i > 0) result += ",\n";
        result += "  import('" + node->load_files[i] + "')";
    }
    
    result += "\n]).then(() => {\n";
    result += "  console.log('All files loaded');\n";
    result += "});";
    
    return result;
}

std::string CHTLJSCompiler::generate_listen(const std::shared_ptr<ListenNode>& node) {
    std::string result = "// CHTL JS Listener\n";
    result += "document.addEventListener('" + node->event + "', ";
    
    if (node->selector) {
        result += "function(event) {\n";
        result += "  if (event.target.matches(" + generate_selector_js(generate_expression(node->selector)) + ")) {\n";
        result += "    " + generate_expression(node->handler) + ";\n";
        result += "  }\n";
        result += "}";
    } else {
        result += generate_expression(node->handler);
    }
    
    result += ");";
    return result;
}

std::string CHTLJSCompiler::generate_animate(const std::shared_ptr<AnimateNode>& node) {
    std::string result = "// CHTL JS Animation\n";
    result += "const animation = {\n";
    result += "  target: " + generate_expression(node->target) + ",\n";
    result += "  duration: " + std::to_string(node->duration) + ",\n";
    result += "  easing: '" + node->easing + "',\n";
    result += "  loop: " + std::to_string(node->loop) + ",\n";
    result += "  delay: " + std::to_string(node->delay) + "\n";
    result += "};\n";
    result += "// Animation implementation would go here";
    
    return result;
}

std::string CHTLJSCompiler::generate_vir(const std::shared_ptr<VirNode>& node) {
    std::string result = "// CHTL JS Virtual Object\n";
    result += "const " + node->name + " = " + generate_expression(node->value) + ";";
    return result;
}

std::string CHTLJSCompiler::generate_router(const std::shared_ptr<RouterNode>& node) {
    std::string result = "// CHTL JS Router\n";
    result += "const router = {\n";
    result += "  path: '" + node->path + "',\n";
    result += "  handler: " + generate_expression(node->handler) + "\n";
    result += "};";
    return result;
}

std::string CHTLJSCompiler::generate_ineveraway(const std::shared_ptr<INeverAwayNode>& node) {
    std::string result = "// CHTL JS iNeverAway\n";
    result += "const iNeverAway = {\n";
    
    for (const auto& pair : node->functions) {
        result += "  " + pair.first + ": " + generate_expression(pair.second) + ",\n";
    }
    
    result += "};";
    return result;
}

std::string CHTLJSCompiler::generate_util(const std::shared_ptr<UtilNode>& node) {
    std::string result = "// CHTL JS Util Expression\n";
    result += "const util = {\n";
    result += "  condition: " + generate_expression(node->condition) + ",\n";
    
    if (node->change) {
        result += "  change: " + generate_expression(node->change) + ",\n";
    }
    
    if (node->then) {
        result += "  then: " + generate_expression(node->then) + "\n";
    }
    
    result += "};";
    return result;
}

std::string CHTLJSCompiler::generate_selector(const std::shared_ptr<SelectorNode>& node) {
    if (node->is_self_ref) {
        return generate_self_reference();
    }
    
    return generate_selector_js(node->selector);
}

// 辅助方法
std::string CHTLJSCompiler::generate_operator(CHTLJSTokenType type) {
    switch (type) {
        case CHTLJSTokenType::PLUS: return "+";
        case CHTLJSTokenType::MINUS: return "-";
        case CHTLJSTokenType::MULTIPLY: return "*";
        case CHTLJSTokenType::DIVIDE: return "/";
        case CHTLJSTokenType::MODULO: return "%";
        case CHTLJSTokenType::POWER: return "**";
        case CHTLJSTokenType::ASSIGN: return "=";
        case CHTLJSTokenType::EQUAL: return "==";
        case CHTLJSTokenType::NOT_EQUAL: return "!=";
        case CHTLJSTokenType::LESS: return "<";
        case CHTLJSTokenType::LESS_EQUAL: return "<=";
        case CHTLJSTokenType::GREATER: return ">";
        case CHTLJSTokenType::GREATER_EQUAL: return ">=";
        case CHTLJSTokenType::AND: return "&&";
        case CHTLJSTokenType::OR: return "||";
        case CHTLJSTokenType::NOT: return "!";
        case CHTLJSTokenType::COLON: return ":";
        case CHTLJSTokenType::QUESTION: return "?";
        default: return "/* unknown operator */";
    }
}

std::string CHTLJSCompiler::generate_selector_js(const std::string& selector) {
    // 移除 {{ 和 }}
    std::string clean_selector = selector;
    if (clean_selector.length() >= 4 && 
        clean_selector.substr(0, 2) == "{{" && 
        clean_selector.substr(clean_selector.length() - 2) == "}}") {
        clean_selector = clean_selector.substr(2, clean_selector.length() - 4);
    }
    
    // 生成选择器函数
    return "document.querySelector('" + clean_selector + "')";
}

std::string CHTLJSCompiler::generate_self_reference() {
    return "this";
}

std::string CHTLJSCompiler::generate_arrow_syntax(const std::string& expression) {
    if (options_.enable_arrow_syntax) {
        return "() => " + expression;
    }
    return "function() { return " + expression + "; }";
}

// 选择器处理
std::vector<std::string> CHTLJSCompiler::extract_selectors(const std::shared_ptr<ProgramNode>& ast) {
    std::vector<std::string> selectors;
    
    // 这里应该遍历AST提取所有选择器
    // 简化实现
    for (const auto& statement : ast->body) {
        if (statement->type == CHTLJSASTNodeType::SELECTOR_EXPRESSION) {
            auto selector_node = std::static_pointer_cast<SelectorNode>(statement);
            selectors.push_back(selector_node->selector);
        }
    }
    
    return selectors;
}

std::string CHTLJSCompiler::generate_selector_function(const std::string& selector) {
    return "function() { return document.querySelector('" + selector + "'); }";
}

std::string CHTLJSCompiler::generate_auto_selector_code() {
    std::string result = "// CHTL JS Auto Selector Code\n";
    result += "const CHTLSelectors = {\n";
    
    for (size_t i = 0; i < selectors_.size(); ++i) {
        if (i > 0) result += ",\n";
        result += "  selector" + std::to_string(i) + ": " + generate_selector_function(selectors_[i]);
    }
    
    result += "\n};";
    return result;
}

// 依赖处理
std::vector<std::string> CHTLJSCompiler::extract_dependencies(const std::shared_ptr<ProgramNode>& ast) {
    std::vector<std::string> dependencies;
    
    // 这里应该遍历AST提取所有依赖
    // 简化实现
    for (const auto& statement : ast->body) {
        if (statement->type == CHTLJSASTNodeType::FILELOADER_EXPRESSION) {
            auto fileloader_node = std::static_pointer_cast<FileloaderNode>(statement);
            dependencies.insert(dependencies.end(), 
                fileloader_node->load_files.begin(), 
                fileloader_node->load_files.end());
        }
    }
    
    return dependencies;
}

std::string CHTLJSCompiler::generate_dependency_loader(const std::vector<std::string>& dependencies) {
    std::string result = "// CHTL JS Dependency Loader\n";
    result += "const dependencies = [\n";
    
    for (size_t i = 0; i < dependencies.size(); ++i) {
        if (i > 0) result += ",\n";
        result += "  '" + dependencies[i] + "'";
    }
    
    result += "\n];\n";
    result += "// Load dependencies here";
    
    return result;
}

// 错误处理
void CHTLJSCompiler::add_error(const std::string& error) {
    errors_.push_back(error);
}

void CHTLJSCompiler::add_warning(const std::string& warning) {
    warnings_.push_back(warning);
}

// 代码优化
std::string CHTLJSCompiler::minify_js(const std::string& js_code) {
    // 简单的代码压缩实现
    std::string result = js_code;
    
    // 移除注释
    size_t pos = 0;
    while ((pos = result.find("//", pos)) != std::string::npos) {
        size_t end = result.find('\n', pos);
        if (end != std::string::npos) {
            result.erase(pos, end - pos);
        } else {
            result.erase(pos);
        }
    }
    
    // 移除多余空白
    result.erase(std::remove_if(result.begin(), result.end(), 
        [](char c) { return c == ' ' || c == '\t' || c == '\r'; }), result.end());
    
    return result;
}

std::string CHTLJSCompiler::optimize_js(const std::string& js_code) {
    // 简单的代码优化实现
    return js_code;
}

std::string CHTLJSCompiler::generate_debug_info(const std::shared_ptr<ProgramNode>& ast) {
    std::string result = "// CHTL JS Debug Info\n";
    result += "const debugInfo = {\n";
    result += "  selectors: " + std::to_string(selectors_.size()) + ",\n";
    result += "  dependencies: " + std::to_string(dependencies_.size()) + ",\n";
    result += "  statements: " + std::to_string(ast->body.size()) + "\n";
    result += "};";
    return result;
}

// CHTL JS模块管理器实现
CHTLJSModuleManager::CHTLJSModuleManager() 
    : compiler_(std::make_unique<CHTLJSCompiler>()) {}

void CHTLJSModuleManager::add_module(const std::string& name, const std::string& source) {
    auto module = std::make_shared<CHTLJSModule>(name, source);
    modules_[name] = module;
}

void CHTLJSModuleManager::remove_module(const std::string& name) {
    modules_.erase(name);
}

std::shared_ptr<CHTLJSModule> CHTLJSModuleManager::get_module(const std::string& name) {
    auto it = modules_.find(name);
    if (it != modules_.end()) {
        return it->second;
    }
    return nullptr;
}

CHTLJSCompileResult CHTLJSModuleManager::compile_module(const std::string& name) {
    auto module = get_module(name);
    if (!module) {
        CHTLJSCompileResult result;
        result.success = false;
        result.errors.push_back("Module not found: " + name);
        return result;
    }
    
    auto result = compiler_->compile(module->source);
    module->compile_result = result;
    return result;
}

CHTLJSCompileResult CHTLJSModuleManager::compile_all_modules() {
    CHTLJSCompileResult result;
    result.success = true;
    
    for (const auto& pair : modules_) {
        auto module_result = compile_module(pair.first);
        if (!module_result.success) {
            result.success = false;
            result.errors.insert(result.errors.end(), 
                module_result.errors.begin(), module_result.errors.end());
        }
    }
    
    return result;
}

std::vector<std::string> CHTLJSModuleManager::resolve_dependencies(const std::string& module_name) {
    // 简化实现，返回空依赖列表
    return {};
}

std::string CHTLJSModuleManager::generate_module_bundle() {
    std::string result = "// CHTL JS Module Bundle\n";
    
    for (const auto& pair : modules_) {
        if (pair.second->compile_result.success) {
            result += "// Module: " + pair.first + "\n";
            result += pair.second->compile_result.js_code + "\n\n";
        }
    }
    
    return result;
}

void CHTLJSModuleManager::set_compiler_options(const CHTLJSCompileOptions& options) {
    options_ = options;
    compiler_->set_options(options);
}

} // namespace chtl_js
} // namespace chtl