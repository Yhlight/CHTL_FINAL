#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

/**
 * @brief 文本节点类
 * 
 * 表示CHTL中的文本内容节点
 */
class TextNode : public BaseNode {
public:
    TextNode(const std::string& content);
    ~TextNode() = default;

    /**
     * @brief 获取文本内容
     * @return 文本内容
     */
    const std::string& getContent() const { return m_content; }

    /**
     * @brief 设置文本内容
     * @param content 文本内容
     */
    void setContent(const std::string& content) { m_content = content; }

    /**
     * @brief 追加文本内容
     * @param content 要追加的文本内容
     */
    void appendContent(const std::string& content);

    /**
     * @brief 检查文本是否为空
     * @return 是否为空
     */
    bool isEmpty() const;

    /**
     * @brief 检查文本是否只包含空白字符
     * @return 是否只包含空白字符
     */
    bool isWhitespace() const;

    /**
     * @brief 去除首尾空白字符
     */
    void trim();

    /**
     * @brief 获取去除空白字符后的内容
     * @return 去除空白字符后的内容
     */
    std::string getTrimmedContent() const;

    /**
     * @brief 检查是否为无修饰字面量
     * @return 是否为无修饰字面量
     */
    bool isLiteral() const { return m_isLiteral; }

    /**
     * @brief 设置是否为无修饰字面量
     * @param isLiteral 是否为无修饰字面量
     */
    void setLiteral(bool isLiteral) { m_isLiteral = isLiteral; }

    /**
     * @brief 检查是否需要引号
     * @return 是否需要引号
     */
    bool needsQuotes() const;

    /**
     * @brief 获取带引号的内容
     * @return 带引号的内容
     */
    std::string getQuotedContent() const;

    /**
     * @brief 转义HTML特殊字符
     * @return 转义后的内容
     */
    std::string getEscapedContent() const;

    /**
     * @brief 克隆节点
     * @return 克隆的节点
     */
    std::unique_ptr<BaseNode> clone() const override;

    /**
     * @brief 接受访问者
     * @param visitor 访问者
     */
    void accept(NodeVisitor* visitor) override;

    /**
     * @brief 获取节点字符串表示
     * @return 字符串表示
     */
    std::string toString() const override;

    /**
     * @brief 获取HTML表示
     * @return HTML字符串
     */
    std::string toHTML() const;

    /**
     * @brief 检查字符是否为空白字符
     * @param c 字符
     * @return 是否为空白字符
     */
    static bool isWhitespaceChar(char c);

    /**
     * @brief 检查字符是否为HTML特殊字符
     * @param c 字符
     * @return 是否为HTML特殊字符
     */
    static bool isHTMLSpecialChar(char c);

    /**
     * @brief 转义HTML特殊字符
     * @param text 原始文本
     * @return 转义后的文本
     */
    static std::string escapeHTML(const std::string& text);

    /**
     * @brief 转义引号
     * @param text 原始文本
     * @param quote 引号字符
     * @return 转义后的文本
     */
    static std::string escapeQuotes(const std::string& text, char quote = '"');

private:
    std::string m_content;
    bool m_isLiteral;
};

} // namespace CHTL