#pragma once

#include <CHTL/CHTLNode/BaseNode.hpp>
#include <CHTL/CHTLManage/TemplateManager.hpp>
#include <string>
#include <memory>
#include <map>

namespace CHTL {

/**
 * @brief 代码生成器
 * 
 * 根据 CHTL.md 文档实现
 * 将 AST 转换为最终的 HTML/CSS/JS 代码
 */
class CodeGenerator {
public:
    CodeGenerator();
    
    // 生成 HTML 代码
    std::string generateHTML(std::shared_ptr<BaseNode> root);
    
    // 生成 CSS 代码
    std::string generateCSS(std::shared_ptr<BaseNode> root);
    
    // 生成 JavaScript 代码
    std::string generateJavaScript(std::shared_ptr<BaseNode> root);
    
    // 生成完整的输出
    std::string generateOutput(std::shared_ptr<BaseNode> root);
    
private:
    TemplateManager& templateManager_;
    std::map<std::string, std::string> generatedCSS_;
    std::map<std::string, std::string> generatedJS_;
    
    // 生成元素节点的 HTML
    std::string generateElementHTML(std::shared_ptr<BaseNode> node);
    
    // 生成文本节点的 HTML
    std::string generateTextHTML(std::shared_ptr<BaseNode> node);
    
    // 生成样式节点的 CSS
    std::string generateStyleCSS(std::shared_ptr<BaseNode> node);
    
    // 生成模板节点的代码
    std::string generateTemplateCode(std::shared_ptr<BaseNode> node);
    
    // 生成自定义节点的代码
    std::string generateCustomCode(std::shared_ptr<BaseNode> node);
    
    // 生成原始嵌入节点的代码
    std::string generateOriginCode(std::shared_ptr<BaseNode> node);
    
    // 生成导入节点的代码
    std::string generateImportCode(std::shared_ptr<BaseNode> node);
    
    // 生成配置节点的代码
    std::string generateConfigCode(std::shared_ptr<BaseNode> node);
    
    // 生成命名空间节点的代码
    std::string generateNamespaceCode(std::shared_ptr<BaseNode> node);
    
    // 生成约束节点的代码
    std::string generateConstraintCode(std::shared_ptr<BaseNode> node);
    
    // 处理模板引用
    std::string resolveTemplateReference(const std::string& reference);
    
    // 处理变量引用
    std::string resolveVariableReference(const std::string& reference);
    
    // 处理样式引用
    std::string resolveStyleReference(const std::string& reference);
    
    // 生成 HTML 属性
    std::string generateAttributes(const std::map<std::string, std::string>& attributes);
    
    // 生成 CSS 属性
    std::string generateCSSProperties(const std::map<std::string, std::string>& properties);
    
    // 清理和格式化输出
    std::string formatOutput(const std::string& html, const std::string& css, const std::string& js);
};

} // namespace CHTL