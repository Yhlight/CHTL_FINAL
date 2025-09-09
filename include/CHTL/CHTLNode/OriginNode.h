#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

/**
 * @brief 原始嵌入类型枚举
 */
enum class OriginType {
    HTML,           // @Html
    STYLE,          // @Style
    JAVASCRIPT,     // @JavaScript
    VUE,            // @Vue
    MARKDOWN,       // @Markdown
    TYPESCRIPT,     // @TypeScript
    PYTHON,         // @Python
    JAVA,           // @Java
    CPP,            // @Cpp
    RUST,           // @Rust
    CUSTOM          // 自定义类型
};

/**
 * @brief 原始嵌入节点类
 * 
 * 表示CHTL中的原始嵌入节点
 */
class OriginNode : public BaseNode {
public:
    OriginNode(OriginType type, const std::string& name = "");
    ~OriginNode() = default;

    /**
     * @brief 获取原始嵌入类型
     * @return 原始嵌入类型
     */
    OriginType getOriginType() const { return m_originType; }

    /**
     * @brief 设置原始嵌入类型
     * @param type 原始嵌入类型
     */
    void setOriginType(OriginType type) { m_originType = type; }

    /**
     * @brief 获取原始嵌入名称
     * @return 原始嵌入名称
     */
    const std::string& getOriginName() const { return m_originName; }

    /**
     * @brief 设置原始嵌入名称
     * @param name 原始嵌入名称
     */
    void setOriginName(const std::string& name) { m_originName = name; }

    /**
     * @brief 获取原始内容
     * @return 原始内容
     */
    const std::string& getContent() const { return m_content; }

    /**
     * @brief 设置原始内容
     * @param content 原始内容
     */
    void setContent(const std::string& content) { m_content = content; }

    /**
     * @brief 追加原始内容
     * @param content 要追加的内容
     */
    void appendContent(const std::string& content);

    /**
     * @brief 检查是否为命名原始嵌入
     * @return 是否为命名原始嵌入
     */
    bool isNamed() const { return !m_originName.empty(); }

    /**
     * @brief 检查是否为HTML原始嵌入
     * @return 是否为HTML原始嵌入
     */
    bool isHTML() const { return m_originType == OriginType::HTML; }

    /**
     * @brief 检查是否为CSS原始嵌入
     * @return 是否为CSS原始嵌入
     */
    bool isCSS() const { return m_originType == OriginType::STYLE; }

    /**
     * @brief 检查是否为JavaScript原始嵌入
     * @return 是否为JavaScript原始嵌入
     */
    bool isJavaScript() const { return m_originType == OriginType::JAVASCRIPT; }

    /**
     * @brief 检查是否为自定义类型原始嵌入
     * @return 是否为自定义类型原始嵌入
     */
    bool isCustomType() const { return m_originType == OriginType::CUSTOM; }

    /**
     * @brief 获取原始内容的HTML表示
     * @return HTML字符串
     */
    std::string toHTML() const;

    /**
     * @brief 获取原始内容的CSS表示
     * @return CSS字符串
     */
    std::string toCSS() const;

    /**
     * @brief 获取原始内容的JS表示
     * @return JS字符串
     */
    std::string toJS() const;

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
     * @brief 获取原始嵌入类型字符串
     * @return 类型字符串
     */
    static std::string originTypeToString(OriginType type);

    /**
     * @brief 从字符串解析原始嵌入类型
     * @param str 字符串
     * @return 原始嵌入类型
     */
    static OriginType parseOriginType(const std::string& str);

    /**
     * @brief 检查是否为有效的原始嵌入类型
     * @param str 类型字符串
     * @return 是否有效
     */
    static bool isValidOriginType(const std::string& str);

private:
    OriginType m_originType;
    std::string m_originName;
    std::string m_content;
};

} // namespace CHTL