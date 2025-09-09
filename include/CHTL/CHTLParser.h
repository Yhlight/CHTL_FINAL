#pragma once

#include "CHTLLexer/Token.h"
#include "CHTLNode/BaseNode.h"
#include <memory>
#include <vector>
#include <string>

namespace CHTL {

/**
 * @brief CHTL语法分析器
 * 
 * 负责将令牌序列转换为抽象语法树(AST)
 */
class CHTLParser {
public:
    CHTLParser();
    ~CHTLParser() = default;

    /**
     * @brief 解析令牌流
     * @param tokens 令牌流
     * @return AST根节点
     */
    std::unique_ptr<BaseNode> parse(std::unique_ptr<TokenStream> tokens);

    /**
     * @brief 获取解析错误
     * @return 错误信息列表
     */
    const std::vector<std::string>& getErrors() const { return m_errors; }

    /**
     * @brief 清空错误信息
     */
    void clearErrors() { m_errors.clear(); }

private:
    std::unique_ptr<TokenStream> m_tokens;
    std::vector<std::string> m_errors;

    // 解析方法
    std::unique_ptr<BaseNode> parseRoot();
    std::unique_ptr<BaseNode> parseElement();
    std::unique_ptr<BaseNode> parseText();
    std::unique_ptr<BaseNode> parseStyle();
    std::unique_ptr<BaseNode> parseScript();
    std::unique_ptr<BaseNode> parseTemplate();
    std::unique_ptr<BaseNode> parseCustom();
    std::unique_ptr<BaseNode> parseOrigin();
    std::unique_ptr<BaseNode> parseImport();
    std::unique_ptr<BaseNode> parseConfiguration();
    std::unique_ptr<BaseNode> parseNamespace();

    // 辅助方法
    bool match(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token* consume(TokenType type);
    Token* consume(const std::vector<TokenType>& types);
    Token* peek();
    Token* peek(size_t offset);
    bool isAtEnd();
    void addError(const std::string& message);
};

} // namespace CHTL