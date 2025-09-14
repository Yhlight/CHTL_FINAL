#pragma once

#include "CHTLLexer/CHTLLexer.h"
#include "CHTLNode/CHTLNode.h"
#include <memory>
#include <vector>

namespace CHTL {

class CHTLParser {
public:
    CHTLParser();
    ~CHTLParser() = default;
    
    // 主要解析函数
    std::unique_ptr<CHTLNode> parse(const std::vector<Token>& tokens);
    
    // 配置
    void setDebugMode(bool enabled) { debugMode_ = enabled; }
    
    // 错误处理
    bool hasErrors() const { return !errors_.empty(); }
    std::vector<std::string> getErrors() const { return errors_; }
    void clearErrors() { errors_.clear(); }

private:
    // 状态
    std::vector<Token> tokens_;
    size_t currentToken_;
    bool debugMode_;
    std::vector<std::string> errors_;
    
    // 核心解析函数
    std::unique_ptr<CHTLNode> parseDocument();
    std::unique_ptr<CHTLNode> parseElement();
    std::unique_ptr<CHTLNode> parseTextNode();
    std::unique_ptr<CHTLNode> parseComment();
    std::unique_ptr<CHTLNode> parseTemplate();
    std::unique_ptr<CHTLNode> parseCustom();
    std::unique_ptr<CHTLNode> parseOrigin();
    std::unique_ptr<CHTLNode> parseImport();
    std::unique_ptr<CHTLNode> parseNamespace();
    std::unique_ptr<CHTLNode> parseConfiguration();
    
    // 属性解析
    std::vector<Attribute> parseAttributes();
    Attribute parseAttribute();
    
    // 样式块解析
    std::unique_ptr<StyleNode> parseStyleBlock();
    std::vector<StyleRule> parseStyleRules();
    StyleRule parseStyleRule();
    
    // 脚本块解析
    std::unique_ptr<ScriptNode> parseScriptBlock();
    
    // 模板解析
    std::unique_ptr<TemplateNode> parseStyleTemplate();
    std::unique_ptr<TemplateNode> parseElementTemplate();
    std::unique_ptr<TemplateNode> parseVarTemplate();
    
    // 自定义解析
    std::unique_ptr<CustomNode> parseCustomStyle();
    std::unique_ptr<CustomNode> parseCustomElement();
    std::unique_ptr<CustomNode> parseCustomVar();
    
    // 辅助函数
    Token currentToken() const;
    Token peekToken() const;
    bool match(TokenType type);
    bool match(const std::string& value);
    Token advance();
    void skipToNextStatement();
    
    // 错误处理
    void addError(const std::string& message);
    void addError(const std::string& message, const Token& token);
    
    // 调试
    void debugParse(const std::string& message) const;
};

} // namespace CHTL