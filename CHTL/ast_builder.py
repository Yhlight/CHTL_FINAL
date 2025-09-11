from typing import Dict, Any
from CHTL.generated.CHTLVisitor import CHTLVisitor
from CHTL.generated.CHTLParser import CHTLParser
from CHTL.ast.nodes import (
    DocumentNode, ElementNode, AttributeNode, TextNode, StyleNode, StyleUsageNode,
    TemplateDefinitionNode, CustomDefinitionNode,
    UsageNode, ElementUsageNode, VarUsageNode
)
from CHTL.symbol_table import SymbolTable

class AstBuilder(CHTLVisitor):
    def __init__(self, registry: Dict[str, Any], symbol_table: SymbolTable):
        self.registry = registry
        self.symbol_table = symbol_table
        super().__init__()

    def visitDocument(self, ctx:CHTLParser.DocumentContext):
        children = []
        if ctx.children:
            for child in ctx.children:
                if isinstance(child, (CHTLParser.DefinitionContext, CHTLParser.ElementContext)):
                    node = self.visit(child)
                    if isinstance(node, (TemplateDefinitionNode, CustomDefinitionNode)):
                        self.symbol_table.define(node)
                    elif node:
                        children.append(node)

        doc = DocumentNode(children=children)
        # Style processing is now handled by the TemplateExpander
        return doc

    def visitDefinition(self, ctx:CHTLParser.DefinitionContext):
        node_data = self.visit(ctx.getChild(1))
        def_type, name, body = node_data
        if ctx.TEMPLATE():
            return TemplateDefinitionNode(def_type=def_type, name=name, body=body)
        elif ctx.CUSTOM():
            return CustomDefinitionNode(def_type=def_type, name=name, body=body)
        return None

    def visitStyleTemplate(self, ctx:CHTLParser.StyleTemplateContext):
        name = ctx.IDENTIFIER().getText()
        start = ctx.LBRACE().symbol.stop + 1
        stop = ctx.RBRACE().symbol.start - 1
        content = ""
        if start <= stop:
            content = ctx.start.getInputStream().getText(start, stop).strip()
        body = [TextNode(value=content)]
        return ('Style', name, body)

    def visitElementTemplate(self, ctx:CHTLParser.ElementTemplateContext):
        name = ctx.IDENTIFIER().getText()
        body = [self.visit(el) for el in ctx.element()]
        return ('Element', name, body)

    def visitVarTemplate(self, ctx:CHTLParser.VarTemplateContext):
        name = ctx.IDENTIFIER().getText()
        body = [self.visit(attr) for attr in ctx.attribute()]
        return ('Var', name, body)

    def visitElement(self, ctx:CHTLParser.ElementContext):
        tag_name = ctx.IDENTIFIER().getText()
        attributes = []
        children = []
        if ctx.children:
            for child_ctx in ctx.children:
                if isinstance(child_ctx, CHTLParser.AttributeContext):
                    attributes.append(self.visit(child_ctx))
                elif isinstance(child_ctx, CHTLParser.ElementContext):
                    children.append(self.visit(child_ctx))
                elif isinstance(child_ctx, CHTLParser.TextNodeContext):
                    children.append(self.visit(child_ctx))
                elif isinstance(child_ctx, CHTLParser.StylePlaceholderContext):
                    style_node = self.visit(child_ctx)
                    if style_node:
                        children.append(style_node)
                elif isinstance(child_ctx, CHTLParser.ElementUsageContext):
                    children.append(self.visit(child_ctx))
        return ElementNode(tag_name=tag_name, attributes=attributes, children=children)

    def visitElementUsage(self, ctx:CHTLParser.ElementUsageContext):
        name = ctx.IDENTIFIER().getText()
        return ElementUsageNode(name=name)

    def visitAttribute(self, ctx:CHTLParser.AttributeContext):
        name = ctx.IDENTIFIER().getText()
        value = self.visit(ctx.value())
        return AttributeNode(name=name, value=value)

    def visitTextNode(self, ctx:CHTLParser.TextNodeContext):
        value = self.visit(ctx.value())
        return TextNode(value=value)

    def visitStylePlaceholder(self, ctx:CHTLParser.StylePlaceholderContext):
        block_id = ctx.STRING().getText()[1:-1]
        style_data = self.registry.get(block_id)
        if not style_data or style_data.get('type') != 'style':
            return None
        return StyleNode(
            raw_content="", # Raw content is not used in the AST, only for parsing
            inline_styles=style_data.get('inline', ''),
            global_rules=style_data.get('global', []),
            style_usages=style_data.get('usages', [])
        )

    def visitValue(self, ctx:CHTLParser.ValueContext):
        if ctx.STRING():
            return ctx.STRING().getText()[1:-1]
        elif ctx.varUsage():
            return self.visit(ctx.varUsage())
        parts = []
        if ctx.children:
            for child in ctx.children:
                parts.append(child.getText())
        if parts:
            return " ".join(parts)
        return ""

    def visitVarUsage(self, ctx:CHTLParser.VarUsageContext):
        template_name = ctx.IDENTIFIER(0).getText()
        var_name = ctx.IDENTIFIER(1).getText()
        return VarUsageNode(template_name=template_name, var_name=var_name)
