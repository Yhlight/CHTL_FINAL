# Generated from grammars/CSSValueExpression.g4 by ANTLR 4.13.2
from antlr4 import *
if "." in __name__:
    from .CSSValueExpressionParser import CSSValueExpressionParser
else:
    from CSSValueExpressionParser import CSSValueExpressionParser

# This class defines a complete listener for a parse tree produced by CSSValueExpressionParser.
class CSSValueExpressionListener(ParseTreeListener):

    # Enter a parse tree produced by CSSValueExpressionParser#parse.
    def enterParse(self, ctx:CSSValueExpressionParser.ParseContext):
        pass

    # Exit a parse tree produced by CSSValueExpressionParser#parse.
    def exitParse(self, ctx:CSSValueExpressionParser.ParseContext):
        pass


    # Enter a parse tree produced by CSSValueExpressionParser#expression.
    def enterExpression(self, ctx:CSSValueExpressionParser.ExpressionContext):
        pass

    # Exit a parse tree produced by CSSValueExpressionParser#expression.
    def exitExpression(self, ctx:CSSValueExpressionParser.ExpressionContext):
        pass


    # Enter a parse tree produced by CSSValueExpressionParser#logicalOrExpression.
    def enterLogicalOrExpression(self, ctx:CSSValueExpressionParser.LogicalOrExpressionContext):
        pass

    # Exit a parse tree produced by CSSValueExpressionParser#logicalOrExpression.
    def exitLogicalOrExpression(self, ctx:CSSValueExpressionParser.LogicalOrExpressionContext):
        pass


    # Enter a parse tree produced by CSSValueExpressionParser#logicalAndExpression.
    def enterLogicalAndExpression(self, ctx:CSSValueExpressionParser.LogicalAndExpressionContext):
        pass

    # Exit a parse tree produced by CSSValueExpressionParser#logicalAndExpression.
    def exitLogicalAndExpression(self, ctx:CSSValueExpressionParser.LogicalAndExpressionContext):
        pass


    # Enter a parse tree produced by CSSValueExpressionParser#equalityExpression.
    def enterEqualityExpression(self, ctx:CSSValueExpressionParser.EqualityExpressionContext):
        pass

    # Exit a parse tree produced by CSSValueExpressionParser#equalityExpression.
    def exitEqualityExpression(self, ctx:CSSValueExpressionParser.EqualityExpressionContext):
        pass


    # Enter a parse tree produced by CSSValueExpressionParser#relationalExpression.
    def enterRelationalExpression(self, ctx:CSSValueExpressionParser.RelationalExpressionContext):
        pass

    # Exit a parse tree produced by CSSValueExpressionParser#relationalExpression.
    def exitRelationalExpression(self, ctx:CSSValueExpressionParser.RelationalExpressionContext):
        pass


    # Enter a parse tree produced by CSSValueExpressionParser#additiveExpression.
    def enterAdditiveExpression(self, ctx:CSSValueExpressionParser.AdditiveExpressionContext):
        pass

    # Exit a parse tree produced by CSSValueExpressionParser#additiveExpression.
    def exitAdditiveExpression(self, ctx:CSSValueExpressionParser.AdditiveExpressionContext):
        pass


    # Enter a parse tree produced by CSSValueExpressionParser#multiplicativeExpression.
    def enterMultiplicativeExpression(self, ctx:CSSValueExpressionParser.MultiplicativeExpressionContext):
        pass

    # Exit a parse tree produced by CSSValueExpressionParser#multiplicativeExpression.
    def exitMultiplicativeExpression(self, ctx:CSSValueExpressionParser.MultiplicativeExpressionContext):
        pass


    # Enter a parse tree produced by CSSValueExpressionParser#powerExpression.
    def enterPowerExpression(self, ctx:CSSValueExpressionParser.PowerExpressionContext):
        pass

    # Exit a parse tree produced by CSSValueExpressionParser#powerExpression.
    def exitPowerExpression(self, ctx:CSSValueExpressionParser.PowerExpressionContext):
        pass


    # Enter a parse tree produced by CSSValueExpressionParser#unaryExpression.
    def enterUnaryExpression(self, ctx:CSSValueExpressionParser.UnaryExpressionContext):
        pass

    # Exit a parse tree produced by CSSValueExpressionParser#unaryExpression.
    def exitUnaryExpression(self, ctx:CSSValueExpressionParser.UnaryExpressionContext):
        pass


    # Enter a parse tree produced by CSSValueExpressionParser#atom.
    def enterAtom(self, ctx:CSSValueExpressionParser.AtomContext):
        pass

    # Exit a parse tree produced by CSSValueExpressionParser#atom.
    def exitAtom(self, ctx:CSSValueExpressionParser.AtomContext):
        pass


    # Enter a parse tree produced by CSSValueExpressionParser#literal.
    def enterLiteral(self, ctx:CSSValueExpressionParser.LiteralContext):
        pass

    # Exit a parse tree produced by CSSValueExpressionParser#literal.
    def exitLiteral(self, ctx:CSSValueExpressionParser.LiteralContext):
        pass


    # Enter a parse tree produced by CSSValueExpressionParser#propertyReference.
    def enterPropertyReference(self, ctx:CSSValueExpressionParser.PropertyReferenceContext):
        pass

    # Exit a parse tree produced by CSSValueExpressionParser#propertyReference.
    def exitPropertyReference(self, ctx:CSSValueExpressionParser.PropertyReferenceContext):
        pass



del CSSValueExpressionParser