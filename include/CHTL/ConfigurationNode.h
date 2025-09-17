#pragma once

#include "BaseNode.h"

namespace CHTL {

// 配置节点类
class ConfigurationNode : public BaseNode {
private:
    String configName_;
    StringMap settings_;
    
public:
    ConfigurationNode(const String& name, const SourceLocation& loc = {})
        : BaseNode(NodeType::Configuration, loc), configName_(name) {}
    
    // 配置名称
    const String& getConfigName() const { return configName_; }
    void setConfigName(const String& name) { configName_ = name; }
    
    // 设置管理
    const StringMap& getSettings() const { return settings_; }
    void setSetting(const String& name, const String& value);
    String getSetting(const String& name) const;
    bool hasSetting(const String& name) const;
    void removeSetting(const String& name);
    void clearSettings();
    
    // 实现基类方法
    String toString() const override;
    String toHTML() const override;
    bool isValid() const override;
    void accept(NodeVisitor& visitor) override;
    
    // 工厂方法
    static ConfigurationNodePtr create(const String& name, const SourceLocation& loc = {});
};

} // namespace CHTL