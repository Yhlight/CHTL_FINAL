#pragma once

#include "CHTLNode/BaseNode.h"
#include <string>
#include <memory>

namespace CHTL {

/**
 * @brief CHTL代码生成器
 * 
 * 负责将AST转换为目标代码(HTML/CSS/JS)
 */
class CHTLGenerator {
public:
    CHTLGenerator();
    ~CHTLGenerator() = default;

    /**
     * @brief 生成HTML代码
     * @param root AST根节点
     * @return HTML字符串
     */
    std::string generateHTML(std::unique_ptr<BaseNode> root);

    /**
     * @brief 生成CSS代码
     * @param root AST根节点
     * @return CSS字符串
     */
    std::string generateCSS(std::unique_ptr<BaseNode> root);

    /**
     * @brief 生成JS代码
     * @param root AST根节点
     * @return JS字符串
     */
    std::string generateJS(std::unique_ptr<BaseNode> root);

    /**
     * @brief 生成完整HTML文档
     * @param root AST根节点
     * @return 完整HTML文档
     */
    std::string generateCompleteHTML(std::unique_ptr<BaseNode> root);

private:
    std::string generateElementHTML(BaseNode* node);
    std::string generateTextHTML(BaseNode* node);
    std::string generateStyleCSS(BaseNode* node);
    std::string generateScriptJS(BaseNode* node);
    std::string generateTemplateHTML(BaseNode* node);
    std::string generateCustomHTML(BaseNode* node);
    std::string generateOriginHTML(BaseNode* node);
    std::string generateImportHTML(BaseNode* node);
    std::string generateConfigurationHTML(BaseNode* node);
    std::string generateNamespaceHTML(BaseNode* node);
};

} // namespace CHTL