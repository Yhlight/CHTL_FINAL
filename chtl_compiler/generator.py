from .ast_nodes import ProgramNode, ElementNode, TextNode, AttributeNode, StyleBlockNode

class HtmlGenerator:
    def generate(self, node: ProgramNode) -> str:
        if not isinstance(node, ProgramNode):
            return self._visit(node)

        return "".join(self._visit(child) for child in node.children)

    def _visit(self, node):
        if node is None:
            return ""
        method_name = f'_visit_{node.__class__.__name__}'
        visitor = getattr(self, method_name, self._generic_visit)
        return visitor(node)

    def _generic_visit(self, node):
        raise Exception(f'No _visit_{node.__class__.__name__} method for {node}')

    def _visit_ElementNode(self, node: ElementNode) -> str:
        attributes = " ".join(self._visit(attr) for attr in node.attributes)

        style_str = ""
        if node.style_block:
            props = [f"{prop.name}: {prop.value};" for prop in node.style_block.properties]
            style_str = f' style="{" ".join(props)}"'

        open_tag_parts = [node.tag_name]
        if attributes:
            open_tag_parts.append(attributes)

        open_tag = f"<{' '.join(open_tag_parts)}{style_str}>"

        self_closing_tags = {'area', 'base', 'br', 'col', 'embed', 'hr', 'img', 'input', 'link', 'meta', 'param', 'source', 'track', 'wbr'}
        if node.tag_name.lower() in self_closing_tags:
            return open_tag

        body = "".join(self._visit(child) for child in node.children)

        close_tag = f"</{node.tag_name}>"

        return f"{open_tag}{body}{close_tag}"

    def _visit_AttributeNode(self, node: AttributeNode) -> str:
        return f'{node.name}="{node.value}"'

    def _visit_TextNode(self, node: TextNode) -> str:
        return node.content

    def _visit_StyleBlockNode(self, node: StyleBlockNode) -> str:
        # Style blocks are handled by the ElementNode visitor, so they don't produce output themselves.
        return ""
