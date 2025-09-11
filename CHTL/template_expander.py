import copy
import cssutils
import logging
from CHTL.ast.nodes import (
    AstNode, DocumentNode, ElementNode, AttributeNode, TextNode, StyleNode, StyleUsageNode,
    UsageNode, ElementUsageNode, VarUsageNode, InsertStatementNode, DeleteStatementNode
)
from CHTL.symbol_table import SymbolTable

class TemplateExpander:
    def __init__(self, symbol_table: SymbolTable):
        self.symbol_table = symbol_table
        cssutils.log.setLevel(logging.CRITICAL)

    def expand(self, node: AstNode):
        self._expand_templates(node)
        self._process_styles(node)

    def _expand_templates(self, node: AstNode):
        method_name = f'_visit_and_expand_{node.__class__.__name__.lower()}'
        visitor_method = getattr(self, method_name, self._visit_default)
        return visitor_method(node)

    def _visit_default(self, node: AstNode):
        if hasattr(node, 'children'):
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

    def _expand_element_usage(self, node: ElementUsageNode):
        template = self.symbol_table.lookup(node.name, 'Element')
        if not template:
            return []

        # Get the base nodes from the template
        expanded_nodes = copy.deepcopy(template.body)

        # Apply specializations
        for spec in node.specializations:
            if isinstance(spec, DeleteStatementNode):
                # Filter out nodes with the matching tag name
                expanded_nodes = [n for n in expanded_nodes if not (isinstance(n, ElementNode) and n.tag_name == spec.tag_name)]
            elif isinstance(spec, InsertStatementNode):
                # For this simplified version, we just append the new elements
                expanded_nodes.extend(spec.elements)

        return expanded_nodes

    def _expand_var_usage(self, node: VarUsageNode):
        if node.override_value is not None:
            if isinstance(node.override_value, VarUsageNode):
                return self._expand_var_usage(node.override_value)
            return node.override_value
        var_template = self.symbol_table.lookup(node.template_name, 'Var')
        if var_template:
            for attr in var_template.body:
                if attr.name == node.var_name:
                    if isinstance(attr.value, VarUsageNode):
                        return self._expand_var_usage(attr.value)
                    return attr.value
        return f"{node.template_name}({node.var_name})"

    def _process_styles(self, node, global_rules_list=None):
        if isinstance(node, DocumentNode):
            global_rules_list = []
        if isinstance(node, ElementNode):
            child_inline_styles_parts = []
            deleted_in_block = []
            for child in node.children:
                if isinstance(child, StyleNode):
                    expanded_styles = self._expand_style_usages(child)
                    if expanded_styles:
                        child_inline_styles_parts.append(expanded_styles)
                    if child.inline_styles:
                        child_inline_styles_parts.append(child.inline_styles)
                    if child.global_rules:
                        global_rules_list.extend(child.global_rules)
                    if child.deleted_properties:
                        deleted_in_block.extend(child.deleted_properties)

            if child_inline_styles_parts:
                full_style_string = "; ".join(filter(None, child_inline_styles_parts))
                style_decl = cssutils.parseStyle(full_style_string)
                for prop_name in deleted_in_block:
                    style_decl.removeProperty(prop_name)
                final_style_text = style_decl.cssText.replace('\n', ' ').strip()

                existing_style_attr = None
                for attr in node.attributes:
                    if attr.name.lower() == 'style':
                        existing_style_attr = attr
                        break
                if existing_style_attr:
                    existing_style_attr.value = final_style_text + "; " + existing_style_attr.value
                else:
                    if final_style_text:
                        node.attributes.append(AttributeNode(name='style', value=final_style_text))

        if hasattr(node, 'children'):
            for child in node.children:
                self._process_styles(child, global_rules_list)

        if isinstance(node, DocumentNode):
            node.global_rules = global_rules_list

    def _expand_style_usages(self, style_node: StyleNode):
        if not style_node.style_usages:
            return ""
        expanded_styles = []
        for usage in style_node.style_usages:
            template = self.symbol_table.lookup(usage.name, 'Style')
            if template and template.body and isinstance(template.body[0], TextNode):
                expanded_styles.append(template.body[0].value)
        return "; ".join(filter(None, expanded_styles))
