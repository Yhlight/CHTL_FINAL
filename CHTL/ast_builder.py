from typing import Dict, Any
from CHTL.generated.CHTLVisitor import CHTLVisitor
from CHTL.generated.CHTLParser import CHTLParser
from CHTL.ast.nodes import DocumentNode, ElementNode, AttributeNode, TextNode, StyleNode
from CHTL.css_parser import parse_style_content

class AstBuilder(CHTLVisitor):
    def __init__(self, registry: Dict[str, Any]):
        self.registry = registry
        super().__init__()

    def visitDocument(self, ctx:CHTLParser.DocumentContext):
        children = [self.visit(element) for element in ctx.element()]
        doc = DocumentNode(children=children)
        self._process_styles(doc)
        return doc

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

        return ElementNode(tag_name=tag_name, attributes=attributes, children=children)

    def visitAttribute(self, ctx:CHTLParser.AttributeContext):
        name = ctx.IDENTIFIER().getText()
        value = self.visit(ctx.value())
        return AttributeNode(name=name, value=value)

    def visitTextNode(self, ctx:CHTLParser.TextNodeContext):
        value = self.visit(ctx.value())
        return TextNode(value=value)

    def visitStylePlaceholder(self, ctx:CHTLParser.StylePlaceholderContext):
        block_id = ctx.STRING().getText()[1:-1]  # Strip quotes
        style_data = self.registry.get(block_id)

        if not style_data or style_data.get('type') != 'style':
            return None

        return StyleNode(
            raw_content="",  # Raw content is no longer needed in the AST
            inline_styles=style_data.get('inline', ''),
            global_rules=style_data.get('global', [])
        )

    def visitValue(self, ctx:CHTLParser.ValueContext):
        if ctx.STRING():
            return ctx.STRING().getText()[1:-1]

        parts = []
        if ctx.children:
            for child in ctx.children:
                parts.append(child.getText())

        if parts:
            return " ".join(parts)

        return ""

    def _process_styles(self, node, global_rules_list=None):
        if isinstance(node, DocumentNode):
            global_rules_list = []

        if isinstance(node, ElementNode):
            child_inline_styles = []
            for child in node.children:
                if isinstance(child, StyleNode):
                    if child.inline_styles:
                        child_inline_styles.append(child.inline_styles)
                    if child.global_rules:
                        global_rules_list.extend(child.global_rules)

            if child_inline_styles:
                existing_style_attr = None
                for attr in node.attributes:
                    if attr.name.lower() == 'style':
                        existing_style_attr = attr
                        break

                if existing_style_attr:
                    new_style_value = "; ".join(child_inline_styles) + "; " + existing_style_attr.value
                    existing_style_attr.value = new_style_value
                else:
                    new_style_attr = AttributeNode(name='style', value="; ".join(child_inline_styles))
                    node.attributes.append(new_style_attr)

        if hasattr(node, 'children'):
            for child in node.children:
                self._process_styles(child, global_rules_list)

        if isinstance(node, DocumentNode):
            node.global_rules = global_rules_list
