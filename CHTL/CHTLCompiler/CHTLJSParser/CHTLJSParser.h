#ifndef CHTL_JS_PARSER_H
#define CHTL_JS_PARSER_H

#include <string>
#include <vector>
#include <memory>
#include "CHTLJSLexer/CHTLJSLexer.h"

namespace CHTL {

// CHTL JS AST node types
enum class CHTLJSNodeType {
    Program,
    Expression,
    Statement,
    Declaration,
    Function,
    Variable,
    Identifier,
    Literal,
    Binary,
    Unary,
    Call,
    Member,
    Array,
    Object,
    Property,
    Block,
    If,
    For,
    While,
    Return,
    Assignment,
    CHTLJSFunction,
    VirtualObject,
    Router,
    Animation,
    EventDelegation,
    EnhancedSelector,
    EnhancedListener,
    FileLoader,
    LocalScript
};

// CHTL JS AST node
class CHTLJSNode {
public:
    CHTLJSNodeType type;
    std::string value;
    size_t line;
    size_t column;
    std::vector<std::shared_ptr<CHTLJSNode>> children;
    
    CHTLJSNode(CHTLJSNodeType t, const std::string& v = "", size_t l = 0, size_t c = 0);
    virtual ~CHTLJSNode() = default;
    
    void addChild(std::shared_ptr<CHTLJSNode> child);
    std::string toString() const;
};

// CHTL JS parser
class CHTLJSParser {
public:
    CHTLJSParser(const std::vector<CHTLJSToken>& tokens);
    ~CHTLJSParser();
    
    // Main parsing function
    std::shared_ptr<CHTLJSNode> parse();
    
    // Expression parsing
    std::shared_ptr<CHTLJSNode> parseExpression();
    std::shared_ptr<CHTLJSNode> parsePrimary();
    std::shared_ptr<CHTLJSNode> parseBinary();
    std::shared_ptr<CHTLJSNode> parseUnary();
    std::shared_ptr<CHTLJSNode> parseCall();
    std::shared_ptr<CHTLJSNode> parseMember();
    std::shared_ptr<CHTLJSNode> parseArray();
    std::shared_ptr<CHTLJSNode> parseObject();
    
    // Statement parsing
    std::shared_ptr<CHTLJSNode> parseStatement();
    std::shared_ptr<CHTLJSNode> parseDeclaration();
    std::shared_ptr<CHTLJSNode> parseBlock();
    std::shared_ptr<CHTLJSNode> parseIf();
    std::shared_ptr<CHTLJSNode> parseFor();
    std::shared_ptr<CHTLJSNode> parseWhile();
    std::shared_ptr<CHTLJSNode> parseReturn();
    std::shared_ptr<CHTLJSNode> parseAssignment();
    
    // CHTL JS specific parsing
    std::shared_ptr<CHTLJSNode> parseCHTLJSFunction();
    std::shared_ptr<CHTLJSNode> parseVirtualObject();
    std::shared_ptr<CHTLJSNode> parseRouter();
    std::shared_ptr<CHTLJSNode> parseAnimation();
    std::shared_ptr<CHTLJSNode> parseEventDelegation();
    std::shared_ptr<CHTLJSNode> parseEnhancedSelector();
    std::shared_ptr<CHTLJSNode> parseEnhancedListener();
    std::shared_ptr<CHTLJSNode> parseFileLoader();
    std::shared_ptr<CHTLJSNode> parseLocalScript();
    
    // Utility functions
    bool isAtEnd() const;
    CHTLJSToken currentToken() const;
    CHTLJSToken peekToken() const;
    CHTLJSToken advance();
    bool match(CHTLJSTokenType type);
    bool check(CHTLJSTokenType type) const;
    CHTLJSToken consume(CHTLJSTokenType type, const std::string& message);
    
    // Error handling
    void error(const std::string& message);
    bool hasError() const;
    std::string getError() const;
    
private:
    std::vector<CHTLJSToken> tokens;
    size_t current;
    std::string error_message;
    bool has_error;
    
    // Helper methods
    std::shared_ptr<CHTLJSNode> parseLiteral();
    std::shared_ptr<CHTLJSNode> parseIdentifier();
    std::shared_ptr<CHTLJSNode> parseFunction();
    std::shared_ptr<CHTLJSNode> parseVariable();
    std::shared_ptr<CHTLJSNode> parseProperty();
    std::shared_ptr<CHTLJSNode> parseParameters();
    std::shared_ptr<CHTLJSNode> parseArguments();
    std::shared_ptr<CHTLJSNode> parseCondition();
    std::shared_ptr<CHTLJSNode> parseBody();
    std::shared_ptr<CHTLJSNode> parseInitializer();
    std::shared_ptr<CHTLJSNode> parseUpdate();
    
    // Precedence handling
    int getPrecedence(CHTLJSTokenType type) const;
    bool isBinaryOperator(CHTLJSTokenType type) const;
    bool isUnaryOperator(CHTLJSTokenType type) const;
    bool isAssignmentOperator(CHTLJSTokenType type) const;
    
    // CHTL JS specific helpers
    bool isCHTLJSKeyword(CHTLJSTokenType type) const;
    std::shared_ptr<CHTLJSNode> parseCHTLJSBlock();
    std::shared_ptr<CHTLJSNode> parseCHTLJSExpression();
    std::shared_ptr<CHTLJSNode> parseCHTLJSParameters();
    std::shared_ptr<CHTLJSNode> parseCHTLJSBody();
};

} // namespace CHTL

#endif // CHTL_JS_PARSER_H