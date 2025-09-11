from CHTL.ast.nodes import DocumentNode, ElementNode, AttributeNode, TextNode, StyleNode

class HtmlGenerator:
    def visit(self, node):
        """The main entry point for the visitor."""
        return node.accept(self)

    def visit_documentnode(self, node: DocumentNode):
        """Visits the root document node and constructs the full HTML page."""
        body_content = "".join(self.visit(child) for child in node.children)

        head_content = ""
        if node.global_rules:
            # Join all global rules into one block
            global_styles = "\n".join(node.global_rules)
            head_content += f"    <style>\n{global_styles}\n    </style>"

        # Using a simple template for the final HTML document
        return f"""<!DOCTYPE html>
<html>
<head>
{head_content}
</head>
<body>
{body_content}
</body>
</html>"""

    def visit_elementnode(self, node: ElementNode):
        """Visits an element node and generates an HTML element."""
        attrs = " ".join(self.visit(attr) for attr in node.attributes)
        attrs_str = f" {attrs}" if attrs else ""

        # For now, all tags are treated as paired tags.
        # Self-closing tags (e.g., <img>, <br>) will be handled later.
        children_html = "".join(self.visit(child) for child in node.children)

        return f"<{node.tag_name}{attrs_str}>{children_html}</{node.tag_name}>"

    def visit_attributenode(self, node: AttributeNode):
        """Visits an attribute node and generates an HTML attribute string."""
        # Basic HTML escaping for attribute values can be added here.
        # For now, we assume values are safe.
        return f'{node.name}="{node.value}"'

    def visit_textnode(self, node: TextNode):
        """Visits a text node."""
        # Basic HTML escaping for text content (e.g., <, >) can be added here.
        return node.value

    def visit_stylenode(self, node: StyleNode):
        """Visits a style node. For now, this does nothing as styles will be handled separately."""
        return ""

    def visit_default(self, node):
        """Default visitor method for unhandled node types."""
        raise TypeError(f"No visit_{type(node).__name__.lower()} method for node type {type(node).__name__}")
