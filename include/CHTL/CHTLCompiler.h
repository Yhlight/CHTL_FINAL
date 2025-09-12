#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * CHTL编译器基类
 */
class CHTLCompiler {
public:
    CHTLCompiler() = default;
    virtual ~CHTLCompiler() = default;
    
    // 编译接口
    virtual std::string compile(const std::string& content) = 0;
    virtual std::string compileFile(const std::string& filePath) = 0;
    
    // 验证接口
    virtual bool validate(const std::string& content) const = 0;
    virtual std::vector<std::string> getValidationErrors(const std::string& content) const = 0;
    
    // 配置接口
    virtual void setConfiguration(const std::string& key, const std::string& value) = 0;
    virtual std::string getConfiguration(const std::string& key) const = 0;
    
    // 状态接口
    virtual bool isReady() const = 0;
    virtual void reset() = 0;
};

} // namespace CHTL