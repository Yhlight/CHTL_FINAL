# Generated from grammars/CHTLJS.g4 by ANTLR 4.13.2
from antlr4 import *
if "." in __name__:
    from .CHTLJSParser import CHTLJSParser
else:
    from CHTLJSParser import CHTLJSParser

# This class defines a complete listener for a parse tree produced by CHTLJSParser.
class CHTLJSListener(ParseTreeListener):

    # Enter a parse tree produced by CHTLJSParser#program.
    def enterProgram(self, ctx:CHTLJSParser.ProgramContext):
        pass

    # Exit a parse tree produced by CHTLJSParser#program.
    def exitProgram(self, ctx:CHTLJSParser.ProgramContext):
        pass


    # Enter a parse tree produced by CHTLJSParser#statement.
    def enterStatement(self, ctx:CHTLJSParser.StatementContext):
        pass

    # Exit a parse tree produced by CHTLJSParser#statement.
    def exitStatement(self, ctx:CHTLJSParser.StatementContext):
        pass


    # Enter a parse tree produced by CHTLJSParser#expression.
    def enterExpression(self, ctx:CHTLJSParser.ExpressionContext):
        pass

    # Exit a parse tree produced by CHTLJSParser#expression.
    def exitExpression(self, ctx:CHTLJSParser.ExpressionContext):
        pass


    # Enter a parse tree produced by CHTLJSParser#selector.
    def enterSelector(self, ctx:CHTLJSParser.SelectorContext):
        pass

    # Exit a parse tree produced by CHTLJSParser#selector.
    def exitSelector(self, ctx:CHTLJSParser.SelectorContext):
        pass


    # Enter a parse tree produced by CHTLJSParser#selectorContent.
    def enterSelectorContent(self, ctx:CHTLJSParser.SelectorContentContext):
        pass

    # Exit a parse tree produced by CHTLJSParser#selectorContent.
    def exitSelectorContent(self, ctx:CHTLJSParser.SelectorContentContext):
        pass



del CHTLJSParser