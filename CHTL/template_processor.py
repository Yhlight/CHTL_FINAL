import copy
from .CHTLNode.nodes import (
    BaseNode, RootNode, ElementNode, StyleNode, TemplateUsageNode
)
from .template_registry import TemplateRegistry

class TemplateProcessor:
    def __init__(self, root: RootNode, registry: TemplateRegistry):
        self.root = root
        self.registry = registry

    def process(self) -> RootNode:
        return self._visit(self.root)

    def _visit(self, node: BaseNode):
        method_name = f'_visit_{node.__class__.__name__}'
        visitor = getattr(self, method_name, self._generic_visit)
        return visitor(node)

    def _generic_visit(self, node: BaseNode):
        return node

    def _visit_RootNode(self, node: RootNode) -> RootNode:
        new_children = []
        for child in node.children:
            result = self._visit(child)
            if isinstance(result, list): new_children.extend(result)
            else: new_children.append(result)
        return RootNode(children=new_children)

    def _visit_ElementNode(self, node: ElementNode) -> ElementNode:
        new_children = []
        for child in node.children:
            result = self._visit(child)
            if isinstance(result, list): new_children.extend(result)
            else: new_children.append(result)
        return ElementNode(tag_name=node.tag_name, attributes=node.attributes, children=new_children)

    def _visit_StyleNode(self, node: StyleNode) -> StyleNode:
        new_rules = []
        for rule in node.rules:
            result = self._visit(rule)
            if isinstance(result, list): new_rules.extend(result)
            else: new_rules.append(result)
        return StyleNode(rules=new_rules)

    def _visit_TemplateUsageNode(self, node: TemplateUsageNode) -> list:
        template = self.registry.find(node.template_type, node.name)
        if not template: raise Exception(f"Template '{node.name}' not found.")

        processed_body = []
        for body_node in template.body:
            result = self._visit(copy.deepcopy(body_node))
            if isinstance(result, list): processed_body.extend(result)
            else: processed_body.append(result)
        return processed_body
