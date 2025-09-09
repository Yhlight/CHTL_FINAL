#pragma once

#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include "CHTL/CHTLContext.h"
#include <memory>

namespace CHTL {

/**
 * @brief CHTL 语法分析器
 * 
 * 负责将 Token 序列转换为抽象语法树 (AST)
 */
class CHTLParser {
public:
    /**
     * @brief 构造函数
     * @param context 编译上下文
     */
    explicit CHTLParser(CHTLContext& context);
    
    /**
     * @brief 析构函数
     */
    ~CHTLParser();
    
    /**
     * @brief 解析源代码
     * @param source 源代码
     * @return AST 根节点
     */
    std::shared_ptr<BaseNode> parse(const std::string& source);
    
    /**
     * @brief 解析 Token 列表
     * @param tokens Token 列表
     * @return AST 根节点
     */
    std::shared_ptr<BaseNode> parse(const TokenList& tokens);

private:
    /**
     * @brief 解析程序
     * @return 程序节点
     */
    std::shared_ptr<BaseNode> parseProgram();
    
    /**
     * @brief 解析元素
     * @return 元素节点
     */
    std::shared_ptr<ElementNode> parseElement();
    
    /**
     * @brief 解析文本节点
     * @return 文本节点
     */
    std::shared_ptr<TextNode> parseText();
    
    /**
     * @brief 解析属性
     * @param element 元素节点
     */
    void parseAttributes(std::shared_ptr<ElementNode> element);
    
    /**
     * @brief 解析局部样式块
     * @return 样式节点
     */
    std::shared_ptr<BaseNode> parseStyleBlock();
    
    /**
     * @brief 解析模板
     * @return 模板节点
     */
    std::shared_ptr<BaseNode> parseTemplate();
    
    /**
     * @brief 解析自定义
     * @return 自定义节点
     */
    std::shared_ptr<BaseNode> parseCustom();
    
    /**
     * @brief 解析原始嵌入
     * @return 原始嵌入节点
     */
    std::shared_ptr<BaseNode> parseOrigin();
    
    /**
     * @brief 解析导入
     * @return 导入节点
     */
    std::shared_ptr<BaseNode> parseImport();
    
    /**
     * @brief 解析配置
     * @return 配置节点
     */
    std::shared_ptr<BaseNode> parseConfiguration();
    
    /**
     * @brief 解析命名空间
     * @return 命名空间节点
     */
    std::shared_ptr<BaseNode> parseNamespace();
    
    /**
     * @brief 解析注释
     * @return 注释节点
     */
    std::shared_ptr<BaseNode> parseComment();
    
    /**
     * @brief 获取当前 Token
     * @return 当前 Token
     */
    Token currentToken() const;
    
    /**
     * @brief 获取下一个 Token
     * @return 下一个 Token
     */
    Token nextToken();
    
    /**
     * @brief 查看下一个 Token（不移动位置）
     * @return 下一个 Token
     */
    Token peekToken();
    
    /**
     * @brief 回退一个 Token
     */
    void ungetToken();
    
    /**
     * @brief 检查当前 Token 是否为指定类型
     * @param type Token 类型
     * @return 是否为指定类型
     */
    bool isCurrentToken(TokenType type) const;
    
    /**
     * @brief 检查当前 Token 是否为指定值
     * @param value Token 值
     * @return 是否为指定值
     */
    bool isCurrentToken(const std::string& value) const;
    
    /**
     * @brief 检查当前 Token 是否为指定类型和值
     * @param type Token 类型
     * @param value Token 值
     * @return 是否为指定类型和值
     */
    bool isCurrentToken(TokenType type, const std::string& value) const;
    
    /**
     * @brief 期望指定类型的 Token
     * @param type Token 类型
     * @return 当前 Token
     */
    Token expectToken(TokenType type);
    
    /**
     * @brief 期望指定值的 Token
     * @param value Token 值
     * @return 当前 Token
     */
    Token expectToken(const std::string& value);
    
    /**
     * @brief 期望指定类型和值的 Token
     * @param type Token 类型
     * @param value Token 值
     * @return 当前 Token
     */
    Token expectToken(TokenType type, const std::string& value);
    
    /**
     * @brief 跳过空白 Token
     */
    void skipWhitespace();
    
    /**
     * @brief 报告错误
     * @param message 错误信息
     */
    void reportError(const std::string& message);
    
    /**
     * @brief 报告警告
     * @param message 警告信息
     */
    void reportWarning(const std::string& message);

private:
    CHTLContext& context_;     ///< 编译上下文
    TokenList tokens_;         ///< Token 列表
    size_t currentIndex_;      ///< 当前 Token 索引
    Lexer lexer_;              ///< 词法分析器
};

} // namespace CHTL