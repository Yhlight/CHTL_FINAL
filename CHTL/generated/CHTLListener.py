# Generated from grammars/CHTL.g4 by ANTLR 4.13.2
from antlr4 import *
if "." in __name__:
    from .CHTLParser import CHTLParser
else:
    from CHTLParser import CHTLParser

# This class defines a complete listener for a parse tree produced by CHTLParser.
class CHTLListener(ParseTreeListener):

    # Enter a parse tree produced by CHTLParser#document.
    def enterDocument(self, ctx:CHTLParser.DocumentContext):
        pass

    # Exit a parse tree produced by CHTLParser#document.
    def exitDocument(self, ctx:CHTLParser.DocumentContext):
        pass


    # Enter a parse tree produced by CHTLParser#element.
    def enterElement(self, ctx:CHTLParser.ElementContext):
        pass

    # Exit a parse tree produced by CHTLParser#element.
    def exitElement(self, ctx:CHTLParser.ElementContext):
        pass


    # Enter a parse tree produced by CHTLParser#attribute.
    def enterAttribute(self, ctx:CHTLParser.AttributeContext):
        pass

    # Exit a parse tree produced by CHTLParser#attribute.
    def exitAttribute(self, ctx:CHTLParser.AttributeContext):
        pass


    # Enter a parse tree produced by CHTLParser#textNode.
    def enterTextNode(self, ctx:CHTLParser.TextNodeContext):
        pass

    # Exit a parse tree produced by CHTLParser#textNode.
    def exitTextNode(self, ctx:CHTLParser.TextNodeContext):
        pass


    # Enter a parse tree produced by CHTLParser#value.
    def enterValue(self, ctx:CHTLParser.ValueContext):
        pass

    # Exit a parse tree produced by CHTLParser#value.
    def exitValue(self, ctx:CHTLParser.ValueContext):
        pass



del CHTLParser