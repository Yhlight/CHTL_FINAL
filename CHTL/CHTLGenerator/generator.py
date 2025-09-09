from CHTL.CHTLNode.nodes import (
    DocumentNode, ElementNode, AttributeNode, TextNode, CommentNode, Node,
    StyleNode, CssRuleNode, CssPropertyNode, ExpressionNode, LiteralNode, OriginNode
)
from CHTL.CHTLevaluator.evaluator import ExpressionEvaluator
import html
from typing import Optional

class HTMLGenerator:
    def __init__(self, ast: DocumentNode):
        self.ast = ast
        self.evaluator = ExpressionEvaluator(ast)
        self.global_styles = []
        self.self_closing_tags = {
            'area', 'base', 'br', 'col', 'embed', 'hr', 'img', 'input',
            'link', 'meta', 'param', 'source', 'track', 'wbr'
        }

    def generate(self, use_default_structure: bool = False) -> str:
        self.global_styles = []

        self.find_and_process_styles(self.ast, None)

        if not use_default_structure:
            html_output = self.visit(self.ast, None, -1)
            if self.global_styles:
                style_block = "\n".join(self.global_styles)
                style_tag = f"<style>\n{style_block}\n</style>"
                html_output = style_tag + "\n" + html_output
            return html_output.strip()
        else:
            head_content_parts, body_nodes = [], []
            head_indent = "  " * 1
            title_node = None

            for node in self.ast.children:
                if isinstance(node, ElementNode) and node.tag_name == 'title' and not title_node:
                    title_node = node
                elif isinstance(node, OriginNode) and node.origin_type == '@Style':
                    head_content_parts.append(node.content)
                elif not isinstance(node, StyleNode):
                    body_nodes.append(node)

            if title_node:
                head_content_parts.append(self.visit(title_node, None, 1))

            if self.global_styles:
                style_tag_content = "\n".join(self.global_styles)
                head_content_parts.append(f"{head_indent}<style>\n{style_tag_content}\n{head_indent}</style>")

            head_content = "\n".join(head_content_parts)

            body_content = "\n".join(filter(None, [self.visit(node, None, 1) for node in body_nodes]))

            return f"<!DOCTYPE html>\n<html>\n<head>\n{head_content}\n</head>\n<body>\n{body_content}\n</body>\n</html>"

    def visit(self, node: Node, context: Optional[ElementNode], indent_level: int = 0) -> str:
        method_name = f'visit_{type(node).__name__}'
        visitor = getattr(self, method_name, self.generic_visit)
        return visitor(node, context, indent_level)

    def generic_visit(self, node: Node, context: Optional[ElementNode], indent_level: int):
        raise Exception(f'No visit_{type(node).__name__} method for {type(node)}')

    def find_and_process_styles(self, node: Node, context: Optional[ElementNode]):
        if isinstance(node, StyleNode):
            for style_child in node.children:
                if isinstance(style_child, CssRuleNode):
                    self.global_styles.append(self.visit(style_child, context, 2))
        elif hasattr(node, 'children'):
            if isinstance(node, ElementNode):
                context = node
            for child in node.children:
                self.find_and_process_styles(child, context)

    def visit_DocumentNode(self, node: DocumentNode, context: Optional[ElementNode], indent_level: int) -> str:
        html_parts = [self.visit(child, None, indent_level + 1) for child in node.children]
        return "\n".join(filter(None, html_parts))

    def visit_ElementNode(self, node: ElementNode, context: Optional[ElementNode], indent_level: int) -> str:
        indent = "  " * indent_level if indent_level >= 0 else ""
        inline_styles, children_nodes = [], []
        for child in node.children:
            if isinstance(child, StyleNode):
                for style_child in child.children:
                    if isinstance(style_child, CssPropertyNode):
                        inline_styles.append(self.visit(style_child, node, 0))
            else:
                children_nodes.append(child)

        attrs = " ".join([self.visit(attr, node, 0) for attr in node.attributes])
        if inline_styles:
            attrs = f'{attrs} style="{" ".join(inline_styles)}"'.strip()
        if attrs: attrs = " " + attrs

        result = f"{indent}<{node.tag_name}{attrs}>"
        if node.tag_name in self.self_closing_tags: return result

        if len(children_nodes) == 1 and isinstance(children_nodes[0], TextNode):
            result += self.visit(children_nodes[0], node, 0)
        elif children_nodes:
            result += "\n" + "\n".join(filter(None, [self.visit(child, node, indent_level + 1) for child in children_nodes]))
            result += f"\n{indent}"

        result += f"</{node.tag_name}>"
        return result

    def visit_AttributeNode(self, node: AttributeNode, context: Optional[ElementNode], indent_level: int = 0) -> str:
        return f'{node.name}="{html.escape(node.value)}"'

    def visit_TextNode(self, node: TextNode, context: Optional[ElementNode], indent_level: int = 0) -> str:
        indent = "  " * indent_level if indent_level > 0 else ""
        return indent + html.escape(node.value)

    def visit_CommentNode(self, node: CommentNode, context: Optional[ElementNode], indent_level: int) -> str:
        if node.value.startswith('--'):
            indent = "  " * indent_level if indent_level >= 0 else ""
            return f"{indent}<!-- {node.value[2:].strip()} -->"
        return ""

    def visit_OriginNode(self, node: OriginNode, context: Optional[ElementNode], indent_level: int) -> str:
        indent = "  " * indent_level if indent_level > 0 else ""
        return indent + node.content

    def visit_StyleNode(self, node: StyleNode, context: Optional[ElementNode], indent_level: int = 0):
        pass

    def visit_CssRuleNode(self, node: CssRuleNode, context: Optional[ElementNode], indent_level: int) -> str:
        indent = "  " * indent_level
        props = "\n".join([self.visit(prop, None, indent_level + 1) for prop in node.properties])
        return f"{indent}{node.selector} {{\n{props}\n{indent}}}"

    def visit_CssPropertyNode(self, node: CssPropertyNode, context: Optional[ElementNode], indent_level: int) -> str:
        value = node.value
        if isinstance(value, ExpressionNode):
            if context is None and not isinstance(value, LiteralNode):
                 raise ValueError("Cannot evaluate complex expressions in a global scope.")
            value = self.evaluator.evaluate(value, context)
        prop_str = f"{node.name}: {value};"
        return f"{'  ' * indent_level}{prop_str}" if indent_level > 0 else prop_str
