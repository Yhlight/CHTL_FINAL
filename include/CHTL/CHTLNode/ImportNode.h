#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>

namespace CHTL {

/**
 * @brief 导入类型枚举
 */
enum class ImportType {
    CHTL,           // @Chtl
    HTML,           // @Html
    STYLE,          // @Style
    JAVASCRIPT,     // @JavaScript
    CONFIG,         // @Config
    CMOD,           // @Cmod
    CJMOD,          // @CJmod
    CUSTOM          // 自定义类型
};

/**
 * @brief 导入模式枚举
 */
enum class ImportMode {
    EXACT,          // 精确导入
    TYPE,           // 类型导入
    WILDCARD        // 通配导入
};

/**
 * @brief 导入节点类
 * 
 * 表示CHTL中的导入节点
 */
class ImportNode : public BaseNode {
public:
    ImportNode(ImportType type, const std::string& path);
    ~ImportNode() = default;

    /**
     * @brief 获取导入类型
     * @return 导入类型
     */
    ImportType getImportType() const { return m_importType; }

    /**
     * @brief 设置导入类型
     * @param type 导入类型
     */
    void setImportType(ImportType type) { m_importType = type; }

    /**
     * @brief 获取导入路径
     * @return 导入路径
     */
    const std::string& getPath() const { return m_path; }

    /**
     * @brief 设置导入路径
     * @param path 导入路径
     */
    void setPath(const std::string& path) { m_path = path; }

    /**
     * @brief 获取导入名称
     * @return 导入名称
     */
    const std::string& getImportName() const { return m_importName; }

    /**
     * @brief 设置导入名称
     * @param name 导入名称
     */
    void setImportName(const std::string& name) { m_importName = name; }

    /**
     * @brief 获取别名
     * @return 别名
     */
    const std::string& getAlias() const { return m_alias; }

    /**
     * @brief 设置别名
     * @param alias 别名
     */
    void setAlias(const std::string& alias) { m_alias = alias; }

    /**
     * @brief 获取导入模式
     * @return 导入模式
     */
    ImportMode getImportMode() const { return m_importMode; }

    /**
     * @brief 设置导入模式
     * @param mode 导入模式
     */
    void setImportMode(ImportMode mode) { m_importMode = mode; }

    /**
     * @brief 添加目标名称
     * @param name 目标名称
     */
    void addTargetName(const std::string& name);

    /**
     * @brief 获取目标名称列表
     * @return 目标名称列表
     */
    const std::vector<std::string>& getTargetNames() const { return m_targetNames; }

    /**
     * @brief 检查是否为精确导入
     * @return 是否为精确导入
     */
    bool isExactImport() const { return m_importMode == ImportMode::EXACT; }

    /**
     * @brief 检查是否为类型导入
     * @return 是否为类型导入
     */
    bool isTypeImport() const { return m_importMode == ImportMode::TYPE; }

    /**
     * @brief 检查是否为通配导入
     * @return 是否为通配导入
     */
    bool isWildcardImport() const { return m_importMode == ImportMode::WILDCARD; }

    /**
     * @brief 检查是否为CHTL导入
     * @return 是否为CHTL导入
     */
    bool isCHTLImport() const { return m_importType == ImportType::CHTL; }

    /**
     * @brief 检查是否为模块导入
     * @return 是否为模块导入
     */
    bool isModuleImport() const { 
        return m_importType == ImportType::CMOD || m_importType == ImportType::CJMOD; 
    }

    /**
     * @brief 检查是否为官方模块导入
     * @return 是否为官方模块导入
     */
    bool isOfficialModuleImport() const;

    /**
     * @brief 获取解析后的文件路径
     * @return 文件路径
     */
    std::string getResolvedPath() const;

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
     * @brief 获取导入类型字符串
     * @return 类型字符串
     */
    static std::string importTypeToString(ImportType type);

    /**
     * @brief 从字符串解析导入类型
     * @param str 字符串
     * @return 导入类型
     */
    static ImportType parseImportType(const std::string& str);

    /**
     * @brief 获取导入模式字符串
     * @return 模式字符串
     */
    static std::string importModeToString(ImportMode mode);

    /**
     * @brief 检查路径是否为通配符
     * @param path 路径
     * @return 是否为通配符
     */
    static bool isWildcardPath(const std::string& path);

    /**
     * @brief 检查路径是否为官方模块
     * @param path 路径
     * @return 是否为官方模块
     */
    static bool isOfficialModulePath(const std::string& path);

private:
    ImportType m_importType;
    std::string m_path;
    std::string m_importName;
    std::string m_alias;
    ImportMode m_importMode;
    std::vector<std::string> m_targetNames;
};

} // namespace CHTL