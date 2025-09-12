from .CHTLNode.nodes import RootNode, ElementNode, TemplateDefinitionNode, BaseNode

class TemplateRegistry:
    def __init__(self, root: RootNode):
        self.root = root
        self.templates = {}
        self._build()

    def _walk(self, node: BaseNode):
        if isinstance(node, TemplateDefinitionNode):
            template_type = node.template_type
            if template_type not in self.templates:
                self.templates[template_type] = {}
            self.templates[template_type][node.name] = node

        if hasattr(node, 'children'):
            for child in node.children:
                self._walk(child)

    def _build(self):
        self._walk(self.root)

    def find(self, template_type: str, name: str) -> TemplateDefinitionNode | None:
        return self.templates.get(template_type, {}).get(name)
