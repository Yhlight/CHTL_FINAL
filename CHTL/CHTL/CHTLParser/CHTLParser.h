#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLNode/BaseNode.h"
#include <memory>
#include <stack>

// 前向声明
namespace CHTL {
    class TemplateManager;
}

namespace CHTL {

/**
 * CHTL语法解析器
 */
class CHTLParser {
public:
    CHTLParser();
    ~CHTLParser();
    
    /**
     * 解析CHTL代码
     * @param tokens Token列表
     * @return AST根节点
     */
    std::shared_ptr<BaseNode> parse(const std::vector<Token>& tokens);
    
    /**
     * 解析CHTL代码
     * @param code 源代码
     * @return AST根节点
     */
    std::shared_ptr<BaseNode> parse(const std::string& code);

private:
    /**
     * 解析元素
     * @return 元素节点
     */
    std::shared_ptr<BaseNode> parseElement();
    
    /**
     * 解析文本节点
     * @return 文本节点
     */
    std::shared_ptr<BaseNode> parseText();
    
    /**
     * 解析注释
     * @return 注释节点
     */
    std::shared_ptr<BaseNode> parseComment();
    
    /**
     * 解析模板
     * @return 模板节点
     */
    std::shared_ptr<BaseNode> parseTemplate();
    
    /**
     * 解析自定义
     * @return 自定义节点
     */
    std::shared_ptr<BaseNode> parseCustom();
    
    /**
     * 解析样式块
     * @return 样式节点
     */
    std::shared_ptr<BaseNode> parseStyle();
    
    /**
     * 解析脚本块
     * @return 脚本节点
     */
    std::shared_ptr<BaseNode> parseScript();
    
    /**
     * 解析原始嵌入
     * @return 原始嵌入节点
     */
    std::shared_ptr<BaseNode> parseOrigin();
    
    /**
     * 解析导入
     * @return 导入节点
     */
    std::shared_ptr<BaseNode> parseImport();
    
    /**
     * 解析配置
     * @return 配置节点
     */
    std::shared_ptr<BaseNode> parseConfig();
    
    /**
     * 解析命名空间
     * @return 命名空间节点
     */
    std::shared_ptr<BaseNode> parseNamespace();
    
    /**
     * 解析属性
     * @param element 元素节点
     */
    void parseAttributes(std::shared_ptr<BaseNode> element);
    
    /**
     * 解析属性值
     * @return 属性值
     */
    std::string parseAttributeValue();
    
    /**
     * 解析样式属性
     * @param styleNode 样式节点
     */
    void parseStyleProperties(std::shared_ptr<BaseNode> styleNode);
    
    /**
     * 解析字面量
     * @return 字面量值
     */
    std::string parseLiteral();
    
    /**
     * 解析字符串
     * @return 字符串值
     */
    std::string parseString();
    
    /**
     * 解析表达式
     * @return 表达式值
     */
    std::string parseExpression();
    
    /**
     * 获取当前Token
     * @return 当前Token
     */
    const Token& current() const;
    
    /**
     * 获取下一个Token
     * @return 下一个Token
     */
    const Token& peek() const;
    
    /**
     * 前进到下一个Token
     */
    void advance();
    
    /**
     * 检查是否到达文件末尾
     * @return 是否到达末尾
     */
    bool isAtEnd() const;
    
    /**
     * 检查当前Token类型
     * @param type Token类型
     * @return 是否匹配
     */
    bool match(TokenType type) const;
    
    /**
     * 检查并消费Token
     * @param type Token类型
     * @return 是否匹配并消费
     */
    bool consume(TokenType type);
    
    /**
     * 跳过空白Token
     */
    void skipWhitespace();

private:
    std::vector<Token> m_tokens;
    size_t m_position;
    std::stack<std::shared_ptr<BaseNode>> m_nodeStack;
    std::shared_ptr<TemplateManager> m_templateManager;
};

} // namespace CHTL

#endif // CHTL_PARSER_H