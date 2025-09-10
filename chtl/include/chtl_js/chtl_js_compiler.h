#pragma once

#include "chtl_js/chtl_js_lexer.h"
#include "chtl_js/chtl_js_parser.h"
#include "chtl_js/chtl_js_ast.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace chtl {
namespace chtl_js {

// CHTL JS编译选项
struct CHTLJSCompileOptions {
    bool enable_selector_auto_add = true;
    bool enable_arrow_syntax = true;
    bool enable_util_expressions = true;
    bool enable_vir_objects = true;
    bool enable_ineveraway = true;
    bool minify_output = false;
    bool debug_mode = false;
};

// CHTL JS编译结果
struct CHTLJSCompileResult {
    std::string js_code;
    std::vector<std::string> dependencies;
    std::vector<std::string> selectors;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    bool success;
    
    CHTLJSCompileResult() : success(false) {}
};

// CHTL JS编译器
class CHTLJSCompiler {
public:
    explicit CHTLJSCompiler(const CHTLJSCompileOptions& options = CHTLJSCompileOptions{});
    
    // 主要接口
    CHTLJSCompileResult compile(const std::string& source);
    CHTLJSCompileResult compile_file(const std::string& file_path);
    
    // 设置选项
    void set_options(const CHTLJSCompileOptions& options);
    CHTLJSCompileOptions get_options() const;
    
    // 重置状态
    void reset();
    
private:
    CHTLJSCompileOptions options_;
    std::unique_ptr<CHTLJSLexer> lexer_;
    std::unique_ptr<CHTLJSParser> parser_;
    std::shared_ptr<ProgramNode> ast_;
    
    // 编译阶段
    CHTLJSCompileResult lex_and_parse(const std::string& source);
    CHTLJSCompileResult generate_js(const std::shared_ptr<ProgramNode>& ast);
    
    // 代码生成
    std::string generate_program(const std::shared_ptr<ProgramNode>& node);
    std::string generate_statement(const std::shared_ptr<CHTLJSASTNode>& node);
    std::string generate_expression(const std::shared_ptr<CHTLJSASTNode>& node);
    std::string generate_identifier(const std::shared_ptr<IdentifierNode>& node);
    std::string generate_literal(const std::shared_ptr<LiteralNode>& node);
    std::string generate_binary_expression(const std::shared_ptr<BinaryExpressionNode>& node);
    std::string generate_call_expression(const std::shared_ptr<CallExpressionNode>& node);
    std::string generate_member_expression(const std::shared_ptr<MemberExpressionNode>& node);
    std::string generate_variable_declaration(const std::shared_ptr<VariableDeclarationNode>& node);
    std::string generate_function_declaration(const std::shared_ptr<FunctionDeclarationNode>& node);
    std::string generate_block_statement(const std::shared_ptr<BlockStatementNode>& node);
    
    // CHTL JS特有语法生成
    std::string generate_fileloader(const std::shared_ptr<FileloaderNode>& node);
    std::string generate_listen(const std::shared_ptr<ListenNode>& node);
    std::string generate_animate(const std::shared_ptr<AnimateNode>& node);
    std::string generate_vir(const std::shared_ptr<VirNode>& node);
    std::string generate_router(const std::shared_ptr<RouterNode>& node);
    std::string generate_ineveraway(const std::shared_ptr<INeverAwayNode>& node);
    std::string generate_util(const std::shared_ptr<UtilNode>& node);
    std::string generate_selector(const std::shared_ptr<SelectorNode>& node);
    
    // 辅助方法
    std::string generate_operator(CHTLJSTokenType type);
    std::string generate_selector_js(const std::string& selector);
    std::string generate_self_reference();
    std::string generate_arrow_syntax(const std::string& expression);
    
    // 选择器处理
    std::vector<std::string> extract_selectors(const std::shared_ptr<ProgramNode>& ast);
    std::string generate_selector_function(const std::string& selector);
    std::string generate_auto_selector_code();
    
    // 依赖处理
    std::vector<std::string> extract_dependencies(const std::shared_ptr<ProgramNode>& ast);
    std::string generate_dependency_loader(const std::vector<std::string>& dependencies);
    
    // 错误处理
    void add_error(const std::string& error);
    void add_warning(const std::string& warning);
    
    // 代码优化
    std::string minify_js(const std::string& js_code);
    std::string optimize_js(const std::string& js_code);
    
    // 调试支持
    std::string generate_debug_info(const std::shared_ptr<ProgramNode>& ast);
    
    // 编译状态
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    std::vector<std::string> selectors_;
    std::vector<std::string> dependencies_;
};

// CHTL JS模块系统
class CHTLJSModule {
public:
    std::string name;
    std::string source;
    std::shared_ptr<ProgramNode> ast;
    CHTLJSCompileResult compile_result;
    
    CHTLJSModule(const std::string& n, const std::string& src) 
        : name(n), source(src) {}
};

// CHTL JS模块管理器
class CHTLJSModuleManager {
public:
    CHTLJSModuleManager();
    
    // 模块管理
    void add_module(const std::string& name, const std::string& source);
    void remove_module(const std::string& name);
    std::shared_ptr<CHTLJSModule> get_module(const std::string& name);
    
    // 编译管理
    CHTLJSCompileResult compile_module(const std::string& name);
    CHTLJSCompileResult compile_all_modules();
    
    // 依赖解析
    std::vector<std::string> resolve_dependencies(const std::string& module_name);
    std::string generate_module_bundle();
    
    // 设置编译器选项
    void set_compiler_options(const CHTLJSCompileOptions& options);
    
private:
    std::unordered_map<std::string, std::shared_ptr<CHTLJSModule>> modules_;
    std::unique_ptr<CHTLJSCompiler> compiler_;
    CHTLJSCompileOptions options_;
    
    // 依赖图
    std::unordered_map<std::string, std::vector<std::string>> dependency_graph_;
    
    // 依赖解析
    void build_dependency_graph();
    std::vector<std::string> topological_sort();
    bool has_circular_dependency();
};

} // namespace chtl_js
} // namespace chtl