import copy
from typing import List, Union, Any
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLNode.nodes import (
    BaseNode, DocumentNode, ElementNode, TextNode, CommentNode, StyleNode, Node,
    TemplateDefinitionNode, TemplateUsageNode, CssRuleNode, CssPropertyNode,
    CustomUsageNode, DeleteNode, InsertNode
)

class ASTTransformer:
    def __init__(self, ast: DocumentNode, context: CompilationContext):
        self.ast = ast
        self.context = context

    def transform(self) -> DocumentNode:
        # The import resolver should be run before the transformer.
        # We assume the context is populated with all necessary symbols.
        return self.visit(self.ast)

    def visit(self, node: BaseNode) -> Any:
        method_name = f'visit_{type(node).__name__}'
        visitor = getattr(self, method_name, self.generic_visit)
        return visitor(node)

    def _visit_list(self, node_list: List[BaseNode]) -> List[BaseNode]:
        new_list = []
        i = 0
        while i < len(node_list):
            item = node_list[i]
            result = self.visit(item)
            if isinstance(result, list):
                new_list.extend(result)
            elif result is not None:
                new_list.append(result)
            i += 1
        return new_list

    def generic_visit(self, node: BaseNode) -> BaseNode:
        # Handle nodes with children, making sure to visit them.
        if hasattr(node, 'children') and isinstance(node.children, list):
            node.children = self._visit_list(node.children)
        # Also visit other list attributes like 'properties' or 'body'
        if hasattr(node, 'properties') and isinstance(node.properties, list):
            node.properties = self._visit_list(node.properties)
        if hasattr(node, 'body') and isinstance(node.body, list):
            node.body = self._visit_list(node.body)
        if hasattr(node, 'content') and isinstance(node.content, list):
            node.content = self._visit_list(node.content)
        return node

    def visit_DocumentNode(self, node: DocumentNode) -> DocumentNode:
        # We start traversal from the document node.
        return self.generic_visit(node)

    def visit_TemplateDefinitionNode(self, node: TemplateDefinitionNode) -> None:
        # Template definitions are processed by the ImportResolver and stored
        # in the context. We remove them from the final AST.
        return None

    def visit_TemplateUsageNode(self, node: TemplateUsageNode) -> List[BaseNode]:
        if node.template_type == 'Element':
            template_content = self.context.get_element_template(node.name, node.namespace_from)
        elif node.template_type == 'Style':
            template_content = self.context.get_style_template(node.name, node.namespace_from)
        else:
            return []

        if not template_content:
            namespace_str = ".".join(node.namespace_from) if node.namespace_from else "current scope"
            raise RuntimeError(f"Template '{node.name}' not found in namespace '{namespace_str}'.")

        return copy.deepcopy(template_content)

    def visit_CustomUsageNode(self, node: CustomUsageNode) -> List[BaseNode]:
        # To handle namespaces, we create a temporary TemplateUsageNode
        temp_usage_node = TemplateUsageNode(
            template_type=node.template_type,
            name=node.name,
            namespace_from=getattr(node, 'namespace_from', None) # Assume it might exist
        )
        expanded_nodes = self.visit_TemplateUsageNode(temp_usage_node)

        for rule in node.body:
            if isinstance(rule, DeleteNode):
                self._apply_delete(expanded_nodes, rule)
            elif isinstance(rule, InsertNode):
                self._apply_insert_recursive(expanded_nodes, rule)
            elif isinstance(rule, ElementNode):
                self._apply_style_override_recursive(expanded_nodes, [rule])

        return self._visit_list(expanded_nodes)

    def _apply_delete(self, nodes: List[ElementNode], rule: DeleteNode):
        for target_name in rule.targets:
            for node in nodes:
                if isinstance(node, ElementNode):
                    node.attributes = [attr for attr in node.attributes if attr.name != target_name]

    def _apply_insert_recursive(self, nodes: List[Node], rule: InsertNode) -> bool:
        target_tag = rule.target_selector
        for i, node in enumerate(nodes):
            if isinstance(node, ElementNode):
                if node.tag_name == target_tag:
                    if rule.position == 'after':
                        nodes[i+1:i+1] = rule.content
                        return True
                    # ... other insert positions
                if node.children and self._apply_insert_recursive(node.children, rule):
                    return True
        return False

    def _apply_style_override_recursive(self, target_nodes: List[Node], override_nodes: List[Node]):
        for override_node in override_nodes:
            if not isinstance(override_node, ElementNode): continue
            for target_node in target_nodes:
                if isinstance(target_node, ElementNode) and target_node.tag_name == override_node.tag_name:
                    override_style = next((c for c in override_node.children if isinstance(c, StyleNode)), None)
                    if override_style:
                        target_style = next((c for c in target_node.children if isinstance(c, StyleNode)), None)
                        if not target_style:
                            target_style = StyleNode(lineno=override_style.lineno)
                            target_node.add_child(target_style)
                        target_style.children.extend(override_style.children)
                    if override_node.children and target_node.children:
                        self._apply_style_override_recursive(target_node.children, override_node.children)
                    break
