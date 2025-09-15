#include "CHTLGenerator.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

CHTLGenerator::CHTLGenerator()
    : generate_html_(true), generate_css_(true), generate_js_(true),
      minify_output_(false), include_comments_(true) {
}

CHTLGenerator::~CHTLGenerator() {
}

std::string CHTLGenerator::generate(std::shared_ptr<CHTLBaseNode> ast) {
    if (!ast) {
        return "";
    }
    
    switch (ast->getType()) {
        case CHTLBaseNode::NodeType::ELEMENT:
            return generateElement(ast);
        case CHTLBaseNode::NodeType::TEXT:
            return generateText(ast);
        case CHTLBaseNode::NodeType::COMMENT:
            return generateComment(ast);
        case CHTLBaseNode::NodeType::STYLE:
            return generateStyle(ast);
        case CHTLBaseNode::NodeType::SCRIPT:
            return generateScript(ast);
        case CHTLBaseNode::NodeType::TEMPLATE:
            return generateTemplate(ast);
        case CHTLBaseNode::NodeType::CUSTOM:
            return generateCustom(ast);
        case CHTLBaseNode::NodeType::ORIGIN:
            return generateOrigin(ast);
        case CHTLBaseNode::NodeType::IMPORT:
            return generateImport(ast);
        case CHTLBaseNode::NodeType::NAMESPACE:
            return generateNamespace(ast);
        case CHTLBaseNode::NodeType::CONFIGURATION:
            return generateConfiguration(ast);
        case CHTLBaseNode::NodeType::DELETE:
        case CHTLBaseNode::NodeType::INSERT:
        case CHTLBaseNode::NodeType::USE:
            return generateOperator(ast);
        case CHTLBaseNode::NodeType::SELECTOR:
            return generateSelector(ast);
        case CHTLBaseNode::NodeType::RESPONSIVE_VALUE:
            return generateResponsiveValue(ast);
        case CHTLBaseNode::NodeType::GENERATOR_COMMENT:
            return generateGeneratorComment(ast);
        case CHTLBaseNode::NodeType::EXPRESSION:
            return generateExpression(ast);
        case CHTLBaseNode::NodeType::BLOCK:
            return generateBlock(ast);
        default:
            return ast->generateCode();
    }
}

std::string CHTLGenerator::generateDocument(const std::vector<std::shared_ptr<CHTLBaseNode>>& statements) {
    html_output_.clear();
    css_output_.clear();
    js_output_.clear();
    
    std::stringstream html_ss;
    std::stringstream css_ss;
    std::stringstream js_ss;
    
    // 生成HTML文档结构
    if (generate_html_) {
        html_ss << "<!DOCTYPE html>\n<html>\n<head>\n";
        html_ss << "    <meta charset=\"UTF-8\">\n";
        html_ss << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
        html_ss << "    <title>CHTL Generated Document</title>\n";
        
        if (generate_css_) {
            html_ss << "    <style>\n";
            html_ss << "        /* CHTL Generated CSS */\n";
            html_ss << "    </style>\n";
        }
        
        html_ss << "</head>\n<body>\n";
    }
    
    // 处理所有语句
    for (const auto& statement : statements) {
        std::string generated = generate(statement);
        
        if (generate_html_) {
            html_ss << generated << "\n";
        }
    }
    
    if (generate_html_) {
        html_ss << "</body>\n</html>";
    }
    
    html_output_ = html_ss.str();
    css_output_ = css_ss.str();
    js_output_ = js_ss.str();
    
    // 应用优化
    if (minify_output_) {
        html_output_ = minifyHTML(html_output_);
        css_output_ = minifyCSS(css_output_);
        js_output_ = minifyJS(js_output_);
    }
    
    return getCombinedOutput();
}

std::string CHTLGenerator::generateElement(std::shared_ptr<CHTLBaseNode> node) {
    auto element = std::dynamic_pointer_cast<ElementNode>(node);
    if (!element) return "";
    
    std::stringstream ss;
    
    // 开始标签
    ss << "<" << element->getTagName();
    
    // 属性
    std::string attributes = generateAttributes(element->getAttributes());
    if (!attributes.empty()) {
        ss << " " << attributes;
    }
    
    if (element->isSelfClosing()) {
        ss << " />";
        return ss.str();
    }
    
    ss << ">";
    
    // 文本内容
    if (element->hasTextContent()) {
        ss << escapeHTML(element->getTextContent());
    }
    
    // 子节点
    for (const auto& child : element->getChildren()) {
        ss << generate(child);
    }
    
    // 结束标签
    ss << "</" << element->getTagName() << ">";
    
    return ss.str();
}

std::string CHTLGenerator::generateText(std::shared_ptr<CHTLBaseNode> node) {
    auto text = std::dynamic_pointer_cast<TextNode>(node);
    if (!text) return "";
    
    return escapeHTML(text->getText());
}

std::string CHTLGenerator::generateComment(std::shared_ptr<CHTLBaseNode> node) {
    auto comment = std::dynamic_pointer_cast<CommentNode>(node);
    if (!comment) return "";
    
    if (include_comments_) {
        return "<!-- " + comment->getComment() + " -->";
    }
    return "";
}

std::string CHTLGenerator::generateStyle(std::shared_ptr<CHTLBaseNode> node) {
    auto style = std::dynamic_pointer_cast<StyleNode>(node);
    if (!style) return "";
    
    if (style->isInlineStyle()) {
        // 内联样式
        std::stringstream ss;
        ss << "style=\"";
        bool first = true;
        for (const auto& prop : style->getStyleProperties()) {
            if (!first) ss << "; ";
            ss << prop.first << ": " << prop.second;
            first = false;
        }
        ss << "\"";
        return ss.str();
    } else {
        // CSS样式块
        std::stringstream ss;
        
        // 选择器
        if (style->hasSelector()) {
            ss << style->getSelector() << " ";
        } else if (!style->getClassSelectors().empty()) {
            ss << "." << style->getClassSelectors()[0] << " ";
        } else if (!style->getIdSelectors().empty()) {
            ss << "#" << style->getIdSelectors()[0] << " ";
        }
        
        // 伪选择器
        for (const auto& pseudo : style->getPseudoSelectors()) {
            ss << pseudo << " ";
        }
        
        ss << "{\n";
        
        // 样式属性
        for (const auto& prop : style->getStyleProperties()) {
            ss << "    " << prop.first << ": " << prop.second << ";\n";
        }
        
        ss << "}";
        return ss.str();
    }
}

std::string CHTLGenerator::generateScript(std::shared_ptr<CHTLBaseNode> node) {
    auto script = std::dynamic_pointer_cast<ScriptNode>(node);
    if (!script) return "";
    
    std::stringstream ss;
    ss << "<script>\n";
    ss << script->getScriptContent() << "\n";
    ss << "</script>";
    return ss.str();
}

std::string CHTLGenerator::generateTemplate(std::shared_ptr<CHTLBaseNode> node) {
    auto template_node = std::dynamic_pointer_cast<TemplateNode>(node);
    if (!template_node) return "";
    
    std::stringstream ss;
    ss << "[Template] " << template_node->getName() << " {\n";
    
    for (const auto& child : template_node->getChildren()) {
        ss << "    " << generate(child) << "\n";
    }
    
    ss << "}";
    return ss.str();
}

std::string CHTLGenerator::generateCustom(std::shared_ptr<CHTLBaseNode> node) {
    auto custom = std::dynamic_pointer_cast<CustomNode>(node);
    if (!custom) return "";
    
    std::stringstream ss;
    ss << "[Custom] " << custom->getName() << " {\n";
    
    for (const auto& child : custom->getChildren()) {
        ss << "    " << generate(child) << "\n";
    }
    
    ss << "}";
    return ss.str();
}

std::string CHTLGenerator::generateOrigin(std::shared_ptr<CHTLBaseNode> node) {
    auto origin = std::dynamic_pointer_cast<OriginNode>(node);
    if (!origin) return "";
    
    return origin->getContent();
}

std::string CHTLGenerator::generateImport(std::shared_ptr<CHTLBaseNode> node) {
    auto import = std::dynamic_pointer_cast<ImportNode>(node);
    if (!import) return "";
    
    std::stringstream ss;
    ss << "[Import] ";
    
    switch (import->getType()) {
        case CHTLBaseNode::NodeType::IMPORT_HTML:
            ss << "@Html";
            break;
        case CHTLBaseNode::NodeType::IMPORT_STYLE:
            ss << "@Style";
            break;
        case CHTLBaseNode::NodeType::IMPORT_JAVASCRIPT:
            ss << "@JavaScript";
            break;
        case CHTLBaseNode::NodeType::IMPORT_CHTL:
            ss << "@Chtl";
            break;
        case CHTLBaseNode::NodeType::IMPORT_CJMOD:
            ss << "@CJmod";
            break;
        default:
            ss << "@Unknown";
            break;
    }
    
    ss << " from \"" << import->getPath() << "\"";
    
    if (import->hasAlias()) {
        ss << " as " << import->getAlias();
    }
    
    if (import->hasNamespace()) {
        ss << " from " << import->getNamespace();
    }
    
    return ss.str();
}

std::string CHTLGenerator::generateNamespace(std::shared_ptr<CHTLBaseNode> node) {
    auto namespace_node = std::dynamic_pointer_cast<NamespaceNode>(node);
    if (!namespace_node) return "";
    
    std::stringstream ss;
    ss << "[Namespace] " << namespace_node->getName() << " {\n";
    
    for (const auto& child : namespace_node->getChildren()) {
        ss << "    " << generate(child) << "\n";
    }
    
    ss << "}";
    return ss.str();
}

std::string CHTLGenerator::generateConfiguration(std::shared_ptr<CHTLBaseNode> node) {
    auto config = std::dynamic_pointer_cast<ConfigurationNode>(node);
    if (!config) return "";
    
    std::stringstream ss;
    ss << "[Configuration]";
    if (config->hasName()) {
        ss << " @Config " << config->getName();
    }
    ss << " {\n";
    
    for (const auto& conf : config->getConfigurations()) {
        ss << "    " << conf.first << " = " << conf.second << ";\n";
    }
    
    ss << "}";
    return ss.str();
}

std::string CHTLGenerator::generateOperator(std::shared_ptr<CHTLBaseNode> node) {
    switch (node->getType()) {
        case CHTLBaseNode::NodeType::DELETE:
            return generateDelete(node);
        case CHTLBaseNode::NodeType::INSERT:
            return generateInsert(node);
        case CHTLBaseNode::NodeType::USE:
            return generateUse(node);
        default:
            return "";
    }
}

std::string CHTLGenerator::generateSelector(std::shared_ptr<CHTLBaseNode> node) {
    auto selector = std::dynamic_pointer_cast<SelectorNode>(node);
    if (!selector) return "";
    
    return "{{" + selector->getSelector() + "}}";
}

std::string CHTLGenerator::generateResponsiveValue(std::shared_ptr<CHTLBaseNode> node) {
    auto responsive = std::dynamic_pointer_cast<ResponsiveValueNode>(node);
    if (!responsive) return "";
    
    return "$" + responsive->getVariableName() + "$";
}

std::string CHTLGenerator::generateGeneratorComment(std::shared_ptr<CHTLBaseNode> node) {
    auto comment = std::dynamic_pointer_cast<GeneratorCommentNode>(node);
    if (!comment) return "";
    
    if (include_comments_) {
        return comment->generateCode();
    }
    return "";
}

std::string CHTLGenerator::generateExpression(std::shared_ptr<CHTLBaseNode> node) {
    auto expression = std::dynamic_pointer_cast<ExpressionNode>(node);
    if (!expression) return "";
    
    return expression->getExpression();
}

std::string CHTLGenerator::generateBlock(std::shared_ptr<CHTLBaseNode> node) {
    auto block = std::dynamic_pointer_cast<BlockNode>(node);
    if (!block) return "";
    
    std::stringstream ss;
    ss << "{\n";
    
    for (const auto& statement : block->getStatements()) {
        ss << "    " << generate(statement) << "\n";
    }
    
    ss << "}";
    return ss.str();
}

// 模板生成方法
std::string CHTLGenerator::generateTemplateStyle(std::shared_ptr<CHTLBaseNode> node) {
    auto template_style = std::dynamic_pointer_cast<TemplateStyleNode>(node);
    if (!template_style) return "";
    
    std::stringstream ss;
    ss << "[Template] @Style " << template_style->getName() << " {\n";
    
    for (const auto& prop : template_style->getStyleProperties()) {
        ss << "    " << prop.first << ": " << prop.second << ";\n";
    }
    
    ss << "}";
    return ss.str();
}

std::string CHTLGenerator::generateTemplateElement(std::shared_ptr<CHTLBaseNode> node) {
    auto template_element = std::dynamic_pointer_cast<TemplateElementNode>(node);
    if (!template_element) return "";
    
    std::stringstream ss;
    ss << "[Template] @Element " << template_element->getName() << " {\n";
    
    for (const auto& child : template_element->getChildren()) {
        ss << "    " << generate(child) << "\n";
    }
    
    ss << "}";
    return ss.str();
}

std::string CHTLGenerator::generateTemplateVar(std::shared_ptr<CHTLBaseNode> node) {
    auto template_var = std::dynamic_pointer_cast<TemplateVarNode>(node);
    if (!template_var) return "";
    
    std::stringstream ss;
    ss << "[Template] @Var " << template_var->getName() << " {\n";
    
    for (const auto& var : template_var->getVariables()) {
        ss << "    " << var.first << ": " << var.second << ";\n";
    }
    
    ss << "}";
    return ss.str();
}

std::string CHTLGenerator::generateCustomStyle(std::shared_ptr<CHTLBaseNode> node) {
    auto custom_style = std::dynamic_pointer_cast<CustomStyleNode>(node);
    if (!custom_style) return "";
    
    std::stringstream ss;
    ss << "[Custom] @Style " << custom_style->getName() << " {\n";
    
    for (const auto& prop : custom_style->getStyleProperties()) {
        ss << "    " << prop.first << ": " << prop.second << ";\n";
    }
    
    ss << "}";
    return ss.str();
}

std::string CHTLGenerator::generateCustomElement(std::shared_ptr<CHTLBaseNode> node) {
    auto custom_element = std::dynamic_pointer_cast<CustomElementNode>(node);
    if (!custom_element) return "";
    
    std::stringstream ss;
    ss << "[Custom] @Element " << custom_element->getName() << " {\n";
    
    for (const auto& child : custom_element->getChildren()) {
        ss << "    " << generate(child) << "\n";
    }
    
    ss << "}";
    return ss.str();
}

std::string CHTLGenerator::generateCustomVar(std::shared_ptr<CHTLBaseNode> node) {
    auto custom_var = std::dynamic_pointer_cast<CustomVarNode>(node);
    if (!custom_var) return "";
    
    std::stringstream ss;
    ss << "[Custom] @Var " << custom_var->getName() << " {\n";
    
    for (const auto& var : custom_var->getVariables()) {
        ss << "    " << var.first << ": " << var.second << ";\n";
    }
    
    ss << "}";
    return ss.str();
}

// 原始嵌入生成方法
std::string CHTLGenerator::generateOriginHTML(std::shared_ptr<CHTLBaseNode> node) {
    auto origin = std::dynamic_pointer_cast<OriginNode>(node);
    if (!origin) return "";
    
    return origin->getContent();
}

std::string CHTLGenerator::generateOriginStyle(std::shared_ptr<CHTLBaseNode> node) {
    auto origin = std::dynamic_pointer_cast<OriginNode>(node);
    if (!origin) return "";
    
    return origin->getContent();
}

std::string CHTLGenerator::generateOriginJavaScript(std::shared_ptr<CHTLBaseNode> node) {
    auto origin = std::dynamic_pointer_cast<OriginNode>(node);
    if (!origin) return "";
    
    return origin->getContent();
}

// 导入生成方法
std::string CHTLGenerator::generateImportHTML(std::shared_ptr<CHTLBaseNode> node) {
    auto import = std::dynamic_pointer_cast<ImportNode>(node);
    if (!import) return "";
    
    return "<!-- Import HTML: " + import->getPath() + " -->";
}

std::string CHTLGenerator::generateImportStyle(std::shared_ptr<CHTLBaseNode> node) {
    auto import = std::dynamic_pointer_cast<ImportNode>(node);
    if (!import) return "";
    
    return "/* Import CSS: " + import->getPath() + " */";
}

std::string CHTLGenerator::generateImportJavaScript(std::shared_ptr<CHTLBaseNode> node) {
    auto import = std::dynamic_pointer_cast<ImportNode>(node);
    if (!import) return "";
    
    return "// Import JS: " + import->getPath();
}

std::string CHTLGenerator::generateImportCHTL(std::shared_ptr<CHTLBaseNode> node) {
    auto import = std::dynamic_pointer_cast<ImportNode>(node);
    if (!import) return "";
    
    return "<!-- Import CHTL: " + import->getPath() + " -->";
}

std::string CHTLGenerator::generateImportCJMOD(std::shared_ptr<CHTLBaseNode> node) {
    auto import = std::dynamic_pointer_cast<ImportNode>(node);
    if (!import) return "";
    
    return "// Import CJMOD: " + import->getPath();
}

// 操作符生成方法
std::string CHTLGenerator::generateDelete(std::shared_ptr<CHTLBaseNode> node) {
    auto delete_node = std::dynamic_pointer_cast<DeleteNode>(node);
    if (!delete_node) return "";
    
    std::stringstream ss;
    ss << "delete ";
    
    const auto& targets = delete_node->getTargets();
    for (size_t i = 0; i < targets.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << targets[i];
    }
    
    return ss.str();
}

std::string CHTLGenerator::generateInsert(std::shared_ptr<CHTLBaseNode> node) {
    auto insert_node = std::dynamic_pointer_cast<InsertNode>(node);
    if (!insert_node) return "";
    
    std::stringstream ss;
    ss << "insert " << insert_node->getPosition() << " " << insert_node->getTarget();
    
    if (insert_node->getContent()) {
        ss << " " << generate(insert_node->getContent());
    }
    
    return ss.str();
}

std::string CHTLGenerator::generateUse(std::shared_ptr<CHTLBaseNode> node) {
    auto use_node = std::dynamic_pointer_cast<UseNode>(node);
    if (!use_node) return "";
    
    return "use " + use_node->getTarget();
}

// 配置方法
void CHTLGenerator::setGenerateHTML(bool generate) {
    generate_html_ = generate;
}

void CHTLGenerator::setGenerateCSS(bool generate) {
    generate_css_ = generate;
}

void CHTLGenerator::setGenerateJS(bool generate) {
    generate_js_ = generate;
}

void CHTLGenerator::setMinifyOutput(bool minify) {
    minify_output_ = minify;
}

void CHTLGenerator::setIncludeComments(bool include) {
    include_comments_ = include;
}

std::string CHTLGenerator::getHTMLOutput() const {
    return html_output_;
}

std::string CHTLGenerator::getCSSOutput() const {
    return css_output_;
}

std::string CHTLGenerator::getJSOutput() const {
    return js_output_;
}

std::string CHTLGenerator::getCombinedOutput() const {
    std::stringstream ss;
    
    if (generate_html_) {
        ss << html_output_;
    }
    
    if (generate_css_ && !css_output_.empty()) {
        ss << "\n<style>\n" << css_output_ << "\n</style>\n";
    }
    
    if (generate_js_ && !js_output_.empty()) {
        ss << "\n<script>\n" << js_output_ << "\n</script>\n";
    }
    
    return ss.str();
}

void CHTLGenerator::clear() {
    html_output_.clear();
    css_output_.clear();
    js_output_.clear();
    template_variables_.clear();
    custom_variables_.clear();
    responsive_variables_.clear();
    namespace_mappings_.clear();
    configurations_.clear();
}

// 辅助方法实现
std::string CHTLGenerator::generateAttributes(const std::map<std::string, std::string>& attributes) const {
    std::stringstream ss;
    bool first = true;
    
    for (const auto& attr : attributes) {
        if (!first) ss << " ";
        ss << attr.first;
        if (!attr.second.empty()) {
            ss << "=\"" << escapeHTML(attr.second) << "\"";
        }
        first = false;
    }
    
    return ss.str();
}

std::string CHTLGenerator::generateStyleProperties(const std::map<std::string, std::string>& properties) const {
    std::stringstream ss;
    bool first = true;
    
    for (const auto& prop : properties) {
        if (!first) ss << "; ";
        ss << prop.first << ": " << prop.second;
        first = false;
    }
    
    return ss.str();
}

std::string CHTLGenerator::generateCSSSelector(const std::string& selector) const {
    return selector;
}

std::string CHTLGenerator::generateJavaScriptCode(const std::string& code) const {
    return code;
}

std::string CHTLGenerator::processTemplateVariables(const std::string& content) const {
    std::string result = content;
    
    for (const auto& var : template_variables_) {
        std::string placeholder = "{{" + var.first + "}}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), var.second);
            pos += var.second.length();
        }
    }
    
    return result;
}

std::string CHTLGenerator::processCustomVariables(const std::string& content) const {
    std::string result = content;
    
    for (const auto& var : custom_variables_) {
        std::string placeholder = "{{" + var.first + "}}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), var.second);
            pos += var.second.length();
        }
    }
    
    return result;
}

std::string CHTLGenerator::processResponsiveValues(const std::string& content) const {
    std::string result = content;
    
    for (const auto& var : responsive_variables_) {
        std::string placeholder = "$" + var.first + "$";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), var.second);
            pos += var.second.length();
        }
    }
    
    return result;
}

std::string CHTLGenerator::minifyHTML(const std::string& html) const {
    // 简单的HTML压缩
    std::string result = html;
    
    // 移除多余空白
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    result = std::regex_replace(result, std::regex(">\\s+<"), "><");
    
    return result;
}

std::string CHTLGenerator::minifyCSS(const std::string& css) const {
    // 简单的CSS压缩
    std::string result = css;
    
    // 移除注释
    result = std::regex_replace(result, std::regex("/\\*.*?\\*/"), "");
    
    // 移除多余空白
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    result = std::regex_replace(result, std::regex("\\s*{\\s*"), "{");
    result = std::regex_replace(result, std::regex("\\s*}\\s*"), "}");
    result = std::regex_replace(result, std::regex("\\s*;\\s*"), ";");
    
    return result;
}

std::string CHTLGenerator::minifyJS(const std::string& js) const {
    // 简单的JS压缩
    std::string result = js;
    
    // 移除注释
    result = std::regex_replace(result, std::regex("//.*"), "");
    result = std::regex_replace(result, std::regex("/\\*.*?\\*/"), "");
    
    // 移除多余空白
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    result = std::regex_replace(result, std::regex("\\s*{\\s*"), "{");
    result = std::regex_replace(result, std::regex("\\s*}\\s*"), "}");
    result = std::regex_replace(result, std::regex("\\s*;\\s*"), ";");
    
    return result;
}

std::string CHTLGenerator::escapeHTML(const std::string& str) const {
    std::string result = str;
    
    // 转义HTML特殊字符
    result = std::regex_replace(result, std::regex("&"), "&amp;");
    result = std::regex_replace(result, std::regex("<"), "&lt;");
    result = std::regex_replace(result, std::regex(">"), "&gt;");
    result = std::regex_replace(result, std::regex("\""), "&quot;");
    result = std::regex_replace(result, std::regex("'"), "&#39;");
    
    return result;
}

std::string CHTLGenerator::escapeCSS(const std::string& str) const {
    // CSS转义
    return str;
}

std::string CHTLGenerator::escapeJS(const std::string& str) const {
    // JavaScript转义
    return str;
}

std::string CHTLGenerator::indentString(int level) const {
    return std::string(level * 4, ' ');
}

} // namespace CHTL