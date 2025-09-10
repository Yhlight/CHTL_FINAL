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

    def generate(self) -> str:
        """
        Generates an HTML fragment from the AST.
        It no longer handles creation of the full <html> document structure.
        """
        self.global_styles = []

        # Find and process global style rules to be prepended to the fragment.
        self.find_and_process_styles(self.ast, None)

        html_output = self.visit(self.ast, None, -1)

        if self.global_styles:
            style_block = "\n".join(self.global_styles)
            style_tag = f"<style>\n{style_block}\n</style>"
            html_output = style_tag + "\n" + html_output

        return html_output.strip()

    def visit(self, node: Node, context: Optional[ElementNode], indent_level: int = 0) -> str:
        method_name = f'visit_{type(node).__name__}'
        visitor = getattr(self, method_name, self.generic_visit)
        return visitor(node, context, indent_level)

    def generic_visit(self, node: Node, context: Optional[ElementNode], indent_level: int):
        raise Exception(f'No visit_{type(node).__name__} method for {type(node)}')

    def find_and_process_styles(self, node: Node, context: Optional[ElementNode]):
        # This method's purpose is to find global CSS rules and add them to self.global_styles
        if isinstance(node, StyleNode):
            for style_child in node.children:
                if isinstance(style_child, CssRuleNode):
                    # Indent level 1 for rules inside the <style> tag
                    self.global_styles.append(self.visit(style_child, context, 1))
        elif hasattr(node, 'children'):
            # This search should only apply to top-level nodes in the document
            if isinstance(node, DocumentNode):
                for child in node.children:
                    # We don't recurse into elements, only find top-level style nodes
                    if isinstance(child, StyleNode):
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
        # Nested style blocks with only properties are handled in visit_ElementNode.
        # Global style blocks with rules are handled by find_and_process_styles.
        # So this node itself doesn't render anything directly.
        pass

    def visit_CssRuleNode(self, node: CssRuleNode, context: Optional[ElementNode], indent_level: int) -> str:
        indent = "  " * indent_level
        # The context for properties inside a global rule is None as per spec.
        props = "\n".join([self.visit(prop, None, indent_level + 1) for prop in node.properties])
        return f"{indent}{node.selector} {{\n{props}\n{indent}}}"

    def visit_CssPropertyNode(self, node: CssPropertyNode, context: Optional[ElementNode], indent_level: int) -> str:
        value = node.value
        if isinstance(value, ExpressionNode):
            # We must have a context to evaluate expressions that might reference other properties
            if context is None and not isinstance(value, LiteralNode):
                 raise ValueError("Cannot evaluate complex expressions in a global scope.")
            value = self.evaluator.evaluate(value, context)
        prop_str = f"{node.name}: {value};"
        return f"{'  ' * indent_level}{prop_str}" if indent_level > 0 else prop_str
