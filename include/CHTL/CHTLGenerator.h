#pragma once

#include "CHTL/CHTLNode/BaseNode.h"
#include "CHTL/CHTLContext.h"
#include <string>
#include <memory>
#include <vector>

namespace CHTL {

/**
 * @brief CHTL代码生成器类
 * 
 * 负责将AST转换为HTML、CSS、JavaScript代码
 */
class CHTLGenerator {
public:
    /**
     * @brief 构造函数
     * @param context CHTL上下文
     */
    explicit CHTLGenerator(std::shared_ptr<CHTLContext> context);
    
    /**
     * @brief 析构函数
     */
    ~CHTLGenerator();
    
    /**
     * @brief 生成HTML代码
     * @param ast AST根节点
     * @return HTML代码
     */
    std::string generateHTML(std::shared_ptr<BaseNode> ast);
    
    /**
     * @brief 生成CSS代码
     * @param ast AST根节点
     * @return CSS代码
     */
    std::string generateCSS(std::shared_ptr<BaseNode> ast);
    
    /**
     * @brief 生成JavaScript代码
     * @param ast AST根节点
     * @return JavaScript代码
     */
    std::string generateJavaScript(std::shared_ptr<BaseNode> ast);
    
    /**
     * @brief 生成完整的HTML文档
     * @param ast AST根节点
     * @return 完整的HTML文档
     */
    std::string generateDocument(std::shared_ptr<BaseNode> ast);
    
    /**
     * @brief 设置是否生成默认HTML结构
     * @param generateDefault 是否生成默认结构
     */
    void setGenerateDefaultStructure(bool generateDefault);
    
    /**
     * @brief 设置是否生成CSS
     * @param generateCSS 是否生成CSS
     */
    void setGenerateCSS(bool generateCSS);
    
    /**
     * @brief 设置是否生成JavaScript
     * @param generateJS 是否生成JavaScript
     */
    void setGenerateJavaScript(bool generateJS);
    
    /**
     * @brief 设置调试模式
     * @param debug 是否启用调试模式
     */
    void setDebugMode(bool debug);
    
    /**
     * @brief 是否处于调试模式
     * @return 是否调试模式
     */
    bool isDebugMode() const;

private:
    /**
     * @brief 生成HTML元素
     * @param node 元素节点
     * @param indent 缩进级别
     * @return HTML代码
     */
    std::string generateElement(std::shared_ptr<BaseNode> node, int indent = 0);
    
    /**
     * @brief 生成文本节点
     * @param node 文本节点
     * @param indent 缩进级别
     * @return HTML代码
     */
    std::string generateText(std::shared_ptr<BaseNode> node, int indent = 0);
    
    /**
     * @brief 生成注释节点
     * @param node 注释节点
     * @param indent 缩进级别
     * @return HTML代码
     */
    std::string generateComment(std::shared_ptr<BaseNode> node, int indent = 0);
    
    /**
     * @brief 生成样式节点
     * @param node 样式节点
     * @param indent 缩进级别
     * @return CSS代码
     */
    std::string generateStyle(std::shared_ptr<BaseNode> node, int indent = 0);
    
    /**
     * @brief 生成脚本节点
     * @param node 脚本节点
     * @param indent 缩进级别
     * @return JavaScript代码
     */
    std::string generateScript(std::shared_ptr<BaseNode> node, int indent = 0);
    
    /**
     * @brief 生成模板节点
     * @param node 模板节点
     * @param indent 缩进级别
     * @return 生成的代码
     */
    std::string generateTemplate(std::shared_ptr<BaseNode> node, int indent = 0);
    
    /**
     * @brief 生成自定义节点
     * @param node 自定义节点
     * @param indent 缩进级别
     * @return 生成的代码
     */
    std::string generateCustom(std::shared_ptr<BaseNode> node, int indent = 0);
    
    /**
     * @brief 生成导入节点
     * @param node 导入节点
     * @param indent 缩进级别
     * @return 生成的代码
     */
    std::string generateImport(std::shared_ptr<BaseNode> node, int indent = 0);
    
    /**
     * @brief 生成命名空间节点
     * @param node 命名空间节点
     * @param indent 缩进级别
     * @return 生成的代码
     */
    std::string generateNamespace(std::shared_ptr<BaseNode> node, int indent = 0);
    
    /**
     * @brief 生成配置节点
     * @param node 配置节点
     * @param indent 缩进级别
     * @return 生成的代码
     */
    std::string generateConfiguration(std::shared_ptr<BaseNode> node, int indent = 0);
    
    /**
     * @brief 生成节点（通用方法）
     * @param node 节点
     * @param indent 缩进级别
     * @return 生成的代码
     */
    std::string generateNode(std::shared_ptr<BaseNode> node, int indent = 0);
    
    /**
     * @brief 生成属性字符串
     * @param node 节点
     * @return 属性字符串
     */
    std::string generateAttributes(std::shared_ptr<BaseNode> node);
    
    /**
     * @brief 生成缩进
     * @param level 缩进级别
     * @return 缩进字符串
     */
    std::string generateIndent(int level) const;
    
    /**
     * @brief 转义HTML特殊字符
     * @param text 原始文本
     * @return 转义后的文本
     */
    std::string escapeHtml(const std::string& text) const;
    
    /**
     * @brief 转义CSS特殊字符
     * @param text 原始文本
     * @return 转义后的文本
     */
    std::string escapeCSS(const std::string& text) const;
    
    /**
     * @brief 转义JavaScript特殊字符
     * @param text 原始文本
     * @return 转义后的文本
     */
    std::string escapeJavaScript(const std::string& text) const;
    
    /**
     * @brief 收集所有样式节点
     * @param node 根节点
     * @param styles 样式节点列表
     */
    void collectStyles(std::shared_ptr<BaseNode> node, std::vector<std::shared_ptr<BaseNode>>& styles);
    
    /**
     * @brief 收集所有脚本节点
     * @param node 根节点
     * @param scripts 脚本节点列表
     */
    void collectScripts(std::shared_ptr<BaseNode> node, std::vector<std::shared_ptr<BaseNode>>& scripts);
    
    /**
     * @brief 添加错误信息
     * @param message 错误信息
     */
    void addError(const std::string& message);
    
    /**
     * @brief 添加警告信息
     * @param message 警告信息
     */
    void addWarning(const std::string& message);

private:
    std::shared_ptr<CHTLContext> m_context;
    bool m_generateDefaultStructure;
    bool m_generateCSS;
    bool m_generateJavaScript;
    bool m_debugMode;
    
    // 生成的代码缓存
    std::string m_htmlCode;
    std::string m_cssCode;
    std::string m_javascriptCode;
};

} // namespace CHTL