#pragma once

#include "CHTL/CHTLNode.h"
#include "CHTLJSLexer.h"
#include <string>
#include <memory>
#include <unordered_map>

namespace CHTL {

/**
 * @brief CHTL JS代码生成器
 * 将CHTL JS AST转换为标准JavaScript代码
 */
class CHTLJSGenerator {
public:
    CHTLJSGenerator();
    ~CHTLJSGenerator() = default;

    /**
     * @brief 生成JavaScript代码
     * @param ast AST根节点
     * @return 生成的JavaScript代码
     */
    std::string generateJavaScript(std::shared_ptr<CHTLNode> ast);

    /**
     * @brief 生成JavaScript代码
     * @param source_code 源代码
     * @return 生成的JavaScript代码
     */
    std::string generateJavaScript(const std::string& source_code);

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

    /**
     * @brief 设置是否生成严格模式代码
     * @param strict 是否严格模式
     */
    void setStrictMode(bool strict);

private:
    /**
     * @brief 生成语句JavaScript
     * @param node 语句节点
     * @param indent_level 缩进级别
     * @return 生成的JavaScript
     */
    std::string generateStatement(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成表达式JavaScript
     * @param node 表达式节点
     * @param indent_level 缩进级别
     * @return 生成的JavaScript
     */
    std::string generateExpression(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成函数调用JavaScript
     * @param node 函数调用节点
     * @param indent_level 缩进级别
     * @return 生成的JavaScript
     */
    std::string generateFunctionCall(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成成员访问JavaScript
     * @param node 成员访问节点
     * @param indent_level 缩进级别
     * @return 生成的JavaScript
     */
    std::string generateMemberAccess(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成数组访问JavaScript
     * @param node 数组访问节点
     * @param indent_level 缩进级别
     * @return 生成的JavaScript
     */
    std::string generateArrayAccess(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成对象字面量JavaScript
     * @param node 对象字面量节点
     * @param indent_level 缩进级别
     * @return 生成的JavaScript
     */
    std::string generateObjectLiteral(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成数组字面量JavaScript
     * @param node 数组字面量节点
     * @param indent_level 缩进级别
     * @return 生成的JavaScript
     */
    std::string generateArrayLiteral(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成函数声明JavaScript
     * @param node 函数声明节点
     * @param indent_level 缩进级别
     * @return 生成的JavaScript
     */
    std::string generateFunctionDeclaration(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成变量声明JavaScript
     * @param node 变量声明节点
     * @param indent_level 缩进级别
     * @return 生成的JavaScript
     */
    std::string generateVariableDeclaration(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成if语句JavaScript
     * @param node if语句节点
     * @param indent_level 缩进级别
     * @return 生成的JavaScript
     */
    std::string generateIfStatement(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成for循环JavaScript
     * @param node for循环节点
     * @param indent_level 缩进级别
     * @return 生成的JavaScript
     */
    std::string generateForStatement(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成while循环JavaScript
     * @param node while循环节点
     * @param indent_level 缩进级别
     * @return 生成的JavaScript
     */
    std::string generateWhileStatement(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成CHTL JS函数JavaScript
     * @param node CHTL JS函数节点
     * @param indent_level 缩进级别
     * @return 生成的JavaScript
     */
    std::string generateCHTLJSFunction(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成listen函数JavaScript
     * @param node listen函数节点
     * @param indent_level 缩进级别
     * @return 生成的JavaScript
     */
    std::string generateListenFunction(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成animate函数JavaScript
     * @param node animate函数节点
     * @param indent_level 缩进级别
     * @return 生成的JavaScript
     */
    std::string generateAnimateFunction(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成delegate函数JavaScript
     * @param node delegate函数节点
     * @param indent_level 缩进级别
     * @return 生成的JavaScript
     */
    std::string generateDelegateFunction(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成router函数JavaScript
     * @param node router函数节点
     * @param indent_level 缩进级别
     * @return 生成的JavaScript
     */
    std::string generateRouterFunction(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成fileloader函数JavaScript
     * @param node fileloader函数节点
     * @param indent_level 缩进级别
     * @return 生成的JavaScript
     */
    std::string generateFileloaderFunction(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成vir声明JavaScript
     * @param node vir声明节点
     * @param indent_level 缩进级别
     * @return 生成的JavaScript
     */
    std::string generateVirDeclaration(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成iNeverAway函数JavaScript
     * @param node iNeverAway函数节点
     * @param indent_level 缩进级别
     * @return 生成的JavaScript
     */
    std::string generateINeverAwayFunction(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成util表达式JavaScript
     * @param node util表达式节点
     * @param indent_level 缩进级别
     * @return 生成的JavaScript
     */
    std::string generateUtilExpression(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 生成增强选择器JavaScript
     * @param node 增强选择器节点
     * @param indent_level 缩进级别
     * @return 生成的JavaScript
     */
    std::string generateEnhancedSelector(std::shared_ptr<CHTLNode> node, int indent_level = 0);

    /**
     * @brief 处理增强选择器
     * @param selector 选择器字符串
     * @return 处理后的选择器
     */
    std::string processEnhancedSelector(const std::string& selector);

    /**
     * @brief 处理虚对象
     * @param node 虚对象节点
     * @return 处理后的JavaScript
     */
    std::string processVirtualObject(std::shared_ptr<CHTLNode> node);

    /**
     * @brief 处理响应式值
     * @param content 内容
     * @return 处理后的内容
     */
    std::string processReactiveValues(const std::string& content);

    /**
     * @brief 获取缩进字符串
     * @param level 缩进级别
     * @return 缩进字符串
     */
    std::string getIndent(int level) const;

    /**
     * @brief 转义JavaScript字符
     * @param text 原始文本
     * @return 转义后的文本
     */
    std::string escapeJavaScript(const std::string& text) const;

    /**
     * @brief 生成唯一标识符
     * @param prefix 前缀
     * @return 唯一标识符
     */
    std::string generateUniqueId(const std::string& prefix = "chtl_");

private:
    std::string indent_char_;
    bool minify_;
    bool strict_mode_;
    
    // 虚对象映射
    std::unordered_map<std::string, std::string> virtual_objects_;
    
    // 响应式值映射
    std::unordered_map<std::string, std::string> reactive_values_;
    
    // 唯一标识符计数器
    size_t unique_id_counter_;
};

} // namespace CHTL