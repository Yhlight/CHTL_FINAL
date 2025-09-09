#pragma once

#include <string>

namespace CHTL {

/**
 * @brief CHTL JS生成器
 * 负责将CHTL JS代码转换为JavaScript
 */
class CHTLJSGenerator {
public:
    CHTLJSGenerator();
    ~CHTLJSGenerator();
    
    // 代码生成
    std::string generateJavaScript(const std::string& chtljsCode);

private:
    // 生成方法
    std::string generateFileLoader(const std::string& path);
    std::string generateScriptBlock(const std::string& content);
    std::string generateListener(const std::string& selector, const std::string& event, const std::string& handler);
    std::string generateDelegate(const std::string& parentSelector, const std::string& targetSelector, 
                                const std::string& event, const std::string& handler);
    std::string generateAnimation(const std::string& name, const std::string& keyframes);
    std::string generateVirtualObject(const std::string& name, const std::string& definition);
    std::string generateRouter(const std::string& routes);
    std::string generateSelector(const std::string& selector);
    std::string generateArrowFunction(const std::string& params, const std::string& body);
};

} // namespace CHTL