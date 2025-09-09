#ifndef CHTL_COMPILER_DISPATCHER_H
#define CHTL_COMPILER_DISPATCHER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace CHTL {

// Forward declarations
class CHTLLexer;
class CHTLParser;
class CHTLGenerator;
class CHTLJSLexer;
class CHTLJSParser;
// Note: CHTLJSFeatures and CHTLJSDynamicExpressions are not used in this simplified version
class CHTLModuleSystem;
class CHTLCJMODAPI;
class CHTLUnifiedScanner;

// Compiler types
enum class CompilerType {
    CHTL,
    CHTLJS,
    CSS,
    JavaScript,
    HTML,
    Mixed
};

// Compiler status
enum class CompilerStatus {
    Idle,
    Running,
    Completed,
    Error,
    Cancelled
};

// Compiler result
struct CompilerResult {
    CompilerStatus status;
    std::string output;
    std::string error_message;
    std::vector<std::string> warnings;
    std::map<std::string, std::string> metadata;
    
    CompilerResult() : status(CompilerStatus::Idle) {}
};

// Compiler configuration
struct CompilerConfig {
    CompilerType type;
    std::string input_file;
    std::string output_file;
    std::map<std::string, std::string> options;
    bool debug_mode;
    bool verbose;
    
    CompilerConfig() : type(CompilerType::CHTL), debug_mode(false), verbose(false) {}
};

// Compiler interface
class ICompiler {
public:
    virtual ~ICompiler() = default;
    virtual CompilerResult compile(const CompilerConfig& config) = 0;
    virtual bool canHandle(const CompilerType& type) const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::vector<std::string> getSupportedExtensions() const = 0;
};

// CHTL Compiler
class CHTLCompiler : public ICompiler {
private:
    std::unique_ptr<CHTLLexer> lexer;
    std::unique_ptr<CHTLParser> parser;
    std::unique_ptr<CHTLGenerator> generator;
    
public:
    CHTLCompiler();
    ~CHTLCompiler();
    
    CompilerResult compile(const CompilerConfig& config) override;
    bool canHandle(const CompilerType& type) const override;
    std::string getVersion() const override;
    std::vector<std::string> getSupportedExtensions() const override;
};

// CHTL JS Compiler
class CHTLJSCompiler : public ICompiler {
private:
    std::unique_ptr<CHTLJSLexer> lexer;
    std::unique_ptr<CHTLJSParser> parser;
    // Note: features and expressions are not used in this simplified version
    
public:
    CHTLJSCompiler();
    ~CHTLJSCompiler();
    
    CompilerResult compile(const CompilerConfig& config) override;
    bool canHandle(const CompilerType& type) const override;
    std::string getVersion() const override;
    std::vector<std::string> getSupportedExtensions() const override;
};

// CSS Compiler
class CSSCompiler : public ICompiler {
public:
    CSSCompiler();
    ~CSSCompiler();
    
    CompilerResult compile(const CompilerConfig& config) override;
    bool canHandle(const CompilerType& type) const override;
    std::string getVersion() const override;
    std::vector<std::string> getSupportedExtensions() const override;
};

// JavaScript Compiler
class JavaScriptCompiler : public ICompiler {
public:
    JavaScriptCompiler();
    ~JavaScriptCompiler();
    
    CompilerResult compile(const CompilerConfig& config) override;
    bool canHandle(const CompilerType& type) const override;
    std::string getVersion() const override;
    std::vector<std::string> getSupportedExtensions() const override;
};

// HTML Compiler
class HTMLCompiler : public ICompiler {
public:
    HTMLCompiler();
    ~HTMLCompiler();
    
    CompilerResult compile(const CompilerConfig& config) override;
    bool canHandle(const CompilerType& type) const override;
    std::string getVersion() const override;
    std::vector<std::string> getSupportedExtensions() const override;
};

// Mixed Compiler
class MixedCompiler : public ICompiler {
private:
    std::unique_ptr<CHTLCompiler> chtl_compiler;
    std::unique_ptr<CHTLJSCompiler> chtljs_compiler;
    std::unique_ptr<CSSCompiler> css_compiler;
    std::unique_ptr<JavaScriptCompiler> js_compiler;
    std::unique_ptr<HTMLCompiler> html_compiler;
    
public:
    MixedCompiler();
    ~MixedCompiler();
    
    CompilerResult compile(const CompilerConfig& config) override;
    bool canHandle(const CompilerType& type) const override;
    std::string getVersion() const override;
    std::vector<std::string> getSupportedExtensions() const override;
};

// Compiler Dispatcher
class CHTLCompilerDispatcher {
private:
    std::map<CompilerType, std::unique_ptr<ICompiler>> compilers;
    std::map<std::string, CompilerType> extension_map;
    std::vector<std::string> search_paths;
    bool debug_mode;
    bool verbose;
    
public:
    CHTLCompilerDispatcher();
    ~CHTLCompilerDispatcher();
    
    // Compiler management
    void registerCompiler(CompilerType type, std::unique_ptr<ICompiler> compiler);
    void unregisterCompiler(CompilerType type);
    ICompiler* getCompiler(CompilerType type);
    std::vector<CompilerType> getAvailableCompilers() const;
    
    // Compilation
    CompilerResult compile(const CompilerConfig& config);
    CompilerResult compile(const std::string& input_file, const std::string& output_file, CompilerType type = CompilerType::CHTL);
    CompilerResult compile(const std::string& input_file, CompilerType type = CompilerType::CHTL);
    
    // Batch compilation
    std::vector<CompilerResult> compileBatch(const std::vector<CompilerConfig>& configs);
    std::vector<CompilerResult> compileDirectory(const std::string& directory, CompilerType type = CompilerType::CHTL);
    
    // Configuration
    void setDebugMode(bool enabled);
    void setVerboseMode(bool enabled);
    void addSearchPath(const std::string& path);
    void removeSearchPath(const std::string& path);
    void setExtensionMapping(const std::string& extension, CompilerType type);
    
    // Utility
    CompilerType detectCompilerType(const std::string& file_path) const;
    std::string getCompilerVersion(CompilerType type) const;
    std::vector<std::string> getSupportedExtensions(CompilerType type) const;
    bool isFileSupported(const std::string& file_path) const;
    
    // Status
    CompilerStatus getCompilerStatus(CompilerType type) const;
    std::map<CompilerType, CompilerStatus> getAllCompilerStatus() const;
    void resetCompilerStatus(CompilerType type);
    void resetAllCompilerStatus();
    
    // Error handling
    std::vector<std::string> getCompilerErrors(CompilerType type) const;
    std::map<CompilerType, std::vector<std::string>> getAllCompilerErrors() const;
    void clearCompilerErrors(CompilerType type);
    void clearAllCompilerErrors();
    
    // Statistics
    std::map<CompilerType, int> getCompilationCounts() const;
    std::map<CompilerType, double> getCompilationTimes() const;
    void resetStatistics();
    
    // Callbacks
    void setCompilationCallback(std::function<void(CompilerType, CompilerStatus)> callback);
    void setErrorCallback(std::function<void(CompilerType, const std::string&)> callback);
    void setProgressCallback(std::function<void(CompilerType, int, int)> callback);
    
private:
    std::map<CompilerType, CompilerStatus> compiler_status;
    std::map<CompilerType, std::vector<std::string>> compiler_errors;
    std::map<CompilerType, int> compilation_counts;
    std::map<CompilerType, double> compilation_times;
    
    std::function<void(CompilerType, CompilerStatus)> compilation_callback;
    std::function<void(CompilerType, const std::string&)> error_callback;
    std::function<void(CompilerType, int, int)> progress_callback;
    
    void updateCompilerStatus(CompilerType type, CompilerStatus status);
    void addCompilerError(CompilerType type, const std::string& error);
    void updateCompilationCount(CompilerType type);
    void updateCompilationTime(CompilerType type, double time);
    void notifyCompilationCallback(CompilerType type, CompilerStatus status);
    void notifyErrorCallback(CompilerType type, const std::string& error);
    void notifyProgressCallback(CompilerType type, int current, int total);
    
    std::string readFile(const std::string& file_path) const;
    bool writeFile(const std::string& file_path, const std::string& content) const;
    std::string getFileExtension(const std::string& file_path) const;
    std::string getFileName(const std::string& file_path) const;
    std::string getFileDirectory(const std::string& file_path) const;
    bool fileExists(const std::string& file_path) const;
    bool createDirectory(const std::string& directory) const;
    std::vector<std::string> findFiles(const std::string& directory, const std::vector<std::string>& extensions) const;
};

} // namespace CHTL

#endif // CHTL_COMPILER_DISPATCHER_H