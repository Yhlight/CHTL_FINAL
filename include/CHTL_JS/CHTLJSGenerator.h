#pragma once

#include "CHTLJSNode.h"
#include "CHTLJSVisitor.h"
#include <ostream>
#include <unordered_map>

namespace CHTL_JS {

// 代码生成器基类
class CHTLJSGenerator : public CHTLJSVisitor {
public:
    CHTLJSGenerator(std::ostream& output);
    virtual ~CHTLJSGenerator() = default;
    
    // 生成代码
    virtual void generate(std::shared_ptr<CHTLJSNode> root) = 0;
    
    // 获取错误列表
    const ErrorList& getErrors() const;

protected:
    std::ostream& output_;
    ErrorList errors_;
    
    // 缩进管理
    int indentLevel_ = 0;
    String indentString_ = "    ";
    
    void increaseIndent();
    void decreaseIndent();
    void writeIndent();
    
    // 错误处理
    void addError(const String& message, const Position& pos);
    
    // 工具方法
    String escapeJs(const String& str) const;
    String formatJsExpression(const String& expr) const;
    String formatJsFunction(const String& name, const String& params, const String& body) const;
    String formatJsObject(const String& obj) const;
    String formatJsArray(const String& arr) const;
    
    // 选择器处理
    String processSelector(const Selector& selector) const;
    String processSelectorList(const StringList& selectors) const;
    
    // 事件处理
    String processEventType(EventType event) const;
    String processEventList(const StringList& events) const;
    
    // 动画处理
    String processEasingFunction(EasingFunction easing) const;
    String processAnimationDirection(AnimationDirection direction) const;
    
    // 配置处理
    String processVirtualObjectConfig(const VirtualObjectConfig& config) const;
    String processListenerConfig(const ListenerConfig& config) const;
    String processDelegateConfig(const DelegateConfig& config) const;
    String processAnimationConfig(const AnimationConfig& config) const;
    String processRouteConfig(const RouteConfig& config) const;
    String processFileLoaderConfig(const FileLoaderConfig& config) const;
    String processUtilConfig(const UtilConfig& config) const;
    String processPrintConfig(const PrintConfig& config) const;
    String processINeverAwayConfig(const INeverAwayConfig& config) const;
};

// JavaScript 生成器
class JSGenerator : public CHTLJSGenerator {
public:
    JSGenerator(std::ostream& output);
    ~JSGenerator() = default;
    
    void generate(std::shared_ptr<CHTLJSNode> root) override;
    
    // 节点访问
    void visit(IdentifierNode& node) override;
    void visit(LiteralNode& node) override;
    void visit(ExpressionNode& node) override;
    void visit(StatementNode& node) override;
    void visit(BlockNode& node) override;
    void visit(FunctionNode& node) override;
    void visit(FunctionCallNode& node) override;
    void visit(ArrowFunctionNode& node) override;
    void visit(ObjectNode& node) override;
    void visit(ObjectPropertyNode& node) override;
    void visit(ArrayNode& node) override;
    void visit(ArrayElementNode& node) override;
    void visit(IfStatementNode& node) override;
    void visit(ForStatementNode& node) override;
    void visit(WhileStatementNode& node) override;
    void visit(SwitchStatementNode& node) override;
    void visit(CaseStatementNode& node) override;
    void visit(BreakStatementNode& node) override;
    void visit(ContinueStatementNode& node) override;
    void visit(ReturnStatementNode& node) override;
    void visit(VariableDeclarationNode& node) override;
    void visit(VariableAssignmentNode& node) override;
    void visit(OperatorNode& node) override;
    void visit(VirtualObjectNode& node) override;
    void visit(ListenNode& node) override;
    void visit(DelegateNode& node) override;
    void visit(AnimateNode& node) override;
    void visit(RouterNode& node) override;
    void visit(FileLoaderNode& node) override;
    void visit(UtilNode& node) override;
    void visit(PrintNode& node) override;
    void visit(INeverAwayNode& node) override;

private:
    void generateIdentifier(IdentifierNode& node);
    void generateLiteral(LiteralNode& node);
    void generateExpression(ExpressionNode& node);
    void generateStatement(StatementNode& node);
    void generateBlock(BlockNode& node);
    void generateFunction(FunctionNode& node);
    void generateFunctionCall(FunctionCallNode& node);
    void generateArrowFunction(ArrowFunctionNode& node);
    void generateObject(ObjectNode& node);
    void generateArray(ArrayNode& node);
    void generateIfStatement(IfStatementNode& node);
    void generateForStatement(ForStatementNode& node);
    void generateWhileStatement(WhileStatementNode& node);
    void generateSwitchStatement(SwitchStatementNode& node);
    void generateCaseStatement(CaseStatementNode& node);
    void generateBreakStatement(BreakStatementNode& node);
    void generateContinueStatement(ContinueStatementNode& node);
    void generateReturnStatement(ReturnStatementNode& node);
    void generateVariableDeclaration(VariableDeclarationNode& node);
    void generateVariableAssignment(VariableAssignmentNode& node);
    void generateOperator(OperatorNode& node);
    
    // CHTL JS 特定生成
    void generateVirtualObject(VirtualObjectNode& node);
    void generateListen(ListenNode& node);
    void generateDelegate(DelegateNode& node);
    void generateAnimate(AnimateNode& node);
    void generateRouter(RouterNode& node);
    void generateFileLoader(FileLoaderNode& node);
    void generateUtil(UtilNode& node);
    void generatePrint(PrintNode& node);
    void generateINeverAway(INeverAwayNode& node);
    
    // 工具方法
    String formatOperator(OperatorType op) const;
    String formatLiteralValue(const LiteralValue& value) const;
    String formatFunctionParameters(const StringList& params) const;
    String formatFunctionArguments(const std::vector<std::shared_ptr<CHTLJSNode>>& args) const;
    String formatObjectProperties(const std::vector<std::shared_ptr<CHTLJSNode>>& properties) const;
    String formatArrayElements(const std::vector<std::shared_ptr<CHTLJSNode>>& elements) const;
    
    // 选择器处理
    String processSelector(const Selector& selector) const;
    String processClassSelector(const String& className) const;
    String processIdSelector(const String& id) const;
    String processTagSelector(const String& tagName) const;
    String processDescendantSelector(const String& parent, const String& child) const;
    String processIndexSelector(const String& tagName, int index) const;
    
    // 事件处理
    String processEventType(EventType event) const;
    String processEventList(const StringList& events) const;
    
    // 动画处理
    String processEasingFunction(EasingFunction easing) const;
    String processAnimationDirection(AnimationDirection direction) const;
    String processAnimationKeyframes(const std::vector<AnimationKeyframe>& keyframes) const;
    
    // 配置处理
    String processVirtualObjectConfig(const VirtualObjectConfig& config) const;
    String processListenerConfig(const ListenerConfig& config) const;
    String processDelegateConfig(const DelegateConfig& config) const;
    String processAnimationConfig(const AnimationConfig& config) const;
    String processRouteConfig(const RouteConfig& config) const;
    String processFileLoaderConfig(const FileLoaderConfig& config) const;
    String processUtilConfig(const UtilConfig& config) const;
    String processPrintConfig(const PrintConfig& config) const;
    String processINeverAwayConfig(const INeverAwayConfig& config) const;
    
    // 状态管理
    std::unordered_map<String, String> variables_;
    std::unordered_map<String, std::shared_ptr<CHTLJSNode>> functions_;
    std::unordered_map<String, std::shared_ptr<CHTLJSNode>> objects_;
    std::unordered_map<String, std::shared_ptr<CHTLJSNode>> arrays_;
};

} // namespace CHTL_JS