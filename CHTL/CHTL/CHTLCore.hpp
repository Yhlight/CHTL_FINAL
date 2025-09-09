#pragma once

#include <CHTL/CHTLScanner/UnifiedScanner.hpp>
#include <CHTL/CHTLModule/ModuleManager.hpp>
#include <string>
#include <memory>
#include <vector>

namespace CHTL {

/**
 * @brief CHTL 核心类
 * 
 * 根据 CHTL.md 文档实现
 * 提供完整的 CHTL 编译功能
 */
class CHTLCore {
public:
    CHTLCore();
    ~CHTLCore();
    
    // 编译功能
    bool compileFile(const std::string& inputPath, const std::string& outputPath);
    bool compileString(const std::string& input, std::string& output);
    
    // 模块管理
    bool registerModule(const std::string& name, ModuleType type, const std::string& path);
    bool loadModule(const std::string& name);
    
    // 代码扫描
    std::vector<CodeBlock> scanCode(const std::string& content);
    
    // 版本信息
    std::string getVersion() const;
    
private:
    ModuleManager* moduleManager_;
    std::unique_ptr<UnifiedScanner> scanner_;
};

} // namespace CHTL