#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/CHTLBaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/OperatorNode.h"
#include <string>
#include <memory>
#include <vector>
#include <map>

namespace CHTL {

/**
 * CHTL代码生成器
 * 将CHTL AST转换为HTML/CSS/JavaScript代码
 */
class CHTLGenerator {
public:
    CHTLGenerator();
    ~CHTLGenerator();
    
    // 主要生成方法
    std::string generate(std::shared_ptr<CHTLBaseNode> ast);
    std::string generateDocument(const std::vector<std::shared_ptr<CHTLBaseNode>>& statements);
    
    // 节点类型生成
    std::string generateElement(std::shared_ptr<CHTLBaseNode> node);
    std::string generateText(std::shared_ptr<CHTLBaseNode> node);
    std::string generateComment(std::shared_ptr<CHTLBaseNode> node);
    std::string generateStyle(std::shared_ptr<CHTLBaseNode> node);
    std::string generateScript(std::shared_ptr<CHTLBaseNode> node);
    std::string generateTemplate(std::shared_ptr<CHTLBaseNode> node);
    std::string generateCustom(std::shared_ptr<CHTLBaseNode> node);
    std::string generateOrigin(std::shared_ptr<CHTLBaseNode> node);
    std::string generateImport(std::shared_ptr<CHTLBaseNode> node);
    std::string generateNamespace(std::shared_ptr<CHTLBaseNode> node);
    std::string generateConfiguration(std::shared_ptr<CHTLBaseNode> node);
    std::string generateOperator(std::shared_ptr<CHTLBaseNode> node);
    std::string generateSelector(std::shared_ptr<CHTLBaseNode> node);
    std::string generateResponsiveValue(std::shared_ptr<CHTLBaseNode> node);
    std::string generateGeneratorComment(std::shared_ptr<CHTLBaseNode> node);
    std::string generateExpression(std::shared_ptr<CHTLBaseNode> node);
    std::string generateBlock(std::shared_ptr<CHTLBaseNode> node);
    
    // 模板生成
    std::string generateTemplateStyle(std::shared_ptr<CHTLBaseNode> node);
    std::string generateTemplateElement(std::shared_ptr<CHTLBaseNode> node);
    std::string generateTemplateVar(std::shared_ptr<CHTLBaseNode> node);
    std::string generateCustomStyle(std::shared_ptr<CHTLBaseNode> node);
    std::string generateCustomElement(std::shared_ptr<CHTLBaseNode> node);
    std::string generateCustomVar(std::shared_ptr<CHTLBaseNode> node);
    
    // 原始嵌入生成
    std::string generateOriginHTML(std::shared_ptr<CHTLBaseNode> node);
    std::string generateOriginStyle(std::shared_ptr<CHTLBaseNode> node);
    std::string generateOriginJavaScript(std::shared_ptr<CHTLBaseNode> node);
    
    // 导入生成
    std::string generateImportHTML(std::shared_ptr<CHTLBaseNode> node);
    std::string generateImportStyle(std::shared_ptr<CHTLBaseNode> node);
    std::string generateImportJavaScript(std::shared_ptr<CHTLBaseNode> node);
    std::string generateImportCHTL(std::shared_ptr<CHTLBaseNode> node);
    std::string generateImportCJMOD(std::shared_ptr<CHTLBaseNode> node);
    
    // 操作符生成
    std::string generateDelete(std::shared_ptr<CHTLBaseNode> node);
    std::string generateInsert(std::shared_ptr<CHTLBaseNode> node);
    std::string generateUse(std::shared_ptr<CHTLBaseNode> node);
    
    // 配置选项
    void setGenerateHTML(bool generate);
    void setGenerateCSS(bool generate);
    void setGenerateJS(bool generate);
    void setMinifyOutput(bool minify);
    void setIncludeComments(bool include);
    
    // 输出格式
    std::string getHTMLOutput() const;
    std::string getCSSOutput() const;
    std::string getJSOutput() const;
    std::string getCombinedOutput() const;
    
    // 清理
    void clear();

private:
    // 输出缓冲区
    std::string html_output_;
    std::string css_output_;
    std::string js_output_;
    
    // 配置选项
    bool generate_html_;
    bool generate_css_;
    bool generate_js_;
    bool minify_output_;
    bool include_comments_;
    
    // 辅助方法
    std::string generateAttributes(const std::map<std::string, std::string>& attributes) const;
    std::string generateStyleProperties(const std::map<std::string, std::string>& properties) const;
    std::string generateCSSSelector(const std::string& selector) const;
    std::string generateJavaScriptCode(const std::string& code) const;
    
    // 模板处理
    std::string processTemplateVariables(const std::string& content) const;
    std::string processCustomVariables(const std::string& content) const;
    std::string processResponsiveValues(const std::string& content) const;
    
    // 代码优化
    std::string minifyHTML(const std::string& html) const;
    std::string minifyCSS(const std::string& css) const;
    std::string minifyJS(const std::string& js) const;
    
    // 工具方法
    std::string escapeHTML(const std::string& str) const;
    std::string escapeCSS(const std::string& str) const;
    std::string escapeJS(const std::string& str) const;
    std::string indentString(int level) const;
    
    // 变量和模板管理
    std::map<std::string, std::string> template_variables_;
    std::map<std::string, std::string> custom_variables_;
    std::map<std::string, std::string> responsive_variables_;
    
    // 命名空间管理
    std::map<std::string, std::string> namespace_mappings_;
    
    // 配置管理
    std::map<std::string, std::string> configurations_;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H