#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

namespace CHTL {

/**
 * @brief CSS 编译器接口
 */
class CSSCompiler {
public:
    virtual ~CSSCompiler() = default;
    
    // 编译 CSS
    virtual std::string compile(const std::string& css) = 0;
    virtual std::string compileFile(const std::string& filePath) = 0;
    
    // 编译选项
    virtual void setOutputStyle(const std::string& style) = 0;
    virtual void setSourceMap(bool enable) = 0;
    virtual void setPrecision(int precision) = 0;
    
    // 错误处理
    virtual std::vector<std::string> getErrors() const = 0;
    virtual bool hasErrors() const = 0;
    virtual void clearErrors() = 0;
};

/**
 * @brief JavaScript 编译器接口
 */
class JSCompiler {
public:
    virtual ~JSCompiler() = default;
    
    // 编译 JavaScript
    virtual std::string compile(const std::string& js) = 0;
    virtual std::string compileFile(const std::string& filePath) = 0;
    
    // 执行 JavaScript
    virtual std::string execute(const std::string& js) = 0;
    virtual std::string executeFile(const std::string& filePath) = 0;
    
    // 编译选项
    virtual void setTarget(const std::string& target) = 0;
    virtual void setOptimizationLevel(int level) = 0;
    virtual void setSourceMap(bool enable) = 0;
    
    // 错误处理
    virtual std::vector<std::string> getErrors() const = 0;
    virtual bool hasErrors() const = 0;
    virtual void clearErrors() = 0;
};

/**
 * @brief ANTLR4 CSS 编译器实现
 */
class ANTLR4CSSCompiler : public CSSCompiler {
public:
    ANTLR4CSSCompiler();
    ~ANTLR4CSSCompiler() override = default;
    
    std::string compile(const std::string& css) override;
    std::string compileFile(const std::string& filePath) override;
    
    void setOutputStyle(const std::string& style) override;
    void setSourceMap(bool enable) override;
    void setPrecision(int precision) override;
    
    std::vector<std::string> getErrors() const override;
    bool hasErrors() const override;
    void clearErrors() override;

private:
    std::string m_outputStyle;
    bool m_sourceMap;
    int m_precision;
    std::vector<std::string> m_errors;
    
    // 辅助方法
    std::string processCSS(const std::string& css) const;
    void addError(const std::string& error);
};

/**
 * @brief ANTLR4 JavaScript 编译器实现
 */
class ANTLR4JSCompiler : public JSCompiler {
public:
    ANTLR4JSCompiler();
    ~ANTLR4JSCompiler() override = default;
    
    std::string compile(const std::string& js) override;
    std::string compileFile(const std::string& filePath) override;
    
    std::string execute(const std::string& js) override;
    std::string executeFile(const std::string& filePath) override;
    
    void setTarget(const std::string& target) override;
    void setOptimizationLevel(int level) override;
    void setSourceMap(bool enable) override;
    
    std::vector<std::string> getErrors() const override;
    bool hasErrors() const override;
    void clearErrors() override;

private:
    std::string m_target;
    int m_optimizationLevel;
    bool m_sourceMap;
    std::vector<std::string> m_errors;
    
    // 辅助方法
    std::string processJS(const std::string& js) const;
    void addError(const std::string& error);
};

/**
 * @brief libsass CSS 编译器实现
 */
class LibSassCSSCompiler : public CSSCompiler {
public:
    LibSassCSSCompiler();
    ~LibSassCSSCompiler() override = default;
    
    std::string compile(const std::string& css) override;
    std::string compileFile(const std::string& filePath) override;
    
    void setOutputStyle(const std::string& style) override;
    void setSourceMap(bool enable) override;
    void setPrecision(int precision) override;
    
    std::vector<std::string> getErrors() const override;
    bool hasErrors() const override;
    void clearErrors() override;

private:
    std::string m_outputStyle;
    bool m_sourceMap;
    int m_precision;
    std::vector<std::string> m_errors;
    
    // 辅助方法
    std::string processSass(const std::string& sass) const;
    void addError(const std::string& error);
};

/**
 * @brief V8 JavaScript 编译器实现
 */
class V8JSCompiler : public JSCompiler {
public:
    V8JSCompiler();
    ~V8JSCompiler() override = default;
    
    std::string compile(const std::string& js) override;
    std::string compileFile(const std::string& filePath) override;
    
    std::string execute(const std::string& js) override;
    std::string executeFile(const std::string& filePath) override;
    
    void setTarget(const std::string& target) override;
    void setOptimizationLevel(int level) override;
    void setSourceMap(bool enable) override;
    
    std::vector<std::string> getErrors() const override;
    bool hasErrors() const override;
    void clearErrors() override;

private:
    std::string m_target;
    int m_optimizationLevel;
    bool m_sourceMap;
    std::vector<std::string> m_errors;
    
    // 辅助方法
    std::string processJS(const std::string& js) const;
    void addError(const std::string& error);
};

/**
 * @brief 外部编译器管理器
 */
class ExternalCompilerManager {
public:
    explicit ExternalCompilerManager();
    ~ExternalCompilerManager() = default;
    
    // 编译器管理
    void setCSSCompiler(std::shared_ptr<CSSCompiler> compiler);
    void setJSCompiler(std::shared_ptr<JSCompiler> compiler);
    
    std::shared_ptr<CSSCompiler> getCSSCompiler() const;
    std::shared_ptr<JSCompiler> getJSCompiler() const;
    
    // 编译功能
    std::string compileCSS(const std::string& css);
    std::string compileJS(const std::string& js);
    std::string executeJS(const std::string& js);
    
    // 编译器选择
    void useANTLR4Compilers();
    void useLibSassV8Compilers();
    void useDefaultCompilers();
    
    // 错误处理
    std::vector<std::string> getCSSErrors() const;
    std::vector<std::string> getJSErrors() const;
    bool hasErrors() const;
    void clearErrors();

private:
    std::shared_ptr<CSSCompiler> m_cssCompiler;
    std::shared_ptr<JSCompiler> m_jsCompiler;
    
    // 辅助方法
    void initializeDefaultCompilers();
};

} // namespace CHTL