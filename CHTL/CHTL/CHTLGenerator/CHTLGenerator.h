#pragma once

#include "../CHTLNode/BaseNode.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace CHTL {

class CHTLGenerator {
public:
    CHTLGenerator();
    
    // 生成HTML
    std::string generateHTML(std::shared_ptr<BaseNode> root);
    
    // 生成CSS
    std::string generateCSS(std::shared_ptr<BaseNode> root);
    
    // 生成JavaScript
    std::string generateJS(std::shared_ptr<BaseNode> root);
    
    // 生成完整文档
    std::string generateDocument(std::shared_ptr<BaseNode> root);
    
    // 设置配置
    void setConfiguration(const std::unordered_map<std::string, std::string>& config);
    
private:
    std::unordered_map<std::string, std::string> configuration_;
    std::vector<std::string> global_css_;
    std::vector<std::string> global_js_;
    
    // HTML生成辅助方法
    std::string generateElementHTML(std::shared_ptr<BaseNode> node);
    std::string generateTextHTML(std::shared_ptr<BaseNode> node);
    std::string generateTemplateHTML(std::shared_ptr<BaseNode> node);
    std::string generateCustomHTML(std::shared_ptr<BaseNode> node);
    std::string generateOriginHTML(std::shared_ptr<BaseNode> node);
    
    // CSS生成辅助方法
    std::string generateElementCSS(std::shared_ptr<BaseNode> node);
    std::string generateStyleCSS(std::shared_ptr<BaseNode> node);
    std::string generateTemplateCSS(std::shared_ptr<BaseNode> node);
    std::string generateCustomCSS(std::shared_ptr<BaseNode> node);
    
    // JS生成辅助方法
    std::string generateElementJS(std::shared_ptr<BaseNode> node);
    std::string generateScriptJS(std::shared_ptr<BaseNode> node);
    std::string generateTemplateJS(std::shared_ptr<BaseNode> node);
    std::string generateCustomJS(std::shared_ptr<BaseNode> node);
    
    // 工具方法
    std::string escapeHTML(const std::string& text);
    std::string escapeCSS(const std::string& text);
    std::string escapeJS(const std::string& text);
    
    // 模板处理
    void processTemplates(std::shared_ptr<BaseNode> root);
    void processCustoms(std::shared_ptr<BaseNode> root);
    
    // 样式处理
    void processStyles(std::shared_ptr<BaseNode> node);
    void processStyleBlock(std::shared_ptr<BaseNode> node);
    
    // 脚本处理
    void processScripts(std::shared_ptr<BaseNode> node);
    void processScriptBlock(std::shared_ptr<BaseNode> node);
};

} // namespace CHTL