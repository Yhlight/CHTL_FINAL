#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

/**
 * @brief 脚本节点类
 * 
 * 表示CHTL中的脚本节点
 */
class ScriptNode : public BaseNode {
public:
    ScriptNode();
    ~ScriptNode() = default;

    /**
     * @brief 获取脚本内容
     * @return 脚本内容
     */
    const std::string& getContent() const { return m_content; }

    /**
     * @brief 设置脚本内容
     * @param content 脚本内容
     */
    void setContent(const std::string& content) { m_content = content; }

    /**
     * @brief 追加脚本内容
     * @param content 要追加的脚本内容
     */
    void appendContent(const std::string& content);

    /**
     * @brief 检查脚本是否为空
     * @return 是否为空
     */
    bool isEmpty() const;

    /**
     * @brief 检查是否为局部脚本
     * @return 是否为局部脚本
     */
    bool isLocal() const { return m_isLocal; }

    /**
     * @brief 设置是否为局部脚本
     * @param isLocal 是否为局部脚本
     */
    void setLocal(bool isLocal) { m_isLocal = isLocal; }

    /**
     * @brief 检查是否为全局脚本
     * @return 是否为全局脚本
     */
    bool isGlobal() const { return !m_isLocal; }

    /**
     * @brief 设置是否为全局脚本
     * @param isGlobal 是否为全局脚本
     */
    void setGlobal(bool isGlobal) { m_isLocal = !isGlobal; }

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
     * @brief 获取JS表示
     * @return JS字符串
     */
    std::string toJS() const;

private:
    std::string m_content;
    bool m_isLocal;
};

} // namespace CHTL