#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

/**
 * @brief 文本节点类
 * 
 * 表示文本内容节点
 */
class TextNode : public BaseNode {
public:
    /**
     * @brief 构造函数
     * @param text 文本内容
     * @param line 行号
     * @param column 列号
     */
    TextNode(const std::string& text, size_t line = 0, size_t column = 0);
    
    /**
     * @brief 析构函数
     */
    ~TextNode() override = default;
    
    /**
     * @brief 接受访问者
     * @param visitor 访问者对象
     */
    void accept(Visitor& visitor) override;
    
    /**
     * @brief 生成代码
     * @return 生成的代码
     */
    std::string generate() override;
    
    /**
     * @brief 获取文本内容
     * @return 文本内容
     */
    const std::string& getText() const { return text_; }
    
    /**
     * @brief 设置文本内容
     * @param text 文本内容
     */
    void setText(const std::string& text) { text_ = text; }
    
    /**
     * @brief 检查是否为空文本
     * @return 是否为空文本
     */
    bool isEmpty() const;
    
    /**
     * @brief 检查是否为空白文本
     * @return 是否为空白文本
     */
    bool isWhitespace() const;
    
    /**
     * @brief 转换为字符串
     * @return 字符串表示
     */
    std::string toString() const override;

private:
    std::string text_;         ///< 文本内容
    
    /**
     * @brief 转义 HTML 特殊字符
     * @param text 原始文本
     * @return 转义后的文本
     */
    std::string escapeHtml(const std::string& text) const;
    
    /**
     * @brief 检查字符是否为空白字符
     * @param c 字符
     * @return 是否为空白字符
     */
    bool isWhitespaceChar(char c) const;
};

} // namespace CHTL