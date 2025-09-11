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


    # Enter a parse tree produced by CHTLParser#directive.
    def enterDirective(self, ctx:CHTLParser.DirectiveContext):
        pass

    # Exit a parse tree produced by CHTLParser#directive.
    def exitDirective(self, ctx:CHTLParser.DirectiveContext):
        pass


    # Enter a parse tree produced by CHTLParser#importStatement.
    def enterImportStatement(self, ctx:CHTLParser.ImportStatementContext):
        pass

    # Exit a parse tree produced by CHTLParser#importStatement.
    def exitImportStatement(self, ctx:CHTLParser.ImportStatementContext):
        pass


    # Enter a parse tree produced by CHTLParser#namespaceStatement.
    def enterNamespaceStatement(self, ctx:CHTLParser.NamespaceStatementContext):
        pass

    # Exit a parse tree produced by CHTLParser#namespaceStatement.
    def exitNamespaceStatement(self, ctx:CHTLParser.NamespaceStatementContext):
        pass


    # Enter a parse tree produced by CHTLParser#configurationStatement.
    def enterConfigurationStatement(self, ctx:CHTLParser.ConfigurationStatementContext):
        pass

    # Exit a parse tree produced by CHTLParser#configurationStatement.
    def exitConfigurationStatement(self, ctx:CHTLParser.ConfigurationStatementContext):
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


    # Enter a parse tree produced by CHTLParser#specializationBody.
    def enterSpecializationBody(self, ctx:CHTLParser.SpecializationBodyContext):
        pass

    # Exit a parse tree produced by CHTLParser#specializationBody.
    def exitSpecializationBody(self, ctx:CHTLParser.SpecializationBodyContext):
        pass


    # Enter a parse tree produced by CHTLParser#insertStatement.
    def enterInsertStatement(self, ctx:CHTLParser.InsertStatementContext):
        pass

    # Exit a parse tree produced by CHTLParser#insertStatement.
    def exitInsertStatement(self, ctx:CHTLParser.InsertStatementContext):
        pass


    # Enter a parse tree produced by CHTLParser#deleteStatement.
    def enterDeleteStatement(self, ctx:CHTLParser.DeleteStatementContext):
        pass

    # Exit a parse tree produced by CHTLParser#deleteStatement.
    def exitDeleteStatement(self, ctx:CHTLParser.DeleteStatementContext):
        pass


    # Enter a parse tree produced by CHTLParser#insertPosition.
    def enterInsertPosition(self, ctx:CHTLParser.InsertPositionContext):
        pass

    # Exit a parse tree produced by CHTLParser#insertPosition.
    def exitInsertPosition(self, ctx:CHTLParser.InsertPositionContext):
        pass


    # Enter a parse tree produced by CHTLParser#selector.
    def enterSelector(self, ctx:CHTLParser.SelectorContext):
        pass

    # Exit a parse tree produced by CHTLParser#selector.
    def exitSelector(self, ctx:CHTLParser.SelectorContext):
        pass


    # Enter a parse tree produced by CHTLParser#namespacePath.
    def enterNamespacePath(self, ctx:CHTLParser.NamespacePathContext):
        pass

    # Exit a parse tree produced by CHTLParser#namespacePath.
    def exitNamespacePath(self, ctx:CHTLParser.NamespacePathContext):
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


    # Enter a parse tree produced by CHTLParser#scriptPlaceholder.
    def enterScriptPlaceholder(self, ctx:CHTLParser.ScriptPlaceholderContext):
        pass

    # Exit a parse tree produced by CHTLParser#scriptPlaceholder.
    def exitScriptPlaceholder(self, ctx:CHTLParser.ScriptPlaceholderContext):
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


    # Enter a parse tree produced by CHTLParser#valuePart.
    def enterValuePart(self, ctx:CHTLParser.ValuePartContext):
        pass

    # Exit a parse tree produced by CHTLParser#valuePart.
    def exitValuePart(self, ctx:CHTLParser.ValuePartContext):
        pass


    # Enter a parse tree produced by CHTLParser#varUsage.
    def enterVarUsage(self, ctx:CHTLParser.VarUsageContext):
        pass

    # Exit a parse tree produced by CHTLParser#varUsage.
    def exitVarUsage(self, ctx:CHTLParser.VarUsageContext):
        pass



del CHTLParser