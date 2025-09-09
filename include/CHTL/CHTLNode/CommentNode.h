#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

/**
 * @brief 注释节点类
 * 
 * 表示CHTL中的注释节点
 */
class CommentNode : public BaseNode {
public:
    CommentNode(const std::string& content, bool isGeneratorComment = false);
    ~CommentNode() = default;

    /**
     * @brief 获取注释内容
     * @return 注释内容
     */
    const std::string& getContent() const { return m_content; }

    /**
     * @brief 设置注释内容
     * @param content 注释内容
     */
    void setContent(const std::string& content) { m_content = content; }

    /**
     * @brief 检查是否为生成器注释
     * @return 是否为生成器注释
     */
    bool isGeneratorComment() const { return m_isGeneratorComment; }

    /**
     * @brief 设置是否为生成器注释
     * @param isGeneratorComment 是否为生成器注释
     */
    void setGeneratorComment(bool isGeneratorComment) { m_isGeneratorComment = isGeneratorComment; }

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

private:
    std::string m_content;
    bool m_isGeneratorComment;
};

} // namespace CHTL