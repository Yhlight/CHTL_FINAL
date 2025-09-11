from CHTL.js.generated.CHTLJSVisitor import CHTLJSVisitor
from CHTL.js.generated.CHTLJSParser import CHTLJSParser
from CHTL.js.ast.nodes import ProgramNode, StatementNode, SelectorNode

class ChtlJsAstBuilder(CHTLJSVisitor):

    def visitProgram(self, ctx:CHTLJSParser.ProgramContext):
        statements = [self.visit(stmt) for stmt in ctx.statement()]
        return ProgramNode(statements=statements)

    def visitStatement(self, ctx:CHTLJSParser.StatementContext):
        expression = self.visit(ctx.expression())
        return StatementNode(expression=expression)

    def visitExpression(self, ctx:CHTLJSParser.ExpressionContext):
        return self.visit(ctx.selector())

    def visitSelector(self, ctx:CHTLJSParser.SelectorContext):
        text = self.visit(ctx.selectorContent())
        return SelectorNode(selector_text=text)

    def visitSelectorContent(self, ctx:CHTLJSParser.SelectorContentContext):
        return "".join([child.getText() for child in ctx.children])
