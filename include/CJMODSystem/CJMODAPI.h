#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * @brief CJMOD API
 * 提供CJMOD模块的API接口
 */
class CJMODAPI {
public:
    CJMODAPI();
    ~CJMODAPI();
    
    // API操作
    bool initialize();
    std::string processCode(const std::string& code);

private:
    bool m_initialized;
    std::vector<std::shared_ptr<class SyntaxExtension>> m_syntaxExtensions;
    std::vector<std::shared_ptr<class VirtualObject>> m_virtualObjects;
    
    // 辅助方法
    void registerDefaultExtensions();
};

} // namespace CHTL