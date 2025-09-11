# Generated from grammars/CSSValueExpression.g4 by ANTLR 4.13.2
from antlr4 import *
if "." in __name__:
    from .CSSValueExpressionParser import CSSValueExpressionParser
else:
    from CSSValueExpressionParser import CSSValueExpressionParser

# This class defines a complete generic visitor for a parse tree produced by CSSValueExpressionParser.

class CSSValueExpressionVisitor(ParseTreeVisitor):

    # Visit a parse tree produced by CSSValueExpressionParser#parse.
    def visitParse(self, ctx:CSSValueExpressionParser.ParseContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CSSValueExpressionParser#expression.
    def visitExpression(self, ctx:CSSValueExpressionParser.ExpressionContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CSSValueExpressionParser#logicalOrExpression.
    def visitLogicalOrExpression(self, ctx:CSSValueExpressionParser.LogicalOrExpressionContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CSSValueExpressionParser#logicalAndExpression.
    def visitLogicalAndExpression(self, ctx:CSSValueExpressionParser.LogicalAndExpressionContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CSSValueExpressionParser#equalityExpression.
    def visitEqualityExpression(self, ctx:CSSValueExpressionParser.EqualityExpressionContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CSSValueExpressionParser#relationalExpression.
    def visitRelationalExpression(self, ctx:CSSValueExpressionParser.RelationalExpressionContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CSSValueExpressionParser#additiveExpression.
    def visitAdditiveExpression(self, ctx:CSSValueExpressionParser.AdditiveExpressionContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CSSValueExpressionParser#multiplicativeExpression.
    def visitMultiplicativeExpression(self, ctx:CSSValueExpressionParser.MultiplicativeExpressionContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CSSValueExpressionParser#powerExpression.
    def visitPowerExpression(self, ctx:CSSValueExpressionParser.PowerExpressionContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CSSValueExpressionParser#unaryExpression.
    def visitUnaryExpression(self, ctx:CSSValueExpressionParser.UnaryExpressionContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CSSValueExpressionParser#atom.
    def visitAtom(self, ctx:CSSValueExpressionParser.AtomContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CSSValueExpressionParser#literal.
    def visitLiteral(self, ctx:CSSValueExpressionParser.LiteralContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CSSValueExpressionParser#propertyReference.
    def visitPropertyReference(self, ctx:CSSValueExpressionParser.PropertyReferenceContext):
        return self.visitChildren(ctx)



del CSSValueExpressionParser