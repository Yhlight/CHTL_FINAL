#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "CHTLParser.h"
#include <string>
#include <map>

namespace CHTL {

/**
 * CHTL代码生成器
 */
class CHTLGenerator {
public:
    CHTLGenerator();
    ~CHTLGenerator();
    
    /**
     * 生成HTML代码
     * @param ast AST根节点
     * @return HTML代码
     */
    std::string generateHTML(const ASTNode* ast);
    
    /**
     * 生成CSS代码
     * @param ast AST根节点
     * @return CSS代码
     */
    std::string generateCSS(const ASTNode* ast);
    
    /**
     * 生成JavaScript代码
     * @param ast AST根节点
     * @return JavaScript代码
     */
    std::string generateJavaScript(const ASTNode* ast);
    
    /**
     * 生成完整HTML文档
     * @param ast AST根节点
     * @return 完整HTML文档
     */
    std::string generateCompleteHTML(const ASTNode* ast);

private:
    std::string globalCSS;
    std::string globalJS;
    std::map<std::string, std::string> templates;
    std::map<std::string, std::string> customElements;
    std::map<std::string, std::string> variables;
    
    std::string generateElement(const ElementNode* element);
    std::string generateStyle(const StyleNode* style);
    std::string generateScript(const ScriptNode* script);
    std::string generateTemplate(const TemplateNode* templateNode);
    std::string generateCustom(const CustomNode* customNode);
    std::string generateImport(const ImportNode* importNode);
    std::string generateNamespace(const NamespaceNode* namespaceNode);
    std::string generateOrigin(const OriginNode* origin);
    std::string generateConfiguration(const ConfigurationNode* config);
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H