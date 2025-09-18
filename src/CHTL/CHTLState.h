#ifndef CHTL_STATE_H
#define CHTL_STATE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace CHTL {

enum class CompilerState {
    INITIAL,
    LEXING,
    PARSING,
    GENERATING,
    COMPLETED,
    ERROR
};

class CHTLState {
public:
    CHTLState();
    ~CHTLState() = default;
    
    // 状态管理
    CompilerState getCurrentState() const { return currentState_; }
    void setState(CompilerState state);
    
    // 文件管理
    void setInputFile(const std::string& filename);
    std::string getInputFile() const { return inputFile_; }
    
    void setOutputFile(const std::string& filename);
    std::string getOutputFile() const { return outputFile_; }
    
    // 配置管理
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key) const;
    
    // 错误和警告
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    const std::vector<std::string>& getErrors() const { return errors_; }
    const std::vector<std::string>& getWarnings() const { return warnings_; }
    bool hasErrors() const { return !errors_.empty(); }
    bool hasWarnings() const { return !warnings_.empty(); }
    
    // 清理
    void clear();
    
private:
    CompilerState currentState_;
    std::string inputFile_;
    std::string outputFile_;
    std::unordered_map<std::string, std::string> configurations_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
};

} // namespace CHTL

#endif // CHTL_STATE_H