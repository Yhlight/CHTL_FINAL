#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <sstream>
#include "ast/ast_node.h"
#include "style/style_processor.h"
#include "import/import_manager.h"

namespace chtl {
namespace generator {

// 生成选项
struct GenerationOptions {
    bool minify_html = false;
    bool minify_css = false;
    bool minify_js = false;
    bool include_comments = true;
    bool inline_styles = false;
    bool inline_scripts = false;
    std::string indent_style = "  ";  // 缩进样式
    int indent_level = 0;             // 缩进级别
};

// 生成上下文
class GenerationContext {
private:
    std::shared_ptr<style::StyleProcessor> style_processor_;
    std::shared_ptr<import::ImportManager> import_manager_;
    GenerationOptions options_;
    std::unordered_map<std::string, std::string> variables_;
    std::vector<std::string> generated_css_;
    std::vector<std::string> generated_js_;
    
public:
    GenerationContext(std::shared_ptr<style::StyleProcessor> sp, 
                     std::shared_ptr<import::ImportManager> im,
                     const GenerationOptions& opts = GenerationOptions());
    
    // 样式处理
    void add_style(const std::string& css);
    std::string get_all_styles() const;
    
    // JavaScript处理
    void add_script(const std::string& js);
    std::string get_all_scripts() const;
    
    // 变量管理
    void set_variable(const std::string& name, const std::string& value);
    std::string get_variable(const std::string& name) const;
    bool has_variable(const std::string& name) const;
    
    // 选项管理
    const GenerationOptions& get_options() const { return options_; }
    void set_options(const GenerationOptions& opts) { options_ = opts; }
    
    // 依赖管理
    std::shared_ptr<style::StyleProcessor> get_style_processor() const { return style_processor_; }
    std::shared_ptr<import::ImportManager> get_import_manager() const { return import_manager_; }
    
    // 调试
    void print_context() const;
};

// HTML生成器
class HTMLGenerator {
private:
    std::shared_ptr<GenerationContext> context_;
    
    // 节点处理
    std::string generate_element(const std::shared_ptr<ast::ElementNode>& node);
    std::string generate_text(const std::shared_ptr<ast::TextNode>& node);
    std::string generate_style(const std::shared_ptr<ast::StyleNode>& node);
    std::string generate_script(const std::shared_ptr<ast::ScriptNode>& node);
    
    // 模板处理
    std::string generate_template_style(const std::shared_ptr<ast::ASTNode>& node);
    std::string generate_template_element(const std::shared_ptr<ast::ASTNode>& node);
    std::string generate_template_var(const std::shared_ptr<ast::ASTNode>& node);
    
    // 自定义处理
    std::string generate_custom_style(const std::shared_ptr<ast::ASTNode>& node);
    std::string generate_custom_element(const std::shared_ptr<ast::ASTNode>& node);
    std::string generate_custom_var(const std::shared_ptr<ast::ASTNode>& node);
    
    // 原始嵌入处理
    std::string generate_origin_html(const std::shared_ptr<ast::ASTNode>& node);
    std::string generate_origin_style(const std::shared_ptr<ast::ASTNode>& node);
    std::string generate_origin_javascript(const std::shared_ptr<ast::ASTNode>& node);
    
    // 导入处理
    std::string generate_import_html(const std::shared_ptr<ast::ASTNode>& node);
    std::string generate_import_style(const std::shared_ptr<ast::ASTNode>& node);
    std::string generate_import_javascript(const std::shared_ptr<ast::ASTNode>& node);
    std::string generate_import_chtl(const std::shared_ptr<ast::ASTNode>& node);
    std::string generate_import_cjmod(const std::shared_ptr<ast::ASTNode>& node);
    
    // 工具方法
    std::string generate_attributes(const std::unordered_map<std::string, std::string>& attributes);
    std::string generate_children(const std::vector<std::shared_ptr<ast::ASTNode>>& children);
    std::string escape_html(const std::string& text);
    std::string get_indent() const;
    
public:
    HTMLGenerator(std::shared_ptr<GenerationContext> context);
    
    std::string generate(const std::shared_ptr<ast::ASTNode>& root);
    std::string generate_document(const std::shared_ptr<ast::ASTNode>& root);
};

// CSS生成器
class CSSGenerator {
private:
    std::shared_ptr<GenerationContext> context_;
    
    // 样式处理
    std::string generate_style_rule(const std::string& selector, 
                                   const std::vector<style::StyleAttribute>& attributes);
    std::string generate_media_query(const std::string& media, 
                                   const std::string& content);
    std::string generate_keyframes(const std::string& name, 
                                 const std::vector<std::string>& keyframes);
    
    // 工具方法
    std::string minify_css(const std::string& css) const;
    std::string format_css(const std::string& css) const;
    
public:
    CSSGenerator(std::shared_ptr<GenerationContext> context);
    
    std::string generate(const std::shared_ptr<ast::ASTNode>& root);
    std::string generate_from_styles(const std::vector<std::string>& styles);
};

// JavaScript生成器
class JSGenerator {
private:
    std::shared_ptr<GenerationContext> context_;
    
    // 脚本处理
    std::string generate_script_content(const std::string& content);
    std::string generate_chtl_js(const std::string& content);
    std::string generate_module_imports(const std::vector<std::string>& imports);
    std::string generate_module_exports(const std::vector<std::string>& exports);
    
    // 工具方法
    std::string minify_js(const std::string& js) const;
    std::string format_js(const std::string& js) const;
    
public:
    JSGenerator(std::shared_ptr<GenerationContext> context);
    
    std::string generate(const std::shared_ptr<ast::ASTNode>& root);
    std::string generate_from_scripts(const std::vector<std::string>& scripts);
};

// 主代码生成器
class CodeGenerator {
private:
    std::shared_ptr<GenerationContext> context_;
    std::unique_ptr<HTMLGenerator> html_generator_;
    std::unique_ptr<CSSGenerator> css_generator_;
    std::unique_ptr<JSGenerator> js_generator_;
    
public:
    CodeGenerator(std::shared_ptr<style::StyleProcessor> style_processor,
                 std::shared_ptr<import::ImportManager> import_manager,
                 const GenerationOptions& options = GenerationOptions());
    
    // 主要生成接口
    std::string generate_html(const std::shared_ptr<ast::ASTNode>& root);
    std::string generate_css(const std::shared_ptr<ast::ASTNode>& root);
    std::string generate_js(const std::shared_ptr<ast::ASTNode>& root);
    
    // 完整文档生成
    std::string generate_document(const std::shared_ptr<ast::ASTNode>& root);
    std::string generate_separate_files(const std::shared_ptr<ast::ASTNode>& root,
                                      const std::string& html_file,
                                      const std::string& css_file,
                                      const std::string& js_file);
    
    // 选项管理
    void set_options(const GenerationOptions& options);
    const GenerationOptions& get_options() const;
    
    // 上下文管理
    std::shared_ptr<GenerationContext> get_context() const { return context_; }
    
    // 调试
    void print_generation_info() const;
};

} // namespace generator
} // namespace chtl