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


    # Visit a parse tree produced by CHTLParser#definition.
    def visitDefinition(self, ctx:CHTLParser.DefinitionContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CHTLParser#styleTemplate.
    def visitStyleTemplate(self, ctx:CHTLParser.StyleTemplateContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CHTLParser#elementTemplate.
    def visitElementTemplate(self, ctx:CHTLParser.ElementTemplateContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CHTLParser#varTemplate.
    def visitVarTemplate(self, ctx:CHTLParser.VarTemplateContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CHTLParser#element.
    def visitElement(self, ctx:CHTLParser.ElementContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CHTLParser#elementUsage.
    def visitElementUsage(self, ctx:CHTLParser.ElementUsageContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CHTLParser#specializationBody.
    def visitSpecializationBody(self, ctx:CHTLParser.SpecializationBodyContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CHTLParser#insertStatement.
    def visitInsertStatement(self, ctx:CHTLParser.InsertStatementContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CHTLParser#deleteStatement.
    def visitDeleteStatement(self, ctx:CHTLParser.DeleteStatementContext):
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


    # Visit a parse tree produced by CHTLParser#styleContent.
    def visitStyleContent(self, ctx:CHTLParser.StyleContentContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CHTLParser#value.
    def visitValue(self, ctx:CHTLParser.ValueContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CHTLParser#valuePart.
    def visitValuePart(self, ctx:CHTLParser.ValuePartContext):
        return self.visitChildren(ctx)


    # Visit a parse tree produced by CHTLParser#varUsage.
    def visitVarUsage(self, ctx:CHTLParser.VarUsageContext):
        return self.visitChildren(ctx)



del CHTLParser