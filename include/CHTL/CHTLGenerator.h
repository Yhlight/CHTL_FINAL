#pragma once

#include "CHTLNode.h"
#include <string>
#include <memory>

namespace CHTL {

/**
 * @brief CHTL代码生成器
 * 将AST转换为HTML、CSS、JavaScript代码
 */
class CHTLGenerator {
public:
    CHTLGenerator();
    ~CHTLGenerator() = default;

    /**
     * @brief 生成HTML代码
     * @param ast AST根节点
     * @return 生成的HTML代码
     */
    std::string generateHTML(std::shared_ptr<CHTLNode> ast);

    /**
     * @brief 生成CSS代码
     * @param ast AST根节点
     * @return 生成的CSS代码
     */
    std::string generateCSS(std::shared_ptr<CHTLNode> ast);

    /**
     * @brief 生成JavaScript代码
     * @param ast AST根节点
     * @return 生成的JavaScript代码
     */
    std::string generateJavaScript(std::shared_ptr<CHTLNode> ast);

    /**
     * @brief 生成完整的HTML文档
     * @param ast AST根节点
     * @return 完整的HTML文档
     */
    std::string generateCompleteHTML(std::shared_ptr<CHTLNode> ast);

    /**
     * @brief 设置是否生成默认HTML结构
     * @param enabled 是否启用
     */
    void setDefaultStructure(bool enabled);

    /**
     * @brief 设置缩进字符
     * @param indent 缩进字符
     */
    void setIndent(const std::string& indent);

    /**
     * @brief 设置是否压缩输出
     * @param minify 是否压缩
     */
    void setMinify(bool minify);

private:
    /**
     * @brief 生成元素HTML
     * @param node 元素节点
     * @param indent_level 缩进级别
     * @return 生成的HTML
     */
    std::string generateElementHTML(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成文本HTML
     * @param node 文本节点
     * @param indent_level 缩进级别
     * @return 生成的HTML
     */
    std::string generateTextHTML(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成注释HTML
     * @param node 注释节点
     * @param indent_level 缩进级别
     * @return 生成的HTML
     */
    std::string generateCommentHTML(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成样式HTML
     * @param node 样式节点
     * @param indent_level 缩进级别
     * @return 生成的HTML
     */
    std::string generateStyleHTML(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成脚本HTML
     * @param node 脚本节点
     * @param indent_level 缩进级别
     * @return 生成的HTML
     */
    std::string generateScriptHTML(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成模板HTML
     * @param node 模板节点
     * @param indent_level 缩进级别
     * @return 生成的HTML
     */
    std::string generateTemplateHTML(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成自定义HTML
     * @param node 自定义节点
     * @param indent_level 缩进级别
     * @return 生成的HTML
     */
    std::string generateCustomHTML(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成原始嵌入HTML
     * @param node 原始嵌入节点
     * @param indent_level 缩进级别
     * @return 生成的HTML
     */
    std::string generateOriginHTML(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成属性字符串
     * @param node 节点
     * @return 属性字符串
     */
    std::string generateAttributes(std::shared_ptr<CHTLNode> node);

    /**
     * @brief 生成CSS样式
     * @param node 样式节点
     * @param indent_level 缩进级别
     * @return 生成的CSS
     */
    std::string generateCSSStyle(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成JavaScript代码
     * @param node 脚本节点
     * @param indent_level 缩进级别
     * @return 生成的JavaScript
     */
    std::string generateJavaScriptCode(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 获取缩进字符串
     * @param level 缩进级别
     * @return 缩进字符串
     */
    std::string getIndent(int level) const;

    /**
     * @brief 转义HTML字符
     * @param text 原始文本
     * @return 转义后的文本
     */
    std::string escapeHTML(const std::string& text) const;

    /**
     * @brief 转义CSS字符
     * @param text 原始文本
     * @return 转义后的文本
     */
    std::string escapeCSS(const std::string& text) const;

    /**
     * @brief 转义JavaScript字符
     * @param text 原始文本
     * @return 转义后的文本
     */
    std::string escapeJavaScript(const std::string& text) const;

    /**
     * @brief 处理模板变量
     * @param content 内容
     * @return 处理后的内容
     */
    std::string processTemplateVariables(const std::string& content);

    /**
     * @brief 处理自定义变量
     * @param content 内容
     * @return 处理后的内容
     */
    std::string processCustomVariables(const std::string& content);

    /**
     * @brief 处理属性条件表达式
     * @param content 内容
     * @return 处理后的内容
     */
    std::string processAttributeConditions(const std::string& content);

    /**
     * @brief 处理引用属性
     * @param content 内容
     * @return 处理后的内容
     */
    std::string processPropertyReferences(const std::string& content);

private:
    bool default_structure_;
    std::string indent_char_;
    bool minify_;
    
    // 生成的代码缓存
    std::string generated_css_;
    std::string generated_js_;
    
    // 模板和自定义变量映射
    std::unordered_map<std::string, std::string> template_variables_;
    std::unordered_map<std::string, std::string> custom_variables_;
    
    // 样式类名和ID映射
    std::unordered_map<std::string, std::string> class_names_;
    std::unordered_map<std::string, std::string> id_names_;
};

} // namespace CHTL