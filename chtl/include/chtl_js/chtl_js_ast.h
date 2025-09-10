#pragma once

#include "chtl_js/chtl_js_lexer.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace chtl {
namespace chtl_js {

// CHTL JS AST节点类型
enum class CHTLJSASTNodeType {
    // 程序结构
    PROGRAM,
    STATEMENT,
    EXPRESSION,
    
    // 声明
    VARIABLE_DECLARATION,
    FUNCTION_DECLARATION,
    CLASS_DECLARATION,
    
    // 表达式
    IDENTIFIER,
    LITERAL,
    BINARY_EXPRESSION,
    UNARY_EXPRESSION,
    ASSIGNMENT_EXPRESSION,
    CALL_EXPRESSION,
    MEMBER_EXPRESSION,
    CONDITIONAL_EXPRESSION,
    
    // CHTL JS特有语法
    FILELOADER_EXPRESSION,
    LISTEN_EXPRESSION,
    DELEGATE_EXPRESSION,
    ANIMATE_EXPRESSION,
    VIR_DECLARATION,
    ROUTER_EXPRESSION,
    INEVERAWAY_EXPRESSION,
    UTIL_EXPRESSION,
    SELECTOR_EXPRESSION,
    
    // 控制流
    IF_STATEMENT,
    FOR_STATEMENT,
    WHILE_STATEMENT,
    SWITCH_STATEMENT,
    CASE_CLAUSE,
    BREAK_STATEMENT,
    CONTINUE_STATEMENT,
    RETURN_STATEMENT,
    
    // 异常处理
    TRY_STATEMENT,
    CATCH_CLAUSE,
    FINALLY_CLAUSE,
    THROW_STATEMENT,
    
    // 块语句
    BLOCK_STATEMENT,
    EXPRESSION_STATEMENT,
    
    // 其他
    ARRAY_EXPRESSION,
    OBJECT_EXPRESSION,
    PROPERTY,
    SEQUENCE_EXPRESSION
};

// CHTL JS AST节点基类
class CHTLJSASTNode {
public:
    CHTLJSASTNodeType type;
    size_t line;
    size_t column;
    
    CHTLJSASTNode(CHTLJSASTNodeType t, size_t l = 1, size_t c = 1) 
        : type(t), line(l), column(c) {}
    
    virtual ~CHTLJSASTNode() = default;
    virtual std::string to_string() const = 0;
    virtual std::string to_js() const = 0;
};

// 程序节点
class ProgramNode : public CHTLJSASTNode {
public:
    std::vector<std::shared_ptr<CHTLJSASTNode>> body;
    
    ProgramNode() : CHTLJSASTNode(CHTLJSASTNodeType::PROGRAM) {}
    
    std::string to_string() const override;
    std::string to_js() const override;
};

// 事件委托节点
class DelegateNode : public CHTLJSASTNode {
public:
    std::shared_ptr<CHTLJSASTNode> parent_selector;
    std::vector<std::shared_ptr<CHTLJSASTNode>> target_selectors;
    std::unordered_map<std::string, std::shared_ptr<CHTLJSASTNode>> handlers;

    DelegateNode(std::shared_ptr<CHTLJSASTNode> parent,
                 const std::vector<std::shared_ptr<CHTLJSASTNode>>& targets)
        : CHTLJSASTNode(CHTLJSASTNodeType::DELEGATE_EXPRESSION),
          parent_selector(parent), target_selectors(targets) {}

    std::string to_string() const override;
    std::string to_js() const override;
};

// 标识符节点
class IdentifierNode : public CHTLJSASTNode {
public:
    std::string name;
    
    IdentifierNode(const std::string& n, size_t l = 1, size_t c = 1) 
        : CHTLJSASTNode(CHTLJSASTNodeType::IDENTIFIER, l, c), name(n) {}
    
    std::string to_string() const override;
    std::string to_js() const override;
};

// 字面量节点
class LiteralNode : public CHTLJSASTNode {
public:
    std::string value;
    CHTLJSTokenType literal_type;
    
    LiteralNode(const std::string& v, CHTLJSTokenType t, size_t l = 1, size_t c = 1) 
        : CHTLJSASTNode(CHTLJSASTNodeType::LITERAL, l, c), value(v), literal_type(t) {}
    
    std::string to_string() const override;
    std::string to_js() const override;
};

// 二元表达式节点
class BinaryExpressionNode : public CHTLJSASTNode {
public:
    std::shared_ptr<CHTLJSASTNode> left;
    CHTLJSTokenType operator_type;
    std::shared_ptr<CHTLJSASTNode> right;
    
    BinaryExpressionNode(std::shared_ptr<CHTLJSASTNode> l, CHTLJSTokenType op, 
                        std::shared_ptr<CHTLJSASTNode> r, size_t line = 1, size_t col = 1) 
        : CHTLJSASTNode(CHTLJSASTNodeType::BINARY_EXPRESSION, line, col), 
          left(l), operator_type(op), right(r) {}
    
    std::string to_string() const override;
    std::string to_js() const override;
};

// 调用表达式节点
class CallExpressionNode : public CHTLJSASTNode {
public:
    std::shared_ptr<CHTLJSASTNode> callee;
    std::vector<std::shared_ptr<CHTLJSASTNode>> arguments;
    
    CallExpressionNode(std::shared_ptr<CHTLJSASTNode> c, 
                      const std::vector<std::shared_ptr<CHTLJSASTNode>>& args = {},
                      size_t l = 1, size_t col = 1) 
        : CHTLJSASTNode(CHTLJSASTNodeType::CALL_EXPRESSION, l, col), 
          callee(c), arguments(args) {}
    
    std::string to_string() const override;
    std::string to_js() const override;
};

// 成员表达式节点
class MemberExpressionNode : public CHTLJSASTNode {
public:
    std::shared_ptr<CHTLJSASTNode> object;
    std::shared_ptr<CHTLJSASTNode> property;
    bool computed;
    
    MemberExpressionNode(std::shared_ptr<CHTLJSASTNode> obj, 
                        std::shared_ptr<CHTLJSASTNode> prop, 
                        bool comp = false, size_t l = 1, size_t col = 1) 
        : CHTLJSASTNode(CHTLJSASTNodeType::MEMBER_EXPRESSION, l, col), 
          object(obj), property(prop), computed(comp) {}
    
    std::string to_string() const override;
    std::string to_js() const override;
};

// 变量声明节点
class VariableDeclarationNode : public CHTLJSASTNode {
public:
    CHTLJSTokenType kind; // var, let, const
    std::vector<std::shared_ptr<CHTLJSASTNode>> declarations;
    
    VariableDeclarationNode(CHTLJSTokenType k, 
                           const std::vector<std::shared_ptr<CHTLJSASTNode>>& decls = {},
                           size_t l = 1, size_t col = 1) 
        : CHTLJSASTNode(CHTLJSASTNodeType::VARIABLE_DECLARATION, l, col), 
          kind(k), declarations(decls) {}
    
    std::string to_string() const override;
    std::string to_js() const override;
};

// 变量声明器节点
class VariableDeclaratorNode : public CHTLJSASTNode {
public:
    std::shared_ptr<CHTLJSASTNode> id;
    std::shared_ptr<CHTLJSASTNode> init;
    
    VariableDeclaratorNode(std::shared_ptr<CHTLJSASTNode> identifier, 
                          std::shared_ptr<CHTLJSASTNode> initializer = nullptr,
                          size_t l = 1, size_t col = 1) 
        : CHTLJSASTNode(CHTLJSASTNodeType::EXPRESSION, l, col), 
          id(identifier), init(initializer) {}
    
    std::string to_string() const override;
    std::string to_js() const override;
};

// 函数声明节点
class FunctionDeclarationNode : public CHTLJSASTNode {
public:
    std::shared_ptr<CHTLJSASTNode> id;
    std::vector<std::shared_ptr<CHTLJSASTNode>> params;
    std::shared_ptr<CHTLJSASTNode> body;
    bool async;
    
    FunctionDeclarationNode(std::shared_ptr<CHTLJSASTNode> identifier,
                           const std::vector<std::shared_ptr<CHTLJSASTNode>>& parameters = {},
                           std::shared_ptr<CHTLJSASTNode> body_node = nullptr,
                           bool is_async = false, size_t l = 1, size_t col = 1) 
        : CHTLJSASTNode(CHTLJSASTNodeType::FUNCTION_DECLARATION, l, col), 
          id(identifier), params(parameters), body(body_node), async(is_async) {}
    
    std::string to_string() const override;
    std::string to_js() const override;
};

// 块语句节点
class BlockStatementNode : public CHTLJSASTNode {
public:
    std::vector<std::shared_ptr<CHTLJSASTNode>> body;
    
    BlockStatementNode(const std::vector<std::shared_ptr<CHTLJSASTNode>>& statements = {},
                      size_t l = 1, size_t col = 1) 
        : CHTLJSASTNode(CHTLJSASTNodeType::BLOCK_STATEMENT, l, col), body(statements) {}
    
    std::string to_string() const override;
    std::string to_js() const override;
};

// CHTL JS特有语法节点

// 文件载入节点
class FileloaderNode : public CHTLJSASTNode {
public:
    std::vector<std::string> load_files;
    
    FileloaderNode(const std::vector<std::string>& files = {}, size_t l = 1, size_t col = 1) 
        : CHTLJSASTNode(CHTLJSASTNodeType::FILELOADER_EXPRESSION, l, col), load_files(files) {}
    
    std::string to_string() const override;
    std::string to_js() const override;
};

// 监听器节点
class ListenNode : public CHTLJSASTNode {
public:
    std::string event;
    std::shared_ptr<CHTLJSASTNode> selector;
    std::shared_ptr<CHTLJSASTNode> handler;
    
    ListenNode(const std::string& evt, std::shared_ptr<CHTLJSASTNode> sel, 
              std::shared_ptr<CHTLJSASTNode> hdl, size_t l = 1, size_t col = 1) 
        : CHTLJSASTNode(CHTLJSASTNodeType::LISTEN_EXPRESSION, l, col), 
          event(evt), selector(sel), handler(hdl) {}
    
    std::string to_string() const override;
    std::string to_js() const override;
};

// 动画节点
class AnimateNode : public CHTLJSASTNode {
public:
    std::shared_ptr<CHTLJSASTNode> target;
    int duration;
    std::string easing;
    std::shared_ptr<CHTLJSASTNode> begin;
    std::vector<std::shared_ptr<CHTLJSASTNode>> when;
    std::shared_ptr<CHTLJSASTNode> end;
    int loop;
    std::string direction;
    int delay;
    std::shared_ptr<CHTLJSASTNode> callback;
    
    AnimateNode(std::shared_ptr<CHTLJSASTNode> tgt, int dur = 100, 
               const std::string& ease = "ease-in-out", size_t l = 1, size_t col = 1) 
        : CHTLJSASTNode(CHTLJSASTNodeType::ANIMATE_EXPRESSION, l, col), 
          target(tgt), duration(dur), easing(ease), loop(-1), delay(0) {}
    
    std::string to_string() const override;
    std::string to_js() const override;
};

// 虚对象节点
class VirNode : public CHTLJSASTNode {
public:
    std::string name;
    std::shared_ptr<CHTLJSASTNode> value;
    
    VirNode(const std::string& n, std::shared_ptr<CHTLJSASTNode> val, 
           size_t l = 1, size_t col = 1) 
        : CHTLJSASTNode(CHTLJSASTNodeType::VIR_DECLARATION, l, col), name(n), value(val) {}
    
    std::string to_string() const override;
    std::string to_js() const override;
};

// 选择器节点
class SelectorNode : public CHTLJSASTNode {
public:
    std::string selector;
    bool is_self_ref;
    
    SelectorNode(const std::string& sel, bool self_ref = false, size_t l = 1, size_t col = 1) 
        : CHTLJSASTNode(CHTLJSASTNodeType::SELECTOR_EXPRESSION, l, col), 
          selector(sel), is_self_ref(self_ref) {}
    
    std::string to_string() const override;
    std::string to_js() const override;
};

// 路由节点
class RouterNode : public CHTLJSASTNode {
public:
    std::string path;
    std::shared_ptr<CHTLJSASTNode> handler;
    
    RouterNode(const std::string& p, std::shared_ptr<CHTLJSASTNode> hdl, 
              size_t l = 1, size_t col = 1) 
        : CHTLJSASTNode(CHTLJSASTNodeType::ROUTER_EXPRESSION, l, col), 
          path(p), handler(hdl) {}
    
    std::string to_string() const override;
    std::string to_js() const override;
};

// iNeverAway节点
class INeverAwayNode : public CHTLJSASTNode {
public:
    std::unordered_map<std::string, std::shared_ptr<CHTLJSASTNode>> functions;
    
    INeverAwayNode(const std::unordered_map<std::string, std::shared_ptr<CHTLJSASTNode>>& funcs = {},
                  size_t l = 1, size_t col = 1) 
        : CHTLJSASTNode(CHTLJSASTNodeType::INEVERAWAY_EXPRESSION, l, col), functions(funcs) {}
    
    std::string to_string() const override;
    std::string to_js() const override;
};

// util表达式节点
class UtilNode : public CHTLJSASTNode {
public:
    std::shared_ptr<CHTLJSASTNode> condition;
    std::shared_ptr<CHTLJSASTNode> change;
    std::shared_ptr<CHTLJSASTNode> then;
    
    UtilNode(std::shared_ptr<CHTLJSASTNode> cond, std::shared_ptr<CHTLJSASTNode> chg, 
            std::shared_ptr<CHTLJSASTNode> thn, size_t l = 1, size_t col = 1) 
        : CHTLJSASTNode(CHTLJSASTNodeType::UTIL_EXPRESSION, l, col), 
          condition(cond), change(chg), then(thn) {}
    
    std::string to_string() const override;
    std::string to_js() const override;
};

} // namespace chtl_js
} // namespace chtl