#ifndef CHTL_STATE_H
#define CHTL_STATE_H

#include <string>
#include <map>
#include <vector>
#include <memory>

namespace CHTL {

enum class CHTLStateType {
    INITIAL,
    PARSING,
    GENERATING,
    COMPLETED,
    ERROR
};

class CHTLState {
private:
    CHTLStateType currentState;
    std::map<std::string, std::string> stateData;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    bool debugMode;

public:
    CHTLState();
    CHTLState(CHTLStateType initialState);
    virtual ~CHTLState() = default;
    
    // 状态管理
    CHTLStateType getCurrentState() const { return currentState; }
    void setCurrentState(CHTLStateType state) { currentState = state; }
    
    // 状态数据管理
    void setStateData(const std::string& key, const std::string& value);
    std::string getStateData(const std::string& key) const;
    bool hasStateData(const std::string& key) const;
    void removeStateData(const std::string& key);
    const std::map<std::string, std::string>& getAllStateData() const { return stateData; }
    
    // 错误和警告管理
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    const std::vector<std::string>& getErrors() const { return errors; }
    const std::vector<std::string>& getWarnings() const { return warnings; }
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    void clearErrors();
    void clearWarnings();
    
    // 模式设置
    bool isDebugMode() const { return debugMode; }
    void setDebugMode(bool mode) { debugMode = mode; }
    
    // 状态操作
    void reset();
    void clear();
    bool isValid() const;
    bool canTransitionTo(CHTLStateType targetState) const;
    bool transitionTo(CHTLStateType targetState);
    
    // 字符串表示
    std::string toString() const;
    std::string toDebugString() const;
    std::string getStateName() const;
    
    // 序列化
    std::string toJSON() const;
    std::string toXML() const;
    std::string toYAML() const;
    
    // 克隆
    CHTLState clone() const;
    
    // 比较
    bool operator==(const CHTLState& other) const;
    bool operator!=(const CHTLState& other) const;
};

} // namespace CHTL

#endif // CHTL_STATE_H