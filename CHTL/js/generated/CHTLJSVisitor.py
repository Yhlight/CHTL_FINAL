# Generated from grammars/CHTLJS.g4 by ANTLR 4.13.2
from antlr4 import *
if "." in __name__:
    from .CHTLJSParser import CHTLJSParser
else:
    from CHTLJSParser import CHTLJSParser

# This class defines a complete generic visitor for a parse tree produced by CHTLJSParser.

class CHTLJSVisitor(ParseTreeVisitor):

    # Visit a parse tree produced by CHTLJSParser#program.
    def visitProgram(self, ctx:CHTLJSParser.ProgramContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CHTLJSParser#statement.
    def visitStatement(self, ctx:CHTLJSParser.StatementContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CHTLJSParser#expression.
    def visitExpression(self, ctx:CHTLJSParser.ExpressionContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CHTLJSParser#selector.
    def visitSelector(self, ctx:CHTLJSParser.SelectorContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CHTLJSParser#selectorContent.
    def visitSelectorContent(self, ctx:CHTLJSParser.SelectorContentContext):
        return self.visitChildren(ctx)



del CHTLJSParser