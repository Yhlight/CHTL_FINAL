import copy
import cssutils
import logging
from CHTL.ast.nodes import (
    AstNode, DocumentNode, ElementNode, AttributeNode, TextNode, StyleNode, StyleUsageNode,
    UsageNode, ElementUsageNode, VarUsageNode, InsertStatementNode, DeleteStatementNode, SelectorNode
)
from CHTL.symbol_table import SymbolTable

class TemplateExpander:
    def __init__(self, symbol_table: SymbolTable):
        self.symbol_table = symbol_table
        cssutils.log.setLevel(logging.CRITICAL)

    def expand(self, node: AstNode):
        self._expand_templates(node)
        # The _process_styles method is now obsolete. Style processing and evaluation
        # is handled by the HtmlGenerator.

    def _expand_templates(self, node: AstNode):
        method_name = f'_visit_and_expand_{node.__class__.__name__.lower()}'
        visitor_method = getattr(self, method_name, self._visit_default)
        return visitor_method(node)

    def _visit_default(self, node: AstNode):
        # We don't want to recurse into StyleNodes from the default visitor
        if hasattr(node, 'children') and not isinstance(node, StyleNode):
            for child in node.children:
                self._expand_templates(child)

    def _visit_and_expand_documentnode(self, node: DocumentNode):
        i = 0
        while i < len(node.children):
            child = node.children[i]
            if isinstance(child, ElementUsageNode):
                expanded_nodes = self._expand_element_usage(child)
                node.children[i:i+1] = expanded_nodes
                for expanded_node in expanded_nodes:
                    self._expand_templates(expanded_node)
                i += len(expanded_nodes)
            else:
                self._expand_templates(child)
                i += 1

    def _visit_and_expand_elementnode(self, node: ElementNode):
        i = 0
        while i < len(node.children):
            child = node.children[i]
            if isinstance(child, ElementUsageNode):
                expanded_nodes = self._expand_element_usage(child)
                node.children[i:i+1] = expanded_nodes
                for expanded_node in expanded_nodes:
                    self._expand_templates(expanded_node)
                i += len(expanded_nodes)
            else:
                self._expand_templates(child)
                i += 1

        for attr in node.attributes:
            if isinstance(attr.value, VarUsageNode):
                attr.value = self._expand_var_usage(attr.value)

    def _find_target_index(self, selector: SelectorNode, nodes: list) -> int:
        """Finds the index of a node in a list based on a selector."""
        count = 0
        for i, node in enumerate(nodes):
            if isinstance(node, ElementNode) and node.tag_name == selector.tag_name:
                # If index is specified, match the count. If not, this is the first match.
                if selector.index is None or count == selector.index:
                    return i
                count += 1
        return -1

    def _expand_element_usage(self, node: ElementUsageNode):
        namespace = ".".join(node.from_namespace) if node.from_namespace else 'global'
        template = self.symbol_table.lookup(node.name, 'Element', namespace=namespace)
        if not template:
            return []

        nodes = copy.deepcopy(template.body)

        # Process deletions first
        # We iterate backwards to avoid index shifting issues when deleting
        for spec in reversed(node.specializations):
            if isinstance(spec, DeleteStatementNode):
                idx_to_delete = self._find_target_index(spec.target_selector, nodes)
                if idx_to_delete != -1:
                    del nodes[idx_to_delete]

        # Process insertions
        for spec in node.specializations:
            if isinstance(spec, InsertStatementNode):
                if spec.position == 'at_top':
                    nodes[:0] = spec.elements
                elif spec.position == 'at_bottom':
                    nodes.extend(spec.elements)
                else:
                    idx = self._find_target_index(spec.target_selector, nodes)
                    if idx != -1:
                        if spec.position == 'before':
                            nodes[idx:idx] = spec.elements
                        elif spec.position == 'after':
                            nodes[idx+1:idx+1] = spec.elements
                        elif spec.position == 'replace':
                            nodes[idx:idx+1] = spec.elements
        return nodes

    def _visit_and_expand_stylenode(self, node: StyleNode):
        """
        Expands any @Style usages within a StyleNode, merging properties
        and handling deletions. Modifies the node in-place.
        """
        if not node.style_usages:
            return

        # This import is here to avoid circular dependency at the top level
        from CHTL.css_parser import parse_expression

        # Properties defined in the usage block take highest precedence.
        final_props = node.inline_styles.copy()

        for usage in node.style_usages:
            namespace = ".".join(usage.from_namespace) if usage.from_namespace else 'global'
            template = self.symbol_table.lookup(usage.name, 'Style', namespace=namespace)

            if not template or not isinstance(template.body, list):
                continue

            # Template body is a list of AttributeNodes
            for attr_node in template.body:
                if not isinstance(attr_node, AttributeNode):
                    continue

                # If the property is not already defined, add it from the template.
                if attr_node.name not in final_props:
                    if attr_node.value is not None:
                        # This is a string or a VarUsageNode
                        value_str = attr_node.value
                        if isinstance(value_str, VarUsageNode):
                            value_str = self._expand_var_usage(value_str)

                        # Parse the string value into an expression AST
                        final_props[attr_node.name] = parse_expression(value_str)
                    else:
                        # This is a valueless property. The spec implies the usage block
                        # MUST provide a value. Our current logic already handles this,
                        # because if it's in final_props, we don't get here. If it's not,
                        # it's an error, which we are currently ignoring. This is acceptable.
                        pass

        # Process deletions
        for prop_to_delete in node.deleted_properties:
            if prop_to_delete in final_props:
                del final_props[prop_to_delete]

        # Update the node with the final, merged properties.
        # This implementation only handles inline styles for now.
        node.inline_styles = final_props
        node.global_rules = {} # Clear global rules as they are not handled here yet.
        node.style_usages = [] # Usages have been processed.
        node.deleted_properties = []


    def _expand_var_usage(self, node: VarUsageNode):
        namespace = 'global'
        if node.override_value is not None:
            if isinstance(node.override_value, VarUsageNode):
                return self._expand_var_usage(node.override_value)
            return node.override_value
        var_template = self.symbol_table.lookup(node.template_name, 'Var', namespace=namespace)
        if var_template:
            for attr in var_template.body:
                if attr.name == node.var_name:
                    if isinstance(attr.value, VarUsageNode):
                        return self._expand_var_usage(attr.value)
                    return attr.value
        return f"{node.template_name}({node.var_name})"
