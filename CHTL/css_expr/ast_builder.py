from .generated.CSSValueExpressionParser import CSSValueExpressionParser
from .generated.CSSValueExpressionVisitor import CSSValueExpressionVisitor
from .nodes import (
    ExprNode, LiteralNode, PropertyRefNode, UnaryOpNode, BinaryOpNode, TernaryOpNode
)

class ExpressionAstBuilder(CSSValueExpressionVisitor):

    def visitParse(self, ctx: CSSValueExpressionParser.ParseContext):
        return self.visit(ctx.expression())

    def visitExpression(self, ctx: CSSValueExpressionParser.ExpressionContext):
        if ctx.getChildCount() == 1:
            return self.visit(ctx.logicalOrExpression())

        condition = self.visit(ctx.logicalOrExpression())
        true_expr = self.visit(ctx.expression(0))
        false_expr = self.visit(ctx.expression(1))
        return TernaryOpNode(condition, true_expr, false_expr)

    def visitLogicalOrExpression(self, ctx: CSSValueExpressionParser.LogicalOrExpressionContext):
        result = self.visit(ctx.logicalAndExpression(0))
        for i in range(1, len(ctx.logicalAndExpression())):
            op = ctx.getChild(2 * i - 1).getText()
            right = self.visit(ctx.logicalAndExpression(i))
            result = BinaryOpNode(result, op, right)
        return result

    def visitLogicalAndExpression(self, ctx: CSSValueExpressionParser.LogicalAndExpressionContext):
        result = self.visit(ctx.equalityExpression(0))
        for i in range(1, len(ctx.equalityExpression())):
            op = ctx.getChild(2 * i - 1).getText()
            right = self.visit(ctx.equalityExpression(i))
            result = BinaryOpNode(result, op, right)
        return result

    def visitEqualityExpression(self, ctx: CSSValueExpressionParser.EqualityExpressionContext):
        result = self.visit(ctx.relationalExpression(0))
        for i in range(1, len(ctx.relationalExpression())):
            op = ctx.getChild(2 * i - 1).getText()
            right = self.visit(ctx.relationalExpression(i))
            result = BinaryOpNode(result, op, right)
        return result

    def visitRelationalExpression(self, ctx: CSSValueExpressionParser.RelationalExpressionContext):
        result = self.visit(ctx.additiveExpression(0))
        for i in range(1, len(ctx.additiveExpression())):
            op = ctx.getChild(2 * i - 1).getText()
            right = self.visit(ctx.additiveExpression(i))
            result = BinaryOpNode(result, op, right)
        return result

    def visitAdditiveExpression(self, ctx: CSSValueExpressionParser.AdditiveExpressionContext):
        result = self.visit(ctx.multiplicativeExpression(0))
        for i in range(1, len(ctx.multiplicativeExpression())):
            op = ctx.getChild(2 * i - 1).getText()
            right = self.visit(ctx.multiplicativeExpression(i))
            result = BinaryOpNode(result, op, right)
        return result

    def visitMultiplicativeExpression(self, ctx: CSSValueExpressionParser.MultiplicativeExpressionContext):
        result = self.visit(ctx.powerExpression(0))
        for i in range(1, len(ctx.powerExpression())):
            op = ctx.getChild(2 * i - 1).getText()
            right = self.visit(ctx.powerExpression(i))
            result = BinaryOpNode(result, op, right)
        return result

    def visitPowerExpression(self, ctx: CSSValueExpressionParser.PowerExpressionContext):
        result = self.visit(ctx.unaryExpression(0))
        for i in range(1, len(ctx.unaryExpression())):
            op = ctx.getChild(2 * i - 1).getText()
            right = self.visit(ctx.unaryExpression(i))
            result = BinaryOpNode(result, op, right)
        return result

    def visitUnaryExpression(self, ctx: CSSValueExpressionParser.UnaryExpressionContext):
        if ctx.atom():
            return self.visit(ctx.atom())

        op = ctx.getChild(0).getText()
        operand = self.visit(ctx.unaryExpression())
        return UnaryOpNode(op, operand)

    def visitAtom(self, ctx: CSSValueExpressionParser.AtomContext):
        if ctx.propertyReference():
            return self.visit(ctx.propertyReference())
        if ctx.literal():
            return self.visit(ctx.literal())
        if ctx.expression():
            return self.visit(ctx.expression())
        return None

    def visitLiteral(self, ctx: CSSValueExpressionParser.LiteralContext):
        if ctx.DIMENSION():
            value = ctx.DIMENSION().getText()
        elif ctx.NUMBER():
            value = ctx.NUMBER().getText()
        elif ctx.STRING():
            value = ctx.STRING().getText()[1:-1]
        elif ctx.COLOR():
            value = ctx.COLOR().getText()
        elif ctx.IDENTIFIER():
            value = ctx.IDENTIFIER().getText()
        else:
            value = ctx.getText()
        return LiteralNode(value)

    def visitPropertyReference(self, ctx: CSSValueExpressionParser.PropertyReferenceContext):
        path = [identifier.getText() for identifier in ctx.IDENTIFIER()]
        return PropertyRefNode(path)
