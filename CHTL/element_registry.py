from .CHTLNode.nodes import RootNode, ElementNode, BaseNode

class ElementRegistry:
    def __init__(self, root: RootNode):
        self.root = root
        self.ids = {} # id -> ElementNode
        self.classes = {} # className -> [ElementNode]
        self.tags = {} # tagName -> [ElementNode]
        self._build()

    def _walk(self, node: BaseNode):
        if isinstance(node, ElementNode):
            self.tags.setdefault(node.tag_name, []).append(node)
            for attr in node.attributes:
                if attr.name == 'id':
                    if attr.value in self.ids: pass # Warn later
                    self.ids[attr.value] = node
                elif attr.name == 'class':
                    for cn in attr.value.split():
                        self.classes.setdefault(cn, []).append(node)
            for child in node.children: self._walk(child)
        elif isinstance(node, RootNode):
            for child in node.children: self._walk(child)

    def _build(self):
        self._walk(self.root)

    def find(self, selector: str) -> list[ElementNode]:
        if not selector: return []
        if selector.startswith('#'):
            node = self.ids.get(selector[1:])
            return [node] if node else []
        elif selector.startswith('.'):
            return self.classes.get(selector[1:], [])
        else:
            return self.tags.get(selector, [])
