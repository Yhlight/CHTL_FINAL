#ifndef CHTL_SCRIPT_NODE_H
#define CHTL_SCRIPT_NODE_H

#include "BaseNode.h"
#include <vector>
#include <map>

namespace CHTL {

class ScriptNode : public BaseNode {
private:
    bool isLocalScript;  // 是否为局部脚本块
    std::string scriptContent;  // 脚本内容
    std::vector<std::shared_ptr<BaseNode>> scriptFunctions;  // 脚本函数
    
    // 脚本类型
    enum class ScriptType {
        JAVASCRIPT,      // 普通JavaScript
        CHTL_JS,         // CHTL JS
        INLINE,          // 内联脚本
        EXTERNAL         // 外部脚本
    };

public:
    ScriptNode(bool local = true);
    virtual ~ScriptNode() = default;
    
    // 脚本类型
    bool isLocalScriptBlock() const { return isLocalScript; }
    void setLocalScript(bool local) { isLocalScript = local; }
    
    // 脚本内容
    const std::string& getScriptContent() const { return scriptContent; }
    void setScriptContent(const std::string& content) { scriptContent = content; }
    
    // 脚本函数操作
    void addScriptFunction(std::shared_ptr<BaseNode> function);
    void removeScriptFunction(std::shared_ptr<BaseNode> function);
    std::vector<std::shared_ptr<BaseNode>>& getScriptFunctions() { return scriptFunctions; }
    const std::vector<std::shared_ptr<BaseNode>>& getScriptFunctions() const { return scriptFunctions; }
    
    // 脚本处理
    std::string getMinifiedScript() const;
    std::string getFormattedScript() const;
    bool isEmpty() const;
    bool isWhitespace() const;
    
    // 验证
    bool isValid() const override;
    
    // 转换为HTML
    std::string toHTML() const override;
    std::string toJavaScript() const;
    
    // 调试信息
    std::string getDebugInfo() const override;
    
    // 克隆
    std::shared_ptr<BaseNode> clone() const override;
    
    // 比较操作
    bool operator==(const ScriptNode& other) const;
    bool operator!=(const ScriptNode& other) const;

private:
    // 辅助方法
    std::string minifyJavaScript(const std::string& script) const;
    std::string formatJavaScript(const std::string& script) const;
    bool isWhitespaceChar(char c) const;
    
    // 脚本解析
    std::vector<std::string> parseScriptLines(const std::string& script) const;
    std::string parseScriptFunction(const std::string& function) const;
    
    // 脚本优化
    std::string optimizeScript(const std::string& script) const;
    std::string removeComments(const std::string& script) const;
    std::string compressWhitespace(const std::string& script) const;
};

} // namespace CHTL

#endif // CHTL_SCRIPT_NODE_H