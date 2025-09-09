from CHTL.CHTLNode.nodes import (
    DocumentNode, ElementNode, AttributeNode, TextNode, CommentNode, Node,
    StyleNode, CssRuleNode, CssPropertyNode
)
import html

class HTMLGenerator:
    def __init__(self, ast: DocumentNode):
        self.ast = ast
        self.global_styles = []
        self.self_closing_tags = {
            'area', 'base', 'br', 'col', 'embed', 'hr', 'img', 'input',
            'link', 'meta', 'param', 'source', 'track', 'wbr'
        }

    def generate(self, use_default_structure: bool = False) -> str:
        self.global_styles = []

        if not use_default_structure:
            # Generate an HTML fragment
            self.find_and_process_styles(self.ast, base_indent=0)
            html_output = self.visit(self.ast, -1)
            if self.global_styles:
                style_block = "\n".join(self.global_styles)
                style_tag = f"<style>\n{style_block}\n</style>"
                html_output = style_tag + "\n" + html_output
            return html_output.strip()
        else:
            # Generate a full HTML document
            self.find_and_process_styles(self.ast, base_indent=1)

            head_content_parts = []
            body_content_parts = []
            head_indent = "  " * 1

            # Separate title and body nodes from the AST
            title_node = None
            body_nodes = []
            for node in self.ast.children:
                if isinstance(node, ElementNode) and node.tag_name == 'title' and not title_node:
                    title_node = node
                # StyleNodes are processed globally, so we don't render them in place
                elif not isinstance(node, StyleNode):
                    body_nodes.append(node)

            # Prepare <head> content
            if title_node:
                head_content_parts.append(self.visit(title_node, 1))

            if self.global_styles:
                style_tag_content = "\n".join(self.global_styles)
                head_content_parts.append(f"{head_indent}<style>\n{style_tag_content}\n{head_indent}</style>")

            head_content = "\n".join(head_content_parts)

            # Prepare <body> content
            for node in body_nodes:
                body_content_parts.append(self.visit(node, 1))
            body_content = "\n".join(filter(None, body_content_parts))

            # Assemble the final document
            return f"""<!DOCTYPE html>
<html>
<head>
{head_content}
</head>
<body>
{body_content}
</body>
</html>"""

    def visit(self, node: Node, indent_level: int = 0) -> str:
        method_name = f'visit_{type(node).__name__}'
        visitor = getattr(self, method_name, self.generic_visit)
        return visitor(node, indent_level)

    def generic_visit(self, node: Node, indent_level: int):
        raise Exception(f'No visit_{type(node).__name__} method')

    def find_and_process_styles(self, node: Node, base_indent: int = 0):
        if isinstance(node, StyleNode):
            for style_child in node.children:
                if isinstance(style_child, CssRuleNode):
                    # Render rule with indentation appropriate for its context (fragment vs. full doc)
                    self.global_styles.append(self.visit(style_child, base_indent + 1))
        elif hasattr(node, 'children'):
            for child in node.children:
                self.find_and_process_styles(child, base_indent)

    def visit_DocumentNode(self, node: DocumentNode, indent_level: int) -> str:
        html_parts = [self.visit(child, indent_level + 1) for child in node.children]
        return "\n".join(filter(None, html_parts))

    def visit_ElementNode(self, node: ElementNode, indent_level: int) -> str:
        indent = "  " * indent_level if indent_level >= 0 else ""

        inline_styles = []
        children_nodes = []
        for child in node.children:
            if isinstance(child, StyleNode):
                for style_child in child.children:
                    if isinstance(style_child, CssPropertyNode):
                        inline_styles.append(self.visit(style_child))
            else:
                children_nodes.append(child)

        attrs = " ".join([self.visit(attr) for attr in node.attributes])
        if inline_styles:
            style_attr = f'style="{" ".join(inline_styles)}"'
            attrs = f"{attrs} {style_attr}".strip()

        if attrs:
            attrs = " " + attrs

        result = f"{indent}<{node.tag_name}{attrs}>"

        if node.tag_name in self.self_closing_tags:
            return result

        # Optimization for elements with only a single text node child
        if len(children_nodes) == 1 and isinstance(children_nodes[0], TextNode):
            result += self.visit(children_nodes[0])
        elif children_nodes:
            result += "\n"
            children_html = [self.visit(child, indent_level + 1) for child in children_nodes]
            result += "\n".join(filter(None, children_html))
            result += f"\n{indent}"

        result += f"</{node.tag_name}>"
        return result

    def visit_AttributeNode(self, node: AttributeNode, indent_level: int = 0) -> str:
        return f'{node.name}="{html.escape(node.value)}"'

    def visit_TextNode(self, node: TextNode, indent_level: int = 0) -> str:
        # Indent text if it's on its own line
        indent = "  " * indent_level if indent_level > 0 else ""
        return indent + html.escape(node.value)

    def visit_CommentNode(self, node: CommentNode, indent_level: int) -> str:
        # CHTL -- comments are rendered as HTML comments
        if node.value.startswith('--'):
            indent = "  " * indent_level if indent_level >= 0 else ""
            comment_content = node.value[2:].strip()
            return f"{indent}<!-- {comment_content} -->"
        # CHTL // and /**/ comments are ignored in the output
        return ""

    def visit_StyleNode(self, node: StyleNode, indent_level: int = 0):
        # StyleNodes are handled by find_and_process_styles, so this visit method does nothing.
        pass

    def visit_CssRuleNode(self, node: CssRuleNode, indent_level: int) -> str:
        indent = "  " * indent_level
        props = "\n".join([self.visit(prop, indent_level + 1) for prop in node.properties])
        return f"{indent}{node.selector} {{\n{props}\n{indent}}}"

    def visit_CssPropertyNode(self, node: CssPropertyNode, indent_level: int = 0) -> str:
        # For inline styles
        if indent_level == 0:
            return f"{node.name}: {node.value};"
        # For global styles
        indent = "  " * indent_level
        return f"{indent}{node.name}: {node.value};"
