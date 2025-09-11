#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/CommentNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/ScriptNode.h"
#include "CHTLStyle/CHTLStyleProcessor.h"
#include <string>
#include <map>
#include <vector>

namespace CHTL {

class CHTLGenerator {
private:
    // 配置
    bool debugMode;
    bool strictMode;
    bool generateDefaultStructure;
    std::string outputFormat;
    int indentLevel;
    std::string indentString;
    
    // 样式处理
    std::map<std::string, std::string> globalStyles;
    std::vector<std::shared_ptr<StyleNode>> localStyles;
    
    // 脚本处理
    std::vector<std::shared_ptr<ScriptNode>> localScripts;
    std::string globalScript;
    mutable std::string collectedLocalCSS;
    
    // 模板和自定义
    std::map<std::string, std::shared_ptr<BaseNode>> templates;
    std::map<std::string, std::shared_ptr<BaseNode>> customs;
    std::map<std::string, std::string> variables;
    
    // 样式处理器
    std::unique_ptr<class CHTLStyleProcessor> styleProcessor;
    
    // 错误和警告
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 辅助方法
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    std::string getIndent() const;
    void increaseIndent();
    void decreaseIndent();
    
    // 节点生成
    std::string generateNode(std::shared_ptr<BaseNode> node) const;
    std::string generateElement(std::shared_ptr<ElementNode> element) const;
    std::string generateText(std::shared_ptr<TextNode> textNode) const;
    std::string generateComment(std::shared_ptr<CommentNode> commentNode) const;
    std::string generateStyle(std::shared_ptr<StyleNode> styleNode) const;
    std::string generateScript(std::shared_ptr<ScriptNode> scriptNode) const;
    std::string generateTemplate(std::shared_ptr<BaseNode> templateNode) const;
    std::string generateCustom(std::shared_ptr<BaseNode> customNode) const;
    std::string generateOrigin(std::shared_ptr<BaseNode> originNode) const;
    std::string generateImport(std::shared_ptr<BaseNode> importNode) const;
    std::string generateNamespace(std::shared_ptr<BaseNode> namespaceNode) const;
    std::string generateConfiguration(std::shared_ptr<BaseNode> configNode) const;
    std::string generateUse(std::shared_ptr<BaseNode> useNode) const;
    
    // 样式处理
    std::string processLocalStyle(std::shared_ptr<StyleNode> styleNode) const;
    std::string processGlobalStyle(std::shared_ptr<StyleNode> styleNode) const;
    std::string generateCSS(std::shared_ptr<StyleNode> styleNode) const;
    std::string processStyleProperties(const std::map<std::string, std::string>& properties) const;
    std::string processStyleRules(const std::vector<std::shared_ptr<BaseNode>>& rules) const;
    
    // 脚本处理
    std::string processLocalScript(std::shared_ptr<ScriptNode> scriptNode) const;
    std::string processGlobalScript(std::shared_ptr<ScriptNode> scriptNode) const;
    std::string generateJavaScript(std::shared_ptr<ScriptNode> scriptNode) const;
    
    // 模板处理
    std::string processTemplate(std::shared_ptr<BaseNode> templateNode) const;
    std::string processStyleTemplate(std::shared_ptr<BaseNode> templateNode) const;
    std::string processElementTemplate(std::shared_ptr<BaseNode> templateNode) const;
    std::string processVariableTemplate(std::shared_ptr<BaseNode> templateNode) const;
    
    // 自定义处理
    std::string processCustom(std::shared_ptr<BaseNode> customNode) const;
    std::string processCustomStyle(std::shared_ptr<BaseNode> customNode) const;
    std::string processCustomElement(std::shared_ptr<BaseNode> customNode) const;
    std::string processCustomVariable(std::shared_ptr<BaseNode> customNode) const;
    
    // 属性处理
    std::string processAttributes(const std::map<std::string, std::string>& attributes) const;
    std::string processAttributeValue(const std::string& value) const;
    
    // 表达式处理
    std::string processExpression(const std::string& expression) const;
    std::string processConditionalExpression(const std::string& expression) const;
    std::string processArithmeticExpression(const std::string& expression) const;
    std::string processPropertyReference(const std::string& reference) const;
    
    // 选择器处理
    std::string processSelector(const std::string& selector) const;
    std::string processClassSelector(const std::string& className) const;
    std::string processIdSelector(const std::string& id) const;
    std::string processPseudoClassSelector(const std::string& pseudoClass) const;
    std::string processPseudoElementSelector(const std::string& pseudoElement) const;
    
    // 路径处理
    std::string processPath(const std::string& path) const;
    std::string resolvePath(const std::string& path) const;
    
    // 验证
    bool validateHTML(const std::string& html) const;
    bool validateCSS(const std::string& css) const;
    bool validateJavaScript(const std::string& js) const;

public:
    CHTLGenerator();
    ~CHTLGenerator() = default;
    
    // 配置
    void setDebugMode(bool debug) { debugMode = debug; }
    void setStrictMode(bool strict) { strictMode = strict; }
    void setGenerateDefaultStructure(bool generate) { generateDefaultStructure = generate; }
    void setOutputFormat(const std::string& format) { outputFormat = format; }
    void setIndentString(const std::string& indent) { indentString = indent; }
    
    // 主要生成方法
    std::string generate(std::shared_ptr<BaseNode> ast);
    std::string generateHTML(std::shared_ptr<BaseNode> ast);
    std::string generateCSS(std::shared_ptr<BaseNode> ast);
    std::string generateJavaScript(std::shared_ptr<BaseNode> ast);
    
    // 样式和脚本收集
    void collectStyles(std::shared_ptr<BaseNode> ast);
    void collectScripts(std::shared_ptr<BaseNode> ast);
    void collectTemplates(std::shared_ptr<BaseNode> ast);
    void collectCustoms(std::shared_ptr<BaseNode> ast);
    void collectVariables(std::shared_ptr<BaseNode> ast);
    
    // 获取收集的内容
    const std::map<std::string, std::string>& getGlobalStyles() const { return globalStyles; }
    const std::vector<std::shared_ptr<StyleNode>>& getLocalStyles() const { return localStyles; }
    const std::vector<std::shared_ptr<ScriptNode>>& getLocalScripts() const { return localScripts; }
    const std::string& getGlobalScript() const { return globalScript; }
    const std::map<std::string, std::shared_ptr<BaseNode>>& getTemplates() const { return templates; }
    const std::map<std::string, std::shared_ptr<BaseNode>>& getCustoms() const { return customs; }
    const std::map<std::string, std::string>& getVariables() const { return variables; }
    
    // 获取错误和警告
    const std::vector<std::string>& getErrors() const { return errors; }
    const std::vector<std::string>& getWarnings() const { return warnings; }
    
    // 清除错误和警告
    void clearMessages();
    
    // 重置生成器
    void reset();
    
    // 验证生成结果
    bool validate() const;
    
    // 调试信息
    std::string getDebugInfo() const;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H