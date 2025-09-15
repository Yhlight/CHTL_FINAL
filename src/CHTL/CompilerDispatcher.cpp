#include "CHTL/CompilerDispatcher.h"
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/TemplateParser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() 
    : context_(std::make_shared<CHTLContext>())
    , scanner_(std::make_unique<UnifiedScanner>())
    , template_parser_(std::make_unique<TemplateParser>()) {
}

CompilerDispatcher::CompilationResult CompilerDispatcher::compileFile(const std::string& input_file, const std::string& output_file) {
    // 读取输入文件
    std::ifstream file(input_file);
    if (!file.is_open()) {
        CompilationResult result;
        result.success = false;
        result.errors.push_back("Cannot open input file: " + input_file);
        return result;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string code = buffer.str();
    file.close();

    // 编译代码
    auto result = compileCode(code);
    
    if (result.success) {
        // 写入输出文件
        std::ofstream out_file(output_file);
        if (out_file.is_open()) {
            out_file << result.output;
            out_file.close();
        } else {
            result.success = false;
            result.errors.push_back("Cannot write to output file: " + output_file);
        }
    }

    return result;
}

CompilerDispatcher::CompilationResult CompilerDispatcher::compileCode(const std::string& code) {
    CompilationResult result;
    
    try {
        // 清空之前的片段
        clearFragments();
        
        // 解析模板
        auto templates = template_parser_->parseTemplates(code);
        
        // 使用统一扫描器扫描代码
        auto scan_result = scanner_->scan(code);
        if (!scan_result.success) {
            result.success = false;
            result.errors = scan_result.errors;
            return result;
        }

        // 处理扫描结果，创建代码片段
        for (const auto& fragment : scan_result.fragments) {
            CodeFragment code_fragment;
            code_fragment.type = identifyFragmentType(fragment.content, fragment.language_hint);
            code_fragment.content = fragment.content;
            code_fragment.language_hint = fragment.language_hint;
            code_fragment.start_line = fragment.start_line;
            code_fragment.end_line = fragment.end_line;
            code_fragment.metadata = fragment.metadata;
            
            addFragment(code_fragment);
        }
        
        // 存储模板信息
        result.templates = templates;

        // 应用模板到代码片段
        applyTemplatesToFragments(templates);
        
        // 编译所有片段
        std::vector<CompilationResult> fragment_results;
        for (const auto& fragment : fragments_) {
            CompilationResult fragment_result;
            
            switch (fragment.type) {
                case FragmentType::CHTL:
                    fragment_result = compileCHTLFragment(fragment);
                    break;
                case FragmentType::CHTL_JS:
                    fragment_result = compileCHTLJSFragment(fragment);
                    break;
                case FragmentType::CSS:
                    fragment_result = compileCSSFragment(fragment);
                    break;
                case FragmentType::JAVASCRIPT:
                    fragment_result = compileJavaScriptFragment(fragment);
                    break;
                case FragmentType::HTML:
                    fragment_result = compileHTMLFragment(fragment);
                    break;
                default:
                    fragment_result.success = false;
                    fragment_result.errors.push_back("Unknown fragment type");
                    break;
            }
            
            fragment_results.push_back(fragment_result);
        }

        // 合并结果
        result = mergeResults(fragment_results);
        
        // 后处理：应用模板、自定义元素等
        if (result.success) {
            result.output = applyTemplates(result.output);
            result.output = applyCustomElements(result.output);
            result.output = processImports(result.output);
            result.output = processNamespaces(result.output);
            result.output = processConfiguration(result.output);
        }

    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back("Compilation error: " + std::string(e.what()));
    }

    return result;
}

void CompilerDispatcher::setContext(std::shared_ptr<CHTLContext> context) {
    context_ = context;
}

std::shared_ptr<CHTLContext> CompilerDispatcher::getContext() const {
    return context_;
}

void CompilerDispatcher::addFragment(const CodeFragment& fragment) {
    fragments_.push_back(fragment);
}

void CompilerDispatcher::clearFragments() {
    fragments_.clear();
}

const std::vector<CompilerDispatcher::CodeFragment>& CompilerDispatcher::getFragments() const {
    return fragments_;
}

CompilerDispatcher::FragmentType CompilerDispatcher::identifyFragmentType(const std::string& content, const std::string& language_hint) {
    // 根据语言提示识别
    if (language_hint == "chtl") {
        return FragmentType::CHTL;
    } else if (language_hint == "chtljs" || language_hint == "cjjs") {
        return FragmentType::CHTL_JS;
    } else if (language_hint == "css") {
        return FragmentType::CSS;
    } else if (language_hint == "javascript" || language_hint == "js") {
        return FragmentType::JAVASCRIPT;
    } else if (language_hint == "html") {
        return FragmentType::HTML;
    }

    // 根据内容特征识别
    std::string trimmed_content = content;
    trimmed_content.erase(0, trimmed_content.find_first_not_of(" \t\n\r"));
    
    // 检查CHTL特征
    if (trimmed_content.find("[Template]") != std::string::npos ||
        trimmed_content.find("[Custom]") != std::string::npos ||
        trimmed_content.find("[Import]") != std::string::npos ||
        trimmed_content.find("[Namespace]") != std::string::npos ||
        trimmed_content.find("[Origin]") != std::string::npos ||
        trimmed_content.find("[Configuration]") != std::string::npos) {
        return FragmentType::CHTL;
    }
    
    // 检查CHTL JS特征
    if (trimmed_content.find("[CJMOD]") != std::string::npos ||
        trimmed_content.find("[Animation]") != std::string::npos ||
        trimmed_content.find("[Route]") != std::string::npos ||
        trimmed_content.find("$") != std::string::npos ||
        trimmed_content.find("{{") != std::string::npos) {
        return FragmentType::CHTL_JS;
    }
    
    // 检查CSS特征
    if (trimmed_content.find("{") != std::string::npos && 
        trimmed_content.find("}") != std::string::npos &&
        (trimmed_content.find(":") != std::string::npos || 
         trimmed_content.find(";") != std::string::npos)) {
        return FragmentType::CSS;
    }
    
    // 检查JavaScript特征
    if (trimmed_content.find("function") != std::string::npos ||
        trimmed_content.find("var ") != std::string::npos ||
        trimmed_content.find("let ") != std::string::npos ||
        trimmed_content.find("const ") != std::string::npos ||
        trimmed_content.find("console.log") != std::string::npos) {
        return FragmentType::JAVASCRIPT;
    }
    
    // 检查HTML特征
    if (trimmed_content.find("<") != std::string::npos && 
        trimmed_content.find(">") != std::string::npos) {
        return FragmentType::HTML;
    }

    return FragmentType::UNKNOWN;
}

CompilerDispatcher::CompilationResult CompilerDispatcher::compileCHTLFragment(const CodeFragment& fragment) {
    CompilationResult result;
    
    try {
        // 创建词法分析器
        CHTLLexer lexer;
        lexer.setInput(fragment.content);
        auto tokens = lexer.tokenize();
        
        if (tokens.empty()) {
            result.success = false;
            result.errors.push_back("No tokens generated from CHTL fragment");
            return result;
        }
        
        // 创建解析器
        CHTLParser parser;
        parser.setInput(fragment.content);
        auto ast = parser.parse();
        
        if (!ast) {
            result.success = false;
            result.errors.push_back("Failed to parse CHTL fragment");
            return result;
        }
        
        // 生成HTML
        result.output = ast->generateHTML();
        result.success = true;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back("CHTL compilation error: " + std::string(e.what()));
    }
    
    return result;
}

CompilerDispatcher::CompilationResult CompilerDispatcher::compileCHTLJSFragment(const CodeFragment& fragment) {
    CompilationResult result;
    
    try {
        // 简化的CHTL JS编译
        std::string output = fragment.content;
        
        // 处理CJMOD
        std::regex cjmod_regex(R"(\[CJMOD\]\s+(\w+)(?:\s+namespace\s+(\w+))?\s*\{([^}]+)\})");
        std::smatch match;
        
        while (std::regex_search(output, match, cjmod_regex)) {
            std::string function_name = match[1].str();
            std::string namespace_name = match[2].str();
            std::string function_body = match[3].str();
            
            std::string replacement = "// CJMOD: " + function_name + "\n";
            if (!namespace_name.empty()) {
                replacement += "// Namespace: " + namespace_name + "\n";
            }
            replacement += "function " + function_name + "() {\n" + function_body + "\n}";
            
            output = std::regex_replace(output, cjmod_regex, replacement);
        }
        
        // 处理响应式变量
        std::regex var_regex(R"(\$(\w+)\$)");
        output = std::regex_replace(output, var_regex, "{{$1}}");
        
        // 处理选择器
        std::regex selector_regex(R"(\{\{([^}]+)\}\})");
        output = std::regex_replace(output, selector_regex, "document.querySelector('$1')");
        
        result.output = output;
        result.success = true;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back("CHTL JS compilation error: " + std::string(e.what()));
    }
    
    return result;
}

CompilerDispatcher::CompilationResult CompilerDispatcher::compileCSSFragment(const CodeFragment& fragment) {
    CompilationResult result;
    result.output = "<style>\n" + fragment.content + "\n</style>";
    result.success = true;
    return result;
}

CompilerDispatcher::CompilationResult CompilerDispatcher::compileJavaScriptFragment(const CodeFragment& fragment) {
    CompilationResult result;
    result.output = "<script>\n" + fragment.content + "\n</script>";
    result.success = true;
    return result;
}

CompilerDispatcher::CompilationResult CompilerDispatcher::compileHTMLFragment(const CodeFragment& fragment) {
    CompilationResult result;
    result.output = fragment.content;
    result.success = true;
    return result;
}

CompilerDispatcher::CompilationResult CompilerDispatcher::mergeResults(const std::vector<CompilationResult>& results) {
    CompilationResult merged_result;
    merged_result.success = true;
    
    for (const auto& result : results) {
        if (!result.success) {
            merged_result.success = false;
            merged_result.errors.insert(merged_result.errors.end(), result.errors.begin(), result.errors.end());
        }
        
        merged_result.warnings.insert(merged_result.warnings.end(), result.warnings.begin(), result.warnings.end());
        
        if (result.success && !result.output.empty()) {
            merged_result.output += result.output + "\n";
        }
    }
    
    return merged_result;
}

std::string CompilerDispatcher::applyTemplates(const std::string& content) {
    std::string result = content;
    
    // 解析模板定义
    auto templates = template_parser_->parseTemplates(content);
    
    // 创建模板映射
    std::map<std::string, TemplateInfo> template_map;
    for (const auto& template_def : templates) {
        template_map[template_def.name] = template_def;
    }
    
    // 移除模板定义，只保留应用部分
    std::regex template_def_regex(R"(\[Template\][^}]+})");
    result = std::regex_replace(result, template_def_regex, "");
    
    // 应用样式模板
    for (const auto& template_pair : template_map) {
        const auto& template_info = template_pair.second;
        if (template_info.type == CHTLNode::TemplateType::STYLE) {
            result = applyStyleTemplate(result, template_info);
        } else if (template_info.type == CHTLNode::TemplateType::ELEMENT) {
            result = applyElementTemplate(result, template_info);
        } else if (template_info.type == CHTLNode::TemplateType::VARIABLE) {
            result = applyVariableTemplate(result, template_info);
        }
    }
    
    return result;
}

std::string CompilerDispatcher::applyCustomElements(const std::string& content) {
    // 简化的自定义元素应用
    std::string result = content;
    
    // 这里应该实现完整的自定义元素应用逻辑
    
    return result;
}

std::string CompilerDispatcher::processImports(const std::string& content) {
    // 简化的导入处理
    std::string result = content;
    
    // 这里应该实现完整的导入处理逻辑
    
    return result;
}

std::string CompilerDispatcher::processNamespaces(const std::string& content) {
    // 简化的命名空间处理
    std::string result = content;
    
    // 这里应该实现完整的命名空间处理逻辑
    
    return result;
}

std::string CompilerDispatcher::processConfiguration(const std::string& content) {
    // 简化的配置处理
    std::string result = content;
    
    // 这里应该实现完整的配置处理逻辑
    
    return result;
}

void CompilerDispatcher::applyTemplatesToFragments(const std::vector<TemplateInfo>& templates) {
    // 创建模板映射
    std::map<std::string, TemplateInfo> template_map;
    for (const auto& template_def : templates) {
        template_map[template_def.name] = template_def;
    }
    
    // 对每个代码片段应用模板
    for (auto& fragment : fragments_) {
        if (fragment.type == FragmentType::CHTL) {
            // 查找并应用模板
            std::string processed_content = fragment.content;
            
            // 应用样式模板
            for (const auto& template_pair : template_map) {
                const auto& template_info = template_pair.second;
                if (template_info.type == CHTLNode::TemplateType::STYLE) {
                    processed_content = applyStyleTemplate(processed_content, template_info);
                } else if (template_info.type == CHTLNode::TemplateType::ELEMENT) {
                    processed_content = applyElementTemplate(processed_content, template_info);
                } else if (template_info.type == CHTLNode::TemplateType::VARIABLE) {
                    processed_content = applyVariableTemplate(processed_content, template_info);
                }
            }
            
            fragment.content = processed_content;
        }
    }
}

std::string CompilerDispatcher::applyStyleTemplate(const std::string& content, const TemplateInfo& template_info) {
    std::string result = content;
    
    // 查找样式模板引用: @TemplateName
    std::regex style_regex("@" + template_info.name);
    std::smatch match;
    
    if (std::regex_search(result, match, style_regex)) {
        // 生成CSS样式
        std::string css_style = "style=\"";
        for (const auto& prop : template_info.properties) {
            css_style += prop.first + ": " + prop.second + "; ";
        }
        css_style += "\"";
        
        // 替换引用
        result = std::regex_replace(result, style_regex, css_style);
    }
    
    return result;
}

std::string CompilerDispatcher::applyElementTemplate(const std::string& content, const TemplateInfo& template_info) {
    std::string result = content;
    
    // 查找元素模板引用: @TemplateName
    std::regex element_regex("@" + template_info.name);
    std::smatch match;
    
    if (std::regex_search(result, match, element_regex)) {
        // 替换为模板内容
        result = std::regex_replace(result, element_regex, template_info.content);
    }
    
    return result;
}

std::string CompilerDispatcher::applyVariableTemplate(const std::string& content, const TemplateInfo& template_info) {
    std::string result = content;
    
    // 查找变量模板引用: @TemplateName.property
    std::regex var_regex("@" + template_info.name + "\\.(\\w+)");
    std::sregex_iterator iter(result.begin(), result.end(), var_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string property_name = match[1].str();
        
        auto it = template_info.properties.find(property_name);
        if (it != template_info.properties.end()) {
            result = std::regex_replace(result, std::regex(match[0].str()), it->second);
        }
    }
    
    return result;
}

} // namespace CHTL