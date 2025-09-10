#pragma once

#include "chtl_js/chtl_js_lexer.h"
#include "chtl_js/chtl_js_ast.h"
#include <memory>
#include <vector>

namespace chtl {
namespace chtl_js {

// CHTL JS解析器
class CHTLJSParser {
public:
    explicit CHTLJSParser(const std::string& source);
    explicit CHTLJSParser(const std::vector<CHTLJSToken>& tokens);
    
    // 主要接口
    std::shared_ptr<ProgramNode> parse();
    
    // 状态查询
    bool is_at_end() const;
    CHTLJSToken current_token() const;
    CHTLJSToken peek_token(size_t offset = 1) const;
    
    // 重置
    void reset();
    
private:
    std::vector<CHTLJSToken> tokens_;
    size_t current_;
    
    // 解析方法
    std::shared_ptr<CHTLJSASTNode> parse_statement();
    std::shared_ptr<CHTLJSASTNode> parse_expression();
    std::shared_ptr<CHTLJSASTNode> parse_primary_expression();
    std::shared_ptr<CHTLJSASTNode> parse_binary_expression();
    std::shared_ptr<CHTLJSASTNode> parse_call_expression();
    std::shared_ptr<CHTLJSASTNode> parse_member_expression();
    std::shared_ptr<CHTLJSASTNode> parse_assignment_expression();
    std::shared_ptr<CHTLJSASTNode> parse_conditional_expression();
    
    // 声明解析
    std::shared_ptr<CHTLJSASTNode> parse_variable_declaration();
    std::shared_ptr<CHTLJSASTNode> parse_function_declaration();
    std::shared_ptr<CHTLJSASTNode> parse_class_declaration();
    
    // 语句解析
    std::shared_ptr<CHTLJSASTNode> parse_if_statement();
    std::shared_ptr<CHTLJSASTNode> parse_for_statement();
    std::shared_ptr<CHTLJSASTNode> parse_while_statement();
    std::shared_ptr<CHTLJSASTNode> parse_switch_statement();
    std::shared_ptr<CHTLJSASTNode> parse_try_statement();
    std::shared_ptr<CHTLJSASTNode> parse_block_statement();
    std::shared_ptr<CHTLJSASTNode> parse_expression_statement();
    
    // CHTL JS特有语法解析
    std::shared_ptr<CHTLJSASTNode> parse_fileloader();
    std::shared_ptr<CHTLJSASTNode> parse_listen();
    std::shared_ptr<CHTLJSASTNode> parse_delegate();
    std::shared_ptr<CHTLJSASTNode> parse_animate();
    std::shared_ptr<CHTLJSASTNode> parse_vir();
    std::shared_ptr<CHTLJSASTNode> parse_router();
    std::shared_ptr<CHTLJSASTNode> parse_ineveraway();
    std::shared_ptr<CHTLJSASTNode> parse_util();
    std::shared_ptr<CHTLJSASTNode> parse_selector();
    
    // 辅助方法
    bool match_token(CHTLJSTokenType type);
    bool match_any_token(const std::vector<CHTLJSTokenType>& types);
    CHTLJSToken consume_token(CHTLJSTokenType type, const std::string& error_message);
    CHTLJSToken advance();
    void synchronize();
    
    // 表达式解析辅助
    std::vector<std::shared_ptr<CHTLJSASTNode>> parse_expression_list();
    std::vector<std::shared_ptr<CHTLJSASTNode>> parse_parameter_list();
    std::shared_ptr<CHTLJSASTNode> parse_object_expression();
    std::shared_ptr<CHTLJSASTNode> parse_array_expression();
    
    // 错误处理
    void error(const std::string& message);
    void error_at_current(const std::string& message);
    void error_at_token(const CHTLJSToken& token, const std::string& message);
    bool is_expression_start() const;
    
    // 优先级处理
    int get_precedence(CHTLJSTokenType type);
    bool is_right_associative(CHTLJSTokenType type);
    
    // 字面量解析
    std::shared_ptr<CHTLJSASTNode> parse_literal();
    std::shared_ptr<CHTLJSASTNode> parse_string_literal();
    std::shared_ptr<CHTLJSASTNode> parse_number_literal();
    std::shared_ptr<CHTLJSASTNode> parse_boolean_literal();
    
    // 属性解析
    std::vector<std::shared_ptr<CHTLJSASTNode>> parse_property_list();
    std::shared_ptr<CHTLJSASTNode> parse_property();
    
    // 选择器解析
    std::string parse_selector_string();
    bool is_selector_start();
    
    // 错误状态
    bool had_error_;
    std::string last_error_;
};

} // namespace chtl_js
} // namespace chtl