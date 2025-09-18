#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/BaseNode.h"
#include <string>
#include <memory>

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
     * @param root AST根节点
     * @return HTML代码
     */
    std::string generateHTML(std::shared_ptr<BaseNode> root);
    
    /**
     * 生成CSS代码
     * @param root AST根节点
     * @return CSS代码
     */
    std::string generateCSS(std::shared_ptr<BaseNode> root);
    
    /**
     * 生成JavaScript代码
     * @param root AST根节点
     * @return JavaScript代码
     */
    std::string generateJavaScript(std::shared_ptr<BaseNode> root);
    
    /**
     * 设置是否生成默认HTML结构
     * @param enable 是否启用
     */
    void setDefaultStructure(bool enable);
    
    /**
     * 设置是否内联CSS
     * @param enable 是否启用
     */
    void setInlineCSS(bool enable);
    
    /**
     * 设置是否内联JavaScript
     * @param enable 是否启用
     */
    void setInlineJavaScript(bool enable);

private:
    /**
     * 生成元素HTML
     * @param element 元素节点
     * @return HTML代码
     */
    std::string generateElementHTML(std::shared_ptr<BaseNode> element);
    
    /**
     * 生成文本HTML
     * @param text 文本节点
     * @return HTML代码
     */
    std::string generateTextHTML(std::shared_ptr<BaseNode> text);
    
    /**
     * 生成注释HTML
     * @param comment 注释节点
     * @return HTML代码
     */
    std::string generateCommentHTML(std::shared_ptr<BaseNode> comment);
    
    /**
     * 生成样式CSS
     * @param style 样式节点
     * @return CSS代码
     */
    std::string generateStyleCSS(std::shared_ptr<BaseNode> style);
    
    /**
     * 生成脚本JavaScript
     * @param script 脚本节点
     * @return JavaScript代码
     */
    std::string generateScriptJavaScript(std::shared_ptr<BaseNode> script);
    
    /**
     * 转义HTML特殊字符
     * @param text 文本
     * @return 转义后的文本
     */
    std::string escapeHTML(const std::string& text);
    
    /**
     * 生成默认HTML结构
     * @param content 内容
     * @return 完整HTML
     */
    std::string generateDefaultHTMLStructure(const std::string& content);

private:
    bool m_defaultStructure;
    bool m_inlineCSS;
    bool m_inlineJavaScript;
    std::string m_globalCSS;
    std::string m_globalJavaScript;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H