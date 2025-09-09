#pragma once

#include "CHTLContext.h"
#include "CHTLNode/BaseNode.h"
#include <string>
#include <unordered_map>
#include <memory>

namespace CHTL {

/**
 * @brief CHTL管理器
 * 负责管理CHTL模块和组件
 */
class CHTLManage {
public:
    CHTLManage(std::shared_ptr<CHTLContext> context);
    ~CHTLManage();
    
    // 模块管理
    void addModule(const std::string& name, std::shared_ptr<BaseNode> module);
    std::shared_ptr<BaseNode> getModule(const std::string& name) const;
    bool hasModule(const std::string& name) const;
    void removeModule(const std::string& name);
    const std::unordered_map<std::string, std::shared_ptr<BaseNode>>& getModules() const;
    void clearModules();

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unordered_map<std::string, std::shared_ptr<BaseNode>> m_modules;
};

} // namespace CHTL