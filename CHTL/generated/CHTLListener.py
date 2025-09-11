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


    # Enter a parse tree produced by CHTLParser#definition.
    def enterDefinition(self, ctx:CHTLParser.DefinitionContext):
        pass

    # Exit a parse tree produced by CHTLParser#definition.
    def exitDefinition(self, ctx:CHTLParser.DefinitionContext):
        pass


    # Enter a parse tree produced by CHTLParser#styleTemplate.
    def enterStyleTemplate(self, ctx:CHTLParser.StyleTemplateContext):
        pass

    # Exit a parse tree produced by CHTLParser#styleTemplate.
    def exitStyleTemplate(self, ctx:CHTLParser.StyleTemplateContext):
        pass


    # Enter a parse tree produced by CHTLParser#elementTemplate.
    def enterElementTemplate(self, ctx:CHTLParser.ElementTemplateContext):
        pass

    # Exit a parse tree produced by CHTLParser#elementTemplate.
    def exitElementTemplate(self, ctx:CHTLParser.ElementTemplateContext):
        pass


    # Enter a parse tree produced by CHTLParser#varTemplate.
    def enterVarTemplate(self, ctx:CHTLParser.VarTemplateContext):
        pass

    # Exit a parse tree produced by CHTLParser#varTemplate.
    def exitVarTemplate(self, ctx:CHTLParser.VarTemplateContext):
        pass


    # Enter a parse tree produced by CHTLParser#element.
    def enterElement(self, ctx:CHTLParser.ElementContext):
        pass

    # Exit a parse tree produced by CHTLParser#element.
    def exitElement(self, ctx:CHTLParser.ElementContext):
        pass


    # Enter a parse tree produced by CHTLParser#elementUsage.
    def enterElementUsage(self, ctx:CHTLParser.ElementUsageContext):
        pass

    # Exit a parse tree produced by CHTLParser#elementUsage.
    def exitElementUsage(self, ctx:CHTLParser.ElementUsageContext):
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


    # Enter a parse tree produced by CHTLParser#stylePlaceholder.
    def enterStylePlaceholder(self, ctx:CHTLParser.StylePlaceholderContext):
        pass

    # Exit a parse tree produced by CHTLParser#stylePlaceholder.
    def exitStylePlaceholder(self, ctx:CHTLParser.StylePlaceholderContext):
        pass


    # Enter a parse tree produced by CHTLParser#styleContent.
    def enterStyleContent(self, ctx:CHTLParser.StyleContentContext):
        pass

    # Exit a parse tree produced by CHTLParser#styleContent.
    def exitStyleContent(self, ctx:CHTLParser.StyleContentContext):
        pass


    # Enter a parse tree produced by CHTLParser#value.
    def enterValue(self, ctx:CHTLParser.ValueContext):
        pass

    # Exit a parse tree produced by CHTLParser#value.
    def exitValue(self, ctx:CHTLParser.ValueContext):
        pass


    # Enter a parse tree produced by CHTLParser#varUsage.
    def enterVarUsage(self, ctx:CHTLParser.VarUsageContext):
        pass

    # Exit a parse tree produced by CHTLParser#varUsage.
    def exitVarUsage(self, ctx:CHTLParser.VarUsageContext):
        pass



del CHTLParser