from typing import Dict, Any
from CHTL.generated.CHTLVisitor import CHTLVisitor
from CHTL.generated.CHTLParser import CHTLParser
from CHTL.ast.nodes import (
    DocumentNode, ElementNode, AttributeNode, TextNode, StyleNode, StyleUsageNode,
    TemplateDefinitionNode, CustomDefinitionNode,
    UsageNode, ElementUsageNode, VarUsageNode,
    SpecializationNode, InsertStatementNode, DeleteStatementNode
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
        return DocumentNode(children=children)

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
        # With the simplified grammar, there are no specializations yet.
        return ElementUsageNode(name=name, specializations=[])

    def visitSpecializationBody(self, ctx:CHTLParser.SpecializationBodyContext):
        if ctx.insertStatement():
            return self.visit(ctx.insertStatement())
        elif ctx.deleteStatement():
            return self.visit(ctx.deleteStatement())
        return None

    def visitInsertStatement(self, ctx:CHTLParser.InsertStatementContext):
        elements = [self.visit(el) for el in ctx.element()]
        return InsertStatementNode(elements=elements)

    def visitDeleteStatement(self, ctx:CHTLParser.DeleteStatementContext):
        tag_name = ctx.IDENTIFIER().getText()
        return DeleteStatementNode(tag_name=tag_name)

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
            raw_content="",
            inline_styles=style_data.get('inline', ''),
            global_rules=style_data.get('global', []),
            style_usages=style_data.get('usages', []),
            deleted_properties=style_data.get('deleted', [])
        )

    def visitValue(self, ctx:CHTLParser.ValueContext):
        parts = [self.visit(child) for child in ctx.children]
        if len(parts) == 1 and isinstance(parts[0], VarUsageNode):
            return parts[0]
        str_parts = [str(p) if not isinstance(p, VarUsageNode) else f"{p.template_name}({p.var_name})" for p in parts]
        return "".join(str_parts)

    def visitValuePart(self, ctx:CHTLParser.ValuePartContext):
        if ctx.STRING():
            return ctx.STRING().getText()[1:-1]
        elif ctx.varUsage():
            return self.visit(ctx.varUsage())
        else:
            return ctx.getChild(0).getText()

    def visitVarUsage(self, ctx:CHTLParser.VarUsageContext):
        template_name = ctx.IDENTIFIER(0).getText()
        var_name = ctx.IDENTIFIER(1).getText()
        override_value = None
        if ctx.value():
            override_value = self.visit(ctx.value())
        return VarUsageNode(template_name=template_name, var_name=var_name, override_value=override_value)
