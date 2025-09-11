# Generated from grammars/CHTL.g4 by ANTLR 4.13.2
from antlr4 import *
if "." in __name__:
    from .CHTLParser import CHTLParser
else:
    from CHTLParser import CHTLParser

# This class defines a complete generic visitor for a parse tree produced by CHTLParser.

class CHTLVisitor(ParseTreeVisitor):

    # Visit a parse tree produced by CHTLParser#document.
    def visitDocument(self, ctx:CHTLParser.DocumentContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CHTLParser#element.
    def visitElement(self, ctx:CHTLParser.ElementContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CHTLParser#attribute.
    def visitAttribute(self, ctx:CHTLParser.AttributeContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CHTLParser#textNode.
    def visitTextNode(self, ctx:CHTLParser.TextNodeContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CHTLParser#stylePlaceholder.
    def visitStylePlaceholder(self, ctx:CHTLParser.StylePlaceholderContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CHTLParser#value.
    def visitValue(self, ctx:CHTLParser.ValueContext):
        return self.visitChildren(ctx)



del CHTLParser