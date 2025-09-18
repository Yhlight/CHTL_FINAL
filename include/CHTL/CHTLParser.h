#pragma once

#include "CHTL/Common.h"
#include "CHTL/BaseNode.h"
#include "CHTL/Token.h"
#include <vector>
#include <memory>
#include <map>
#include <set>

namespace CHTL {

/**
 * CHTL解析器 - 将Token序列解析为AST
 */
class CHTLParser {
public:
    // 解析选项
    struct ParseOptions {
        bool enableTemplates;
        bool enableCustoms;
        bool enableImports;
        bool enableNamespaces;
        bool enableConfiguration;
        bool enableOriginEmbedding;
        bool enableStyleBlocks;
        bool enableScriptBlocks;
        bool enableAttributeOperations;
        bool enablePropertyReferences;
        bool enableConditionalExpressions;
        bool enableResponsiveValues;
        bool debugMode;
        
        ParseOptions() : 
            enableTemplates(true),
            enableCustoms(true),
            enableImports(true),
            enableNamespaces(true),
            enableConfiguration(true),
            enableOriginEmbedding(true),
            enableStyleBlocks(true),
            enableScriptBlocks(true),
            enableAttributeOperations(true),
            enablePropertyReferences(true),
            enableConditionalExpressions(true),
            enableResponsiveValues(true),
            debugMode(false) {}
    };

    // 解析结果
    struct ParseResult {
        bool success;
        std::shared_ptr<BaseNode> ast;
        std::string errorMessage;
        std::vector<std::string> warnings;
        size_t tokensConsumed;
        
        ParseResult() : success(false), tokensConsumed(0) {}
    };

public:
    CHTLParser();
    ~CHTLParser();

    /**
     * 解析Token序列
     * @param tokens Token序列
     * @param options 解析选项
     * @return 解析结果
     */
    ParseResult parse(const std::vector<Token>& tokens, const ParseOptions& options = {});

    /**
     * 设置上下文
     * @param context 上下文
     */
    void setContext(std::shared_ptr<void> context);

    /**
     * 获取上下文
     * @return 上下文
     */
    std::shared_ptr<void> getContext() const;

    /**
     * 重置解析器状态
     */
    void reset();

private:
    // 内部解析方法
    ParseResult parseDocument(const std::vector<Token>& tokens, size_t startPos);
    ParseResult parseElement(const std::vector<Token>& tokens, size_t startPos);
    ParseResult parseText(const std::vector<Token>& tokens, size_t startPos);
    ParseResult parseStyle(const std::vector<Token>& tokens, size_t startPos);
    ParseResult parseScript(const std::vector<Token>& tokens, size_t startPos);
    ParseResult parseTemplate(const std::vector<Token>& tokens, size_t startPos);
    ParseResult parseCustom(const std::vector<Token>& tokens, size_t startPos);
    ParseResult parseImport(const std::vector<Token>& tokens, size_t startPos);
    ParseResult parseNamespace(const std::vector<Token>& tokens, size_t startPos);
    ParseResult parseConfiguration(const std::vector<Token>& tokens, size_t startPos);
    ParseResult parseOrigin(const std::vector<Token>& tokens, size_t startPos);
    ParseResult parseOperator(const std::vector<Token>& tokens, size_t startPos);
    
    // 属性解析
    std::map<std::string, std::string> parseAttributes(const std::vector<Token>& tokens, size_t& pos);
    std::string parseAttributeValue(const std::vector<Token>& tokens, size_t& pos);
    
    // 样式解析
    std::map<std::string, std::string> parseStyleProperties(const std::vector<Token>& tokens, size_t& pos);
    std::string parseStyleValue(const std::vector<Token>& tokens, size_t& pos);
    
    // 脚本解析
    std::string parseScriptContent(const std::vector<Token>& tokens, size_t& pos);
    
    // 模板解析
    std::map<std::string, std::string> parseTemplateParameters(const std::vector<Token>& tokens, size_t& pos);
    
    // 导入解析
    std::string parseImportPath(const std::vector<Token>& tokens, size_t& pos);
    std::string parseImportAlias(const std::vector<Token>& tokens, size_t& pos);
    
    // 命名空间解析
    std::string parseNamespaceName(const std::vector<Token>& tokens, size_t& pos);
    
    // 配置解析
    std::map<std::string, std::string> parseConfigurationProperties(const std::vector<Token>& tokens, size_t& pos);
    
    // 原始嵌入解析
    std::string parseOriginContent(const std::vector<Token>& tokens, size_t& pos);
    
    // 操作符解析
    std::string parseOperatorType(const std::vector<Token>& tokens, size_t& pos);
    std::vector<std::string> parseOperatorTargets(const std::vector<Token>& tokens, size_t& pos);
    
    // 工具方法
    bool isAtEnd(const std::vector<Token>& tokens, size_t pos) const;
    Token getCurrentToken(const std::vector<Token>& tokens, size_t pos) const;
    Token peekToken(const std::vector<Token>& tokens, size_t pos, size_t offset = 1) const;
    bool matchToken(const std::vector<Token>& tokens, size_t& pos, TokenType type) const;
    bool matchToken(const std::vector<Token>& tokens, size_t& pos, TokenType type, const std::string& value) const;
    bool matchToken(const std::vector<Token>& tokens, size_t& pos, const std::string& value) const;
    void advancePosition(size_t& pos) const;
    void skipWhitespace(const std::vector<Token>& tokens, size_t& pos) const;
    
    // 错误处理
    void handleError(const std::string& error, ParseResult& result);
    void handleWarning(const std::string& warning, ParseResult& result);
    
    // 验证
    bool validateElement(const std::string& elementName) const;
    bool validateAttribute(const std::string& attributeName) const;
    bool validateStyleProperty(const std::string& propertyName) const;
    bool validateTemplateName(const std::string& templateName) const;
    bool validateCustomName(const std::string& customName) const;
    bool validateImportPath(const std::string& path) const;
    bool validateNamespaceName(const std::string& name) const;
    bool validateConfigurationProperty(const std::string& property) const;
    bool validateOriginType(const std::string& type) const;
    bool validateOperatorType(const std::string& type) const;

private:
    std::shared_ptr<void> context_;
    ParseOptions currentOptions_;
    
    // 解析状态
    size_t currentPosition_;
    std::vector<Token> currentTokens_;
    
    // 验证器
    std::set<std::string> validElements_;
    std::set<std::string> validAttributes_;
    std::set<std::string> validStyleProperties_;
    std::set<std::string> validTemplateNames_;
    std::set<std::string> validCustomNames_;
    std::set<std::string> validOriginTypes_;
    std::set<std::string> validOperatorTypes_;
    
    // 初始化验证器
    void initializeValidators();
};

} // namespace CHTL