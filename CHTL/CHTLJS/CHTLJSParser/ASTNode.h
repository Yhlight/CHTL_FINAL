#ifndef CHTLJS_AST_NODE_H
#define CHTLJS_AST_NODE_H

#include "Token.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTLJS {

enum class ASTNodeType {
    // 基础节点
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
    ARRAY_EXPRESSION,
    OBJECT_EXPRESSION,
    CONDITIONAL_EXPRESSION,
    
    // 语句
    EXPRESSION_STATEMENT,
    BLOCK_STATEMENT,
    IF_STATEMENT,
    WHILE_STATEMENT,
    FOR_STATEMENT,
    RETURN_STATEMENT,
    BREAK_STATEMENT,
    CONTINUE_STATEMENT,
    TRY_STATEMENT,
    THROW_STATEMENT,
    
    // CHTL JS 特有节点
    VIR_DECLARATION,
    LISTEN_EXPRESSION,
    ANIMATE_EXPRESSION,
    ROUTER_EXPRESSION,
    SCRIPT_LOADER_EXPRESSION,
    DELEGATE_EXPRESSION,
    SELECTOR_EXPRESSION,
    REACTIVE_VALUE_EXPRESSION,
    
    // 属性
    PROPERTY,
    PARAMETER,
    
    // 其他
    COMMENT,
    UNKNOWN
};

class ASTNode {
public:
    ASTNodeType type;
    Token token;
    std::vector<std::shared_ptr<ASTNode>> children;
    std::unordered_map<std::string, std::shared_ptr<ASTNode>> properties;
    
    ASTNode(ASTNodeType t = ASTNodeType::UNKNOWN, const Token& tok = Token())
        : type(t), token(tok) {}
    
    virtual ~ASTNode() = default;
    
    virtual std::string toString() const;
    virtual std::string toCode() const = 0;
    
    void addChild(std::shared_ptr<ASTNode> child);
    void setProperty(const std::string& key, std::shared_ptr<ASTNode> value);
    std::shared_ptr<ASTNode> getProperty(const std::string& key) const;
    
    bool hasChildren() const;
    size_t getChildCount() const;
    std::shared_ptr<ASTNode> getChild(size_t index) const;
    
    bool hasProperty(const std::string& key) const;
    std::vector<std::string> getPropertyKeys() const;
};

// 具体节点类型
class ProgramNode : public ASTNode {
public:
    ProgramNode() : ASTNode(ASTNodeType::PROGRAM) {}
    std::string toCode() const override;
};

class VariableDeclarationNode : public ASTNode {
public:
    std::string variableType; // var, let, const
    std::string name;
    std::shared_ptr<ASTNode> initializer;
    
    VariableDeclarationNode(const std::string& varType, const std::string& varName)
        : ASTNode(ASTNodeType::VARIABLE_DECLARATION), variableType(varType), name(varName) {}
    
    std::string toCode() const override;
};

class FunctionDeclarationNode : public ASTNode {
public:
    std::string name;
    std::vector<std::string> parameters;
    std::shared_ptr<ASTNode> body;
    
    FunctionDeclarationNode(const std::string& funcName) 
        : ASTNode(ASTNodeType::FUNCTION_DECLARATION), name(funcName) {}
    
    std::string toCode() const override;
};

class BinaryExpressionNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> left;
    std::shared_ptr<ASTNode> right;
    TokenType operatorType;
    
    BinaryExpressionNode(std::shared_ptr<ASTNode> l, TokenType op, std::shared_ptr<ASTNode> r)
        : ASTNode(ASTNodeType::BINARY_EXPRESSION), left(l), right(r), operatorType(op) {}
    
    std::string toCode() const override;
};

class CallExpressionNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> callee;
    std::vector<std::shared_ptr<ASTNode>> arguments;
    
    CallExpressionNode(std::shared_ptr<ASTNode> func) 
        : ASTNode(ASTNodeType::CALL_EXPRESSION), callee(func) {}
    
    std::string toCode() const override;
};

class MemberExpressionNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> object;
    std::shared_ptr<ASTNode> property;
    bool computed;
    
    MemberExpressionNode(std::shared_ptr<ASTNode> obj, std::shared_ptr<ASTNode> prop, bool comp = false)
        : ASTNode(ASTNodeType::MEMBER_EXPRESSION), object(obj), property(prop), computed(comp) {}
    
    std::string toCode() const override;
};

class LiteralNode : public ASTNode {
public:
    std::string value;
    TokenType literalType;
    
    LiteralNode(const std::string& val, TokenType type) 
        : ASTNode(ASTNodeType::LITERAL), value(val), literalType(type) {}
    
    std::string toCode() const override;
};

class IdentifierNode : public ASTNode {
public:
    std::string name;
    
    IdentifierNode(const std::string& identifierName) 
        : ASTNode(ASTNodeType::IDENTIFIER), name(identifierName) {}
    
    std::string toCode() const override;
};

// CHTL JS 特有节点
class VirDeclarationNode : public ASTNode {
public:
    std::string name;
    std::shared_ptr<ASTNode> value;
    
    VirDeclarationNode(const std::string& varName) 
        : ASTNode(ASTNodeType::VIR_DECLARATION), name(varName) {}
    
    std::string toCode() const override;
};

class ListenExpressionNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> target;
    std::unordered_map<std::string, std::shared_ptr<ASTNode>> events;
    
    ListenExpressionNode() : ASTNode(ASTNodeType::LISTEN_EXPRESSION) {}
    
    std::string toCode() const override;
};

class AnimateExpressionNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> target;
    std::shared_ptr<ASTNode> duration;
    std::shared_ptr<ASTNode> easing;
    std::shared_ptr<ASTNode> begin;
    std::vector<std::shared_ptr<ASTNode>> when;
    std::shared_ptr<ASTNode> end;
    std::shared_ptr<ASTNode> loop;
    std::shared_ptr<ASTNode> direction;
    std::shared_ptr<ASTNode> delay;
    std::shared_ptr<ASTNode> callback;
    
    AnimateExpressionNode() : ASTNode(ASTNodeType::ANIMATE_EXPRESSION) {}
    
    std::string toCode() const override;
};

class SelectorExpressionNode : public ASTNode {
public:
    std::string selector;
    
    SelectorExpressionNode(const std::string& sel) 
        : ASTNode(ASTNodeType::SELECTOR_EXPRESSION), selector(sel) {}
    
    std::string toCode() const override;
};

class ReactiveValueExpressionNode : public ASTNode {
public:
    std::string variableName;
    
    ReactiveValueExpressionNode(const std::string& varName) 
        : ASTNode(ASTNodeType::REACTIVE_VALUE_EXPRESSION), variableName(varName) {}
    
    std::string toCode() const override;
};

} // namespace CHTLJS

#endif // CHTLJS_AST_NODE_H