#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>
#include "CHTLNode/BaseNode.hpp"

namespace CHTL {

/**
 * @brief 原始嵌入管理器
 * 
 * 管理原始HTML、CSS、JavaScript代码的嵌入
 */
class OriginManager {
public:
    enum class OriginType {
        HTML,           // HTML代码
        CSS,            // CSS代码
        JAVASCRIPT,     // JavaScript代码
        CUSTOM          // 自定义类型
    };
    
    struct OriginDefinition {
        OriginType type;
        std::string name;
        std::string content;
        std::map<std::string, std::string> attributes;
    };
    
    // 单例模式
    static OriginManager& getInstance();
    
    // 添加原始嵌入定义
    void addOrigin(const std::string& name, OriginType type, 
                  const std::string& content);
    
    // 添加带属性的原始嵌入定义
    void addOrigin(const std::string& name, OriginType type, 
                  const std::string& content,
                  const std::map<std::string, std::string>& attributes);
    
    // 获取原始嵌入定义
    std::shared_ptr<OriginDefinition> getOrigin(const std::string& name) const;
    
    // 检查原始嵌入是否存在
    bool hasOrigin(const std::string& name) const;
    
    // 获取原始嵌入内容
    std::string getOriginContent(const std::string& name) const;
    
    // 获取原始嵌入属性
    std::map<std::string, std::string> getOriginAttributes(const std::string& name) const;
    
    // 获取所有原始嵌入名称
    std::vector<std::string> getAllOriginNames() const;
    
    // 获取指定类型的所有原始嵌入名称
    std::vector<std::string> getOriginNamesByType(OriginType type) const;
    
    // 清除所有原始嵌入
    void clear();
    
    // 移除原始嵌入
    void removeOrigin(const std::string& name);
    
    // 注册自定义类型
    void registerCustomType(const std::string& typeName, OriginType type);
    
    // 获取自定义类型
    OriginType getCustomType(const std::string& typeName) const;
    
    // 检查是否为自定义类型
    bool isCustomType(const std::string& typeName) const;
    
private:
    OriginManager() = default;
    ~OriginManager() = default;
    OriginManager(const OriginManager&) = delete;
    OriginManager& operator=(const OriginManager&) = delete;
    
    std::map<std::string, std::shared_ptr<OriginDefinition>> origins_;
    std::map<std::string, OriginType> customTypes_;
    
    // 验证原始嵌入内容
    bool validateOriginContent(OriginType type, const std::string& content) const;
    
    // 处理原始嵌入内容
    std::string processOriginContent(OriginType type, const std::string& content) const;
};

} // namespace CHTL