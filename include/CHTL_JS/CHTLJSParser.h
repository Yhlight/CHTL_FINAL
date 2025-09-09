#pragma once

#include "CHTLJSNode.h"
#include "CHTLJSLexer.h"
#include "CHTLJSVisitor.h"
#include <memory>

namespace CHTL_JS {

// 语法分析器
class CHTLJSParser {
public:
    CHTLJSParser(std::istream& input);
    ~CHTLJSParser() = default;
    
    // 解析整个文档
    std::shared_ptr<CHTLJSNode> parse();
    
    // 解析特定类型的节点
    std::shared_ptr<IdentifierNode> parseIdentifier();
    std::shared_ptr<LiteralNode> parseLiteral();
    std::shared_ptr<ExpressionNode> parseExpression();
    std::shared_ptr<StatementNode> parseStatement();
    std::shared_ptr<BlockNode> parseBlock();
    std::shared_ptr<FunctionNode> parseFunction();
    std::shared_ptr<FunctionCallNode> parseFunctionCall();
    std::shared_ptr<ArrowFunctionNode> parseArrowFunction();
    std::shared_ptr<ObjectNode> parseObject();
    std::shared_ptr<ArrayNode> parseArray();
    
    // 解析 CHTL JS 特定节点
    std::shared_ptr<VirtualObjectNode> parseVirtualObject();
    std::shared_ptr<ListenNode> parseListen();
    std::shared_ptr<DelegateNode> parseDelegate();
    std::shared_ptr<AnimateNode> parseAnimate();
    std::shared_ptr<RouterNode> parseRouter();
    std::shared_ptr<FileLoaderNode> parseFileLoader();
    std::shared_ptr<UtilNode> parseUtil();
    std::shared_ptr<PrintNode> parsePrint();
    std::shared_ptr<INeverAwayNode> parseINeverAway();
    
    // 获取错误列表
    const ErrorList& getErrors() const;

private:
    CHTLJSLexer lexer_;
    ErrorList errors_;
    
    // 当前解析的根节点
    std::shared_ptr<CHTLJSNode> rootNode_;
    
    // 内部解析方法
    std::shared_ptr<CHTLJSNode> parseNode();
    std::shared_ptr<CHTLJSNode> parsePrimaryExpression();
    std::shared_ptr<CHTLJSNode> parseBinaryExpression();
    std::shared_ptr<CHTLJSNode> parseUnaryExpression();
    std::shared_ptr<CHTLJSNode> parseTernaryExpression();
    std::shared_ptr<CHTLJSNode> parseAssignmentExpression();
    
    // 控制流解析
    std::shared_ptr<IfStatementNode> parseIfStatement();
    std::shared_ptr<ForStatementNode> parseForStatement();
    std::shared_ptr<WhileStatementNode> parseWhileStatement();
    std::shared_ptr<SwitchStatementNode> parseSwitchStatement();
    std::shared_ptr<CaseStatementNode> parseCaseStatement();
    std::shared_ptr<BreakStatementNode> parseBreakStatement();
    std::shared_ptr<ContinueStatementNode> parseContinueStatement();
    std::shared_ptr<ReturnStatementNode> parseReturnStatement();
    
    // 变量解析
    std::shared_ptr<VariableDeclarationNode> parseVariableDeclaration();
    std::shared_ptr<VariableAssignmentNode> parseVariableAssignment();
    
    // 操作符解析
    std::shared_ptr<OperatorNode> parseOperator();
    
    // 工具方法
    bool match(CHTLJSTokenType type);
    bool match(const String& keyword);
    bool matchOperator(const String& op);
    CHTLJSToken consume(CHTLJSTokenType type);
    CHTLJSToken consume(const String& keyword);
    CHTLJSToken consumeOperator(const String& op);
    CHTLJSToken expect(CHTLJSTokenType type);
    CHTLJSToken expect(const String& keyword);
    CHTLJSToken expectOperator(const String& op);
    
    // 错误处理
    void addError(const String& message, const Position& pos);
    void addError(const String& message, const CHTLJSToken& token);
    
    // 恢复方法
    void synchronize();
    void skipToBlockEnd();
    void skipToStatementEnd();
    
    // 预读方法
    CHTLJSToken peek();
    CHTLJSToken peekNext();
    bool isAtEnd();
    
    // 语法检查
    bool isValidIdentifier(const String& name) const;
    bool isValidFunctionName(const String& name) const;
    bool isValidVariableName(const String& name) const;
    
    // 字符串处理
    String unescapeString(const String& str) const;
    String escapeString(const String& str) const;
    
    // 数字处理
    double parseNumber(const String& str) const;
    bool isValidNumber(const String& str) const;
    
    // 选择器解析
    Selector parseSelector(const String& selector) const;
    StringList parseSelectorList(const String& selectors) const;
    
    // 事件解析
    EventType parseEventType(const String& event) const;
    StringList parseEventList(const String& events) const;
    
    // 动画解析
    EasingFunction parseEasingFunction(const String& easing) const;
    AnimationDirection parseAnimationDirection(const String& direction) const;
    
    // 配置解析
    VirtualObjectConfig parseVirtualObjectConfig();
    ListenerConfig parseListenerConfig();
    DelegateConfig parseDelegateConfig();
    AnimationConfig parseAnimationConfig();
    RouteConfig parseRouteConfig();
    FileLoaderConfig parseFileLoaderConfig();
    UtilConfig parseUtilConfig();
    PrintConfig parsePrintConfig();
    INeverAwayConfig parseINeverAwayConfig();
};

} // namespace CHTL_JS