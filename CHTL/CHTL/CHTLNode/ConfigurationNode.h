#pragma once
#include "BaseNode.h"
#include <string>
#include <vector>
#include <map>

namespace CHTL {

enum class ConfigurationType {
    Keyword,        // 关键字自定义
    OriginType,     // 原始类型自定义
    Named,          // 命名配置组
    Import          // 导入配置组
};

class ConfigurationNode : public BaseNode {
public:
    ConfigurationType configType;
    std::string name; // 配置组名称
    std::map<std::string, std::string> options; // 配置选项
    std::vector<std::string> keywords; // 关键字列表（用于关键字自定义）
    std::vector<std::string> originTypes; // 原始类型列表（用于原始类型自定义）
    std::string importPath; // 导入路径（用于导入配置组）

    ConfigurationNode(ConfigurationType type, const std::string& name);
    std::string toString(int indent = 0) const override;
};

} // namespace CHTL