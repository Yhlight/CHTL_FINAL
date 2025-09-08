from nodes import CommentNode, ElementNode, TextNode

class Generator:
    def __init__(self):
        self.output = []
        self.indent_level = -1

    def generate(self, node):
        self.visit(node)
        return "".join(self.output)

    def indent(self):
        return "  " * self.indent_level

    def visit(self, node):
        method_name = f'visit_{node.__class__.__name__}'
        visitor = getattr(self, method_name)
        visitor(node)

    def visit_CommentNode(self, node):
        self.output.append(self.indent() + f"<!-- {node.value} -->\n")

    def visit_ElementNode(self, node):
        if node.tag_name == '_root_':
            self.indent_level += 1
            for child in node.children:
                self.visit(child)
            self.indent_level -= 1
            return

        # Render opening tag with attributes
        self.output.append(self.indent() + f"<{node.tag_name}")
        for key, val in node.attributes.items():
            self.output.append(f' {key}="{val}"')
        self.output.append(">\n")

        self.indent_level += 1
        for child in node.children:
            self.visit(child)
        self.indent_level -= 1

        self.output.append(self.indent() + f"</{node.tag_name}>\n")

    def visit_TextNode(self, node):
        self.output.append(self.indent() + node.value + "\n")
