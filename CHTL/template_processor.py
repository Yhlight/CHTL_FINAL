import copy
from .CHTLNode.nodes import (
    BaseNode, RootNode, ElementNode, StyleNode, TemplateUsageNode,
    CustomUsageNode, DeleteNode, StylePropertyNode
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
            # Handle CustomUsageNode which might not be in a list context
            if isinstance(child, CustomUsageNode):
                result = self._visit_CustomUsageNode(child)
            else:
                result = self._visit(child)

            if isinstance(result, list): new_children.extend(result)
            else: new_children.append(result)
        return ElementNode(tag_name=node.tag_name, attributes=node.attributes, children=new_children)

    def _visit_StyleNode(self, node: StyleNode) -> StyleNode:
        new_rules = []
        for rule in node.rules:
            if isinstance(rule, CustomUsageNode):
                result = self._visit_CustomUsageNode(rule)
            else:
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

    def _visit_CustomUsageNode(self, node: CustomUsageNode) -> list:
        # 1. Expand the base template/custom definition
        template = self.registry.find(node.template_type, node.name)
        if not template:
            raise Exception(f"Custom definition '{node.name}' not found.")

        expanded_body = []
        for body_node in template.body:
            result = self._visit(copy.deepcopy(body_node))
            if isinstance(result, list):
                expanded_body.extend(result)
            else:
                expanded_body.append(result)

        # 2. Apply specializations from the usage body
        delete_targets = set()
        for rule in node.body:
            if isinstance(rule, DeleteNode):
                delete_targets.update(rule.targets)

        # Filter out deleted properties
        final_body = [
            item for item in expanded_body
            if not (isinstance(item, StylePropertyNode) and item.property_name in delete_targets)
        ]

        # TODO: Handle other specializations like insert, property filling, etc.

        return final_body
