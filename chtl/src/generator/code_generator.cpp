#include "generator/code_generator.h"
#include <algorithm>
#include <regex>
#include <iostream>

namespace chtl {
namespace generator {

// GenerationContext 实现
GenerationContext::GenerationContext(std::shared_ptr<style::StyleProcessor> sp, 
                                   std::shared_ptr<import::ImportManager> im,
                                   const GenerationOptions& opts)
    : style_processor_(sp), import_manager_(im), options_(opts) {
}

void GenerationContext::add_style(const std::string& css) {
    generated_css_.push_back(css);
}

std::string GenerationContext::get_all_styles() const {
    std::ostringstream oss;
    for (const auto& css : generated_css_) {
        oss << css << "\n";
    }
    return oss.str();
}

void GenerationContext::add_script(const std::string& js) {
    generated_js_.push_back(js);
}

std::string GenerationContext::get_all_scripts() const {
    std::ostringstream oss;
    for (const auto& js : generated_js_) {
        oss << js << "\n";
    }
    return oss.str();
}

void GenerationContext::set_variable(const std::string& name, const std::string& value) {
    variables_[name] = value;
}

std::string GenerationContext::get_variable(const std::string& name) const {
    auto it = variables_.find(name);
    return (it != variables_.end()) ? it->second : "";
}

bool GenerationContext::has_variable(const std::string& name) const {
    return variables_.find(name) != variables_.end();
}

void GenerationContext::print_context() const {
    std::cout << "=== Generation Context ===" << std::endl;
    std::cout << "CSS blocks: " << generated_css_.size() << std::endl;
    std::cout << "JS blocks: " << generated_js_.size() << std::endl;
    std::cout << "Variables: " << variables_.size() << std::endl;
}

// HTMLGenerator 实现
HTMLGenerator::HTMLGenerator(std::shared_ptr<GenerationContext> context) 
    : context_(context) {
}

std::string HTMLGenerator::generate(const std::shared_ptr<ast::ASTNode>& root) {
    if (!root) return "";
    
    std::ostringstream oss;
    
    switch (root->type) {
        case ast::NodeType::ROOT:
            oss << generate_children(root->children);
            break;
        case ast::NodeType::ELEMENT:
            oss << generate_element(std::static_pointer_cast<ast::ElementNode>(root));
            break;
        case ast::NodeType::TEXT:
            oss << generate_text(std::static_pointer_cast<ast::TextNode>(root));
            break;
        case ast::NodeType::STYLE:
            oss << generate_style(std::static_pointer_cast<ast::StyleNode>(root));
            break;
        case ast::NodeType::SCRIPT:
            oss << generate_script(std::static_pointer_cast<ast::ScriptNode>(root));
            break;
        case ast::NodeType::TEMPLATE_STYLE:
            oss << generate_template_style(root);
            break;
        case ast::NodeType::TEMPLATE_ELEMENT:
            oss << generate_template_element(root);
            break;
        case ast::NodeType::TEMPLATE_VAR:
            oss << generate_template_var(root);
            break;
        case ast::NodeType::CUSTOM_STYLE:
            oss << generate_custom_style(root);
            break;
        case ast::NodeType::CUSTOM_ELEMENT:
            oss << generate_custom_element(root);
            break;
        case ast::NodeType::CUSTOM_VAR:
            oss << generate_custom_var(root);
            break;
        case ast::NodeType::ORIGIN_HTML:
            oss << generate_origin_html(root);
            break;
        case ast::NodeType::ORIGIN_STYLE:
            oss << generate_origin_style(root);
            break;
        case ast::NodeType::ORIGIN_JAVASCRIPT:
            oss << generate_origin_javascript(root);
            break;
        case ast::NodeType::IMPORT_HTML:
            oss << generate_import_html(root);
            break;
        case ast::NodeType::IMPORT_STYLE:
            oss << generate_import_style(root);
            break;
        case ast::NodeType::IMPORT_JAVASCRIPT:
            oss << generate_import_javascript(root);
            break;
        case ast::NodeType::IMPORT_CHTL:
            oss << generate_import_chtl(root);
            break;
        case ast::NodeType::IMPORT_CJMOD:
            oss << generate_import_cjmod(root);
            break;
        default:
            break;
    }
    
    return oss.str();
}

std::string HTMLGenerator::generate_document(const std::shared_ptr<ast::ASTNode>& root) {
    std::ostringstream oss;
    
    oss << "<!DOCTYPE html>\n";
    oss << "<html>\n";
    oss << generate(root);
    oss << "</html>\n";
    
    return oss.str();
}

std::string HTMLGenerator::generate_element(const std::shared_ptr<ast::ElementNode>& node) {
    if (!node) return "";
    
    std::ostringstream oss;
    std::string indent = get_indent();
    
    // 开始标签
    oss << indent << "<" << node->name;
    
    // 属性
    if (!node->attributes.empty()) {
        oss << " " << generate_attributes(node->attributes);
    }
    
    // 自闭合标签
    if (node->children.empty() && node->value.empty()) {
        oss << " />\n";
        return oss.str();
    }
    
    oss << ">";
    
    // 文本内容
    if (!node->value.empty()) {
        oss << escape_html(node->value);
    }
    
    // 子元素
    if (!node->children.empty()) {
        if (!node->value.empty()) {
            oss << "\n";
        }
        oss << generate_children(node->children);
        if (!node->value.empty()) {
            oss << indent;
        }
    }
    
    // 结束标签
    oss << "</" << node->name << ">\n";
    
    return oss.str();
}

std::string HTMLGenerator::generate_text(const std::shared_ptr<ast::TextNode>& node) {
    if (!node) return "";
    return escape_html(node->value);
}

std::string HTMLGenerator::generate_style(const std::shared_ptr<ast::StyleNode>& node) {
    if (!node) return "";
    
    std::ostringstream oss;
    std::string indent = get_indent();
    
    oss << indent << "<style>\n";
    oss << node->value << "\n";
    oss << indent << "</style>\n";
    
    // 添加到上下文
    context_->add_style(node->value);
    
    return oss.str();
}

std::string HTMLGenerator::generate_script(const std::shared_ptr<ast::ScriptNode>& node) {
    if (!node) return "";
    
    std::ostringstream oss;
    std::string indent = get_indent();
    
    oss << indent << "<script>\n";
    oss << node->value << "\n";
    oss << indent << "</script>\n";
    
    // 添加到上下文
    context_->add_script(node->value);
    
    return oss.str();
}

std::string HTMLGenerator::generate_template_style(const std::shared_ptr<ast::ASTNode>& node) {
    // 模板样式处理
    auto it = node->attributes.find("content");
    if (it != node->attributes.end()) {
        context_->add_style(it->second);
        return "<!-- Template Style -->\n";
    }
    return "";
}

std::string HTMLGenerator::generate_template_element(const std::shared_ptr<ast::ASTNode>& node) {
    // 模板元素处理
    auto it = node->attributes.find("content");
    if (it != node->attributes.end()) {
        return it->second;
    }
    return "";
}

std::string HTMLGenerator::generate_template_var(const std::shared_ptr<ast::ASTNode>& node) {
    // 模板变量处理
    auto it = node->attributes.find("content");
    if (it != node->attributes.end()) {
        return it->second;
    }
    return "";
}

std::string HTMLGenerator::generate_custom_style(const std::shared_ptr<ast::ASTNode>& node) {
    // 自定义样式处理
    auto it = node->attributes.find("content");
    if (it != node->attributes.end()) {
        context_->add_style(it->second);
        return "<!-- Custom Style -->\n";
    }
    return "";
}

std::string HTMLGenerator::generate_custom_element(const std::shared_ptr<ast::ASTNode>& node) {
    // 自定义元素处理
    auto it = node->attributes.find("content");
    if (it != node->attributes.end()) {
        return it->second;
    }
    return "";
}

std::string HTMLGenerator::generate_custom_var(const std::shared_ptr<ast::ASTNode>& node) {
    // 自定义变量处理
    auto it = node->attributes.find("content");
    if (it != node->attributes.end()) {
        return it->second;
    }
    return "";
}

std::string HTMLGenerator::generate_origin_html(const std::shared_ptr<ast::ASTNode>& node) {
    // 原始HTML处理
    auto it = node->attributes.find("content");
    if (it != node->attributes.end()) {
        return it->second;
    }
    return "";
}

std::string HTMLGenerator::generate_origin_style(const std::shared_ptr<ast::ASTNode>& node) {
    // 原始样式处理
    auto it = node->attributes.find("content");
    if (it != node->attributes.end()) {
        context_->add_style(it->second);
        return "<!-- Origin Style -->\n";
    }
    return "";
}

std::string HTMLGenerator::generate_origin_javascript(const std::shared_ptr<ast::ASTNode>& node) {
    // 原始JavaScript处理
    auto it = node->attributes.find("content");
    if (it != node->attributes.end()) {
        context_->add_script(it->second);
        return "<!-- Origin JavaScript -->\n";
    }
    return "";
}

std::string HTMLGenerator::generate_import_html(const std::shared_ptr<ast::ASTNode>& node) {
    // 导入HTML处理
    auto it = node->attributes.find("content");
    if (it != node->attributes.end()) {
        return it->second;
    }
    return "";
}

std::string HTMLGenerator::generate_import_style(const std::shared_ptr<ast::ASTNode>& node) {
    // 导入样式处理
    auto it = node->attributes.find("content");
    if (it != node->attributes.end()) {
        context_->add_style(it->second);
        return "<!-- Import Style -->\n";
    }
    return "";
}

std::string HTMLGenerator::generate_import_javascript(const std::shared_ptr<ast::ASTNode>& node) {
    // 导入JavaScript处理
    auto it = node->attributes.find("content");
    if (it != node->attributes.end()) {
        context_->add_script(it->second);
        return "<!-- Import JavaScript -->\n";
    }
    return "";
}

std::string HTMLGenerator::generate_import_chtl(const std::shared_ptr<ast::ASTNode>& node) {
    // 导入CHTL处理
    auto it = node->attributes.find("content");
    if (it != node->attributes.end()) {
        return it->second;
    }
    return "";
}

std::string HTMLGenerator::generate_import_cjmod(const std::shared_ptr<ast::ASTNode>& node) {
    // 导入CJMOD处理
    auto it = node->attributes.find("content");
    if (it != node->attributes.end()) {
        return it->second;
    }
    return "";
}

std::string HTMLGenerator::generate_attributes(const std::unordered_map<std::string, std::string>& attributes) {
    std::ostringstream oss;
    bool first = true;
    
    for (const auto& pair : attributes) {
        if (!first) oss << " ";
        oss << pair.first << "=\"" << escape_html(pair.second) << "\"";
        first = false;
    }
    
    return oss.str();
}

std::string HTMLGenerator::generate_children(const std::vector<std::shared_ptr<ast::ASTNode>>& children) {
    std::ostringstream oss;
    
    for (const auto& child : children) {
        oss << generate(child);
    }
    
    return oss.str();
}

std::string HTMLGenerator::escape_html(const std::string& text) {
    std::string result = text;
    
    // 转义HTML特殊字符
    std::regex lt_regex("<");
    std::regex gt_regex(">");
    std::regex amp_regex("&");
    std::regex quot_regex("\"");
    std::regex apos_regex("'");
    
    result = std::regex_replace(result, lt_regex, "&lt;");
    result = std::regex_replace(result, gt_regex, "&gt;");
    result = std::regex_replace(result, amp_regex, "&amp;");
    result = std::regex_replace(result, quot_regex, "&quot;");
    result = std::regex_replace(result, apos_regex, "&#39;");
    
    return result;
}

std::string HTMLGenerator::get_indent() const {
    const auto& options = context_->get_options();
    std::string indent;
    for (int i = 0; i < options.indent_level; ++i) {
        indent += options.indent_style;
    }
    return indent;
}

// CSSGenerator 实现
CSSGenerator::CSSGenerator(std::shared_ptr<GenerationContext> context) 
    : context_(context) {
}

std::string CSSGenerator::generate(const std::shared_ptr<ast::ASTNode>& root) {
    // 从上下文中获取所有样式
    std::string all_styles = context_->get_all_styles();
    
    const auto& options = context_->get_options();
    if (options.minify_css) {
        return minify_css(all_styles);
    } else {
        return format_css(all_styles);
    }
}

std::string CSSGenerator::generate_from_styles(const std::vector<std::string>& styles) {
    std::ostringstream oss;
    
    for (const auto& style : styles) {
        oss << style << "\n";
    }
    
    const auto& options = context_->get_options();
    if (options.minify_css) {
        return minify_css(oss.str());
    } else {
        return format_css(oss.str());
    }
}

std::string CSSGenerator::minify_css(const std::string& css) const {
    // 简单的CSS压缩
    std::string result = css;
    
    // 移除注释
    std::regex comment_regex(R"(/\*.*?\*/)");
    result = std::regex_replace(result, comment_regex, "");
    
    // 移除多余空白
    std::regex whitespace_regex(R"(\s+)");
    result = std::regex_replace(result, whitespace_regex, " ");
    
    // 移除分号前的空格
    std::regex semicolon_regex(R"(\s*;\s*)");
    result = std::regex_replace(result, semicolon_regex, ";");
    
    return result;
}

std::string CSSGenerator::format_css(const std::string& css) const {
    // 简单的CSS格式化
    return css;
}

// JSGenerator 实现
JSGenerator::JSGenerator(std::shared_ptr<GenerationContext> context) 
    : context_(context) {
}

std::string JSGenerator::generate(const std::shared_ptr<ast::ASTNode>& root) {
    // 从上下文中获取所有脚本
    std::string all_scripts = context_->get_all_scripts();
    
    const auto& options = context_->get_options();
    if (options.minify_js) {
        return minify_js(all_scripts);
    } else {
        return format_js(all_scripts);
    }
}

std::string JSGenerator::generate_from_scripts(const std::vector<std::string>& scripts) {
    std::ostringstream oss;
    
    for (const auto& script : scripts) {
        oss << script << "\n";
    }
    
    const auto& options = context_->get_options();
    if (options.minify_js) {
        return minify_js(oss.str());
    } else {
        return format_js(oss.str());
    }
}

std::string JSGenerator::minify_js(const std::string& js) const {
    // 简单的JavaScript压缩
    std::string result = js;
    
    // 移除单行注释
    std::regex single_comment_regex(R"(//.*$)");
    result = std::regex_replace(result, single_comment_regex, "");
    
    // 移除多行注释
    std::regex multi_comment_regex(R"(/\*.*?\*/)");
    result = std::regex_replace(result, multi_comment_regex, "");
    
    // 移除多余空白
    std::regex whitespace_regex(R"(\s+)");
    result = std::regex_replace(result, whitespace_regex, " ");
    
    return result;
}

std::string JSGenerator::format_js(const std::string& js) const {
    // 简单的JavaScript格式化
    return js;
}

// CodeGenerator 实现
CodeGenerator::CodeGenerator(std::shared_ptr<style::StyleProcessor> style_processor,
                           std::shared_ptr<import::ImportManager> import_manager,
                           const GenerationOptions& options)
    : context_(std::make_shared<GenerationContext>(style_processor, import_manager, options)) {
    html_generator_ = std::make_unique<HTMLGenerator>(context_);
    css_generator_ = std::make_unique<CSSGenerator>(context_);
    js_generator_ = std::make_unique<JSGenerator>(context_);
}

std::string CodeGenerator::generate_html(const std::shared_ptr<ast::ASTNode>& root) {
    return html_generator_->generate(root);
}

std::string CodeGenerator::generate_css(const std::shared_ptr<ast::ASTNode>& root) {
    return css_generator_->generate(root);
}

std::string CodeGenerator::generate_js(const std::shared_ptr<ast::ASTNode>& root) {
    return js_generator_->generate(root);
}

std::string CodeGenerator::generate_document(const std::shared_ptr<ast::ASTNode>& root) {
    return html_generator_->generate_document(root);
}

std::string CodeGenerator::generate_separate_files(const std::shared_ptr<ast::ASTNode>& root,
                                                  const std::string& html_file,
                                                  const std::string& css_file,
                                                  const std::string& js_file) {
    // 生成分离的文件
    std::string html = generate_html(root);
    std::string css = generate_css(root);
    std::string js = generate_js(root);
    
    // 这里应该写入文件，暂时返回内容
    std::ostringstream oss;
    oss << "HTML:\n" << html << "\n\n";
    oss << "CSS:\n" << css << "\n\n";
    oss << "JS:\n" << js << "\n";
    
    return oss.str();
}

void CodeGenerator::set_options(const GenerationOptions& options) {
    context_->set_options(options);
}

const GenerationOptions& CodeGenerator::get_options() const {
    return context_->get_options();
}

void CodeGenerator::print_generation_info() const {
    context_->print_context();
}

} // namespace generator
} // namespace chtl