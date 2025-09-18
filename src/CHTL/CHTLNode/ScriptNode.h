#ifndef SCRIPT_NODE_H
#define SCRIPT_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class ScriptNode : public BaseNode {
public:
    ScriptNode(const std::string& name = "script");
    ~ScriptNode() = default;
    
    // 脚本内容
    void setScriptContent(const std::string& content);
    const std::string& getScriptContent() const { return scriptContent_; }
    
    // 脚本类型
    void setScriptType(const std::string& type);
    const std::string& getScriptType() const { return scriptType_; }
    
    // 脚本源
    void setScriptSrc(const std::string& src);
    const std::string& getScriptSrc() const { return scriptSrc_; }
    
    // 重写基类方法
    std::string generateHTML() const override;
    std::string generateCSS() const override;
    std::string generateJS() const override;
    
private:
    std::string scriptContent_;
    std::string scriptType_;
    std::string scriptSrc_;
};

} // namespace CHTL

#endif // SCRIPT_NODE_H