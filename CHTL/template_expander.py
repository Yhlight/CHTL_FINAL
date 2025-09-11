import copy
from CHTL.ast.nodes import (
    AstNode, DocumentNode, ElementNode, AttributeNode, TextNode, StyleNode, StyleUsageNode,
    UsageNode, ElementUsageNode, VarUsageNode
)
from CHTL.symbol_table import SymbolTable
from CHTL.css_parser import parse_style_content

class TemplateExpander:
    """
    Walks the AST and expands all template usages (@Element, @Var, @Style).
    This is a transformative pass that modifies the AST in place.
    """
    def __init__(self, symbol_table: SymbolTable):
        self.symbol_table = symbol_table

    def expand(self, node: AstNode):
        """Public method to start the expansion and style processing."""
        self._expand_templates(node)
        self._process_styles(node) # New style processing pass

    def _expand_templates(self, node: AstNode):
        """Recursively expands @Element and @Var usages."""
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
                # Re-visit the newly inserted nodes
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
        if template:
            return copy.deepcopy(template.body)
        return []

    def _expand_var_usage(self, node: VarUsageNode):
        var_template = self.symbol_table.lookup(node.template_name, 'Var')
        if var_template:
            for attr in var_template.body:
                if attr.name == node.var_name:
                    return attr.value
        return f"{node.template_name}({node.var_name})"

    def _process_styles(self, node, global_rules_list=None):
        if isinstance(node, DocumentNode):
            global_rules_list = []

        if isinstance(node, ElementNode):
            child_inline_styles = []
            for child in node.children:
                if isinstance(child, StyleNode):
                    # Expand @Style usages within the style block
                    self._expand_style_usages(child)

                    if child.inline_styles:
                        child_inline_styles.append(child.inline_styles)
                    if child.global_rules:
                        global_rules_list.extend(child.global_rules)

            if child_inline_styles:
                # ... (merging logic from ast_builder) ...
                existing_style_attr = None
                for attr in node.attributes:
                    if attr.name.lower() == 'style':
                        existing_style_attr = attr
                        break
                if existing_style_attr:
                    existing_style_attr.value = "; ".join(child_inline_styles) + "; " + existing_style_attr.value
                else:
                    node.attributes.append(AttributeNode(name='style', value="; ".join(child_inline_styles)))

        if hasattr(node, 'children'):
            for child in node.children:
                self._process_styles(child, global_rules_list)

        if isinstance(node, DocumentNode):
            node.global_rules = global_rules_list

    def _expand_style_usages(self, style_node: StyleNode):
        """
        Expands @Style usages within a StyleNode by looking up the template
        and prepending its content to the node's inline styles.
        """
        if not style_node.style_usages:
            return

        expanded_styles = []
        for usage in style_node.style_usages:
            template = self.symbol_table.lookup(usage.name, 'Style')
            if template and template.body and isinstance(template.body[0], TextNode):
                # The body of a style template is its raw text content
                expanded_styles.append(template.body[0].value)

        # Prepend the expanded styles to the existing inline styles
        all_inline_styles = expanded_styles
        if style_node.inline_styles:
            all_inline_styles.append(style_node.inline_styles)

        # Clean up parts before joining to avoid double semicolons
        cleaned_styles = [s.strip().removesuffix(';') for s in all_inline_styles if s.strip()]
        style_node.inline_styles = "; ".join(filter(None, cleaned_styles))

        # Mark the usages as expanded
        style_node.style_usages = []
