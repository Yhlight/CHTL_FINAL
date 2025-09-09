#pragma once

#include "BaseNode.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace CHTL {

/**
 * @brief 自定义类型枚举
 */
enum class CustomType {
    STYLE,      // @Style 自定义样式组
    ELEMENT,    // @Element 自定义元素
    VAR         // @Var 自定义变量组
};

/**
 * @brief 特例化操作类型
 */
enum class SpecializationType {
    DELETE,     // 删除操作
    INSERT,     // 插入操作
    MODIFY,     // 修改操作
    REPLACE     // 替换操作
};

/**
 * @brief 特例化操作
 */
struct SpecializationOperation {
    SpecializationType type;
    std::string target;
    std::string value;
    size_t index; // 用于索引访问
    
    SpecializationOperation(SpecializationType t, const std::string& target, const std::string& value = "", size_t idx = 0)
        : type(t), target(target), value(value), index(idx) {}
};

/**
 * @brief 自定义节点类
 * 
 * 表示CHTL中的自定义节点
 */
class CustomNode : public BaseNode {
public:
    CustomNode(CustomType type, const std::string& name);
    ~CustomNode() = default;

    /**
     * @brief 获取自定义类型
     * @return 自定义类型
     */
    CustomType getCustomType() const { return m_customType; }

    /**
     * @brief 设置自定义类型
     * @param type 自定义类型
     */
    void setCustomType(CustomType type) { m_customType = type; }

    /**
     * @brief 获取自定义名称
     * @return 自定义名称
     */
    const std::string& getCustomName() const { return m_customName; }

    /**
     * @brief 设置自定义名称
     * @param name 自定义名称
     */
    void setCustomName(const std::string& name) { m_customName = name; }

    /**
     * @brief 添加特例化操作
     * @param operation 特例化操作
     */
    void addSpecialization(const SpecializationOperation& operation);

    /**
     * @brief 获取特例化操作列表
     * @return 特例化操作列表
     */
    const std::vector<SpecializationOperation>& getSpecializations() const { return m_specializations; }

    /**
     * @brief 添加无值属性
     * @param name 属性名
     */
    void addValuelessProperty(const std::string& name);

    /**
     * @brief 获取无值属性列表
     * @return 无值属性列表
     */
    const std::vector<std::string>& getValuelessProperties() const { return m_valuelessProperties; }

    /**
     * @brief 检查是否为无值属性
     * @param name 属性名
     * @return 是否为无值属性
     */
    bool isValuelessProperty(const std::string& name) const;

    /**
     * @brief 添加属性
     * @param name 属性名
     * @param value 属性值
     */
    void addProperty(const std::string& name, const std::string& value);

    /**
     * @brief 获取属性值
     * @param name 属性名
     * @return 属性值
     */
    std::string getProperty(const std::string& name) const;

    /**
     * @brief 检查是否有属性
     * @param name 属性名
     * @return 是否有该属性
     */
    bool hasProperty(const std::string& name) const;

    /**
     * @brief 移除属性
     * @param name 属性名
     * @return 是否成功移除
     */
    bool removeProperty(const std::string& name);

    /**
     * @brief 获取所有属性
     * @return 属性映射
     */
    const std::unordered_map<std::string, std::string>& getProperties() const { return m_properties; }

    /**
     * @brief 检查是否为样式组自定义
     * @return 是否为样式组自定义
     */
    bool isStyleCustom() const { return m_customType == CustomType::STYLE; }

    /**
     * @brief 检查是否为元素自定义
     * @return 是否为元素自定义
     */
    bool isElementCustom() const { return m_customType == CustomType::ELEMENT; }

    /**
     * @brief 检查是否为变量组自定义
     * @return 是否为变量组自定义
     */
    bool isVarCustom() const { return m_customType == CustomType::VAR; }

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
     * @brief 获取自定义类型字符串
     * @return 类型字符串
     */
    static std::string customTypeToString(CustomType type);

    /**
     * @brief 从字符串解析自定义类型
     * @param str 字符串
     * @return 自定义类型
     */
    static CustomType parseCustomType(const std::string& str);

    /**
     * @brief 获取特例化类型字符串
     * @return 类型字符串
     */
    static std::string specializationTypeToString(SpecializationType type);

private:
    CustomType m_customType;
    std::string m_customName;
    std::vector<SpecializationOperation> m_specializations;
    std::vector<std::string> m_valuelessProperties;
    std::unordered_map<std::string, std::string> m_properties;
};

} // namespace CHTL