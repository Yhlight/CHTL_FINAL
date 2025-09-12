#pragma once

#include "CHTL/CHTLNode/BaseNode.h"
#include <string>

namespace CHTL {

/**
 * @brief 文本节点类
 * 
 * 表示文本内容，可以是纯文本或包含CHTL语法的文本
 */
class TextNode : public BaseNode {
public:
    /**
     * @brief 构造函数
     * @param text 文本内容
     */
    explicit TextNode(const std::string& text = "");
    
    /**
     * @brief 析构函数
     */
    ~TextNode() override;
    
    /**
     * @brief 设置文本内容
     * @param text 文本内容
     */
    void setText(const std::string& text);
    
    /**
     * @brief 获取文本内容
     * @return 文本内容
     */
    const std::string& getText() const;
    
    /**
     * @brief 追加文本内容
     * @param text 要追加的文本
     */
    void appendText(const std::string& text);
    
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
     * @brief 去除前后空白字符
     */
    void trim();
    
    /**
     * @brief 设置是否为原始文本（不进行转义）
     * @param raw 是否为原始文本
     */
    void setRaw(bool raw);
    
    /**
     * @brief 是否为原始文本
     * @return 是否为原始文本
     */
    bool isRaw() const;
    
    /**
     * @brief 设置是否为生成器注释
     * @param generatorComment 是否为生成器注释
     */
    void setGeneratorComment(bool generatorComment);
    
    /**
     * @brief 是否为生成器注释
     * @return 是否为生成器注释
     */
    bool isGeneratorComment() const;
    
    /**
     * @brief 克隆节点
     * @return 克隆的节点
     */
    std::shared_ptr<BaseNode> clone() const override;
    
    /**
     * @brief 转换为字符串表示
     * @return 字符串表示
     */
    std::string toString() const override;
    
    /**
     * @brief 接受访问者
     * @param visitor 访问者
     */
    void accept(NodeVisitor& visitor) override;

private:
    bool m_raw;
    bool m_generatorComment;
    
    /**
     * @brief 转义HTML特殊字符
     * @param text 原始文本
     * @return 转义后的文本
     */
    static std::string escapeHtml(const std::string& text);
    
    /**
     * @brief 反转义HTML特殊字符
     * @param text 转义后的文本
     * @return 原始文本
     */
    static std::string unescapeHtml(const std::string& text);
};

} // namespace CHTL