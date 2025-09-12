import html
from .CHTLNode.nodes import (
    RootNode, ElementNode, TextNode, CommentNode, BaseNode, StyleNode,
    SelectorBlockNode, StylePropertyNode
)
from .evaluator import Evaluator, EvaluatedDimension, EvaluatedString, EvaluatedValue
from .element_registry import ElementRegistry

class Generator:
    def __init__(self, indent: int = 4):
        self.indent_str = " " * indent
        self.indent_level = 0
        self.element_registry = None
        self.global_styles = []
        self.self_closing_tags = {
            "area", "base", "br", "col", "embed", "hr", "img", "input",
            "link", "meta", "param", "source", "track", "wbr"
        }

    def generate(self, root: RootNode) -> str:
        self.element_registry = ElementRegistry(root)
        self.global_styles = []
        body_html = self._visit(root)
        if self.global_styles:
            style_content = "\n".join(self.global_styles)
            style_tag = f"<style>\n{style_content}\n</style>"
            if "</head>" in body_html:
                return body_html.replace("</head>", f"    {style_tag}\n</head>")
            else:
                return f"{style_tag}\n{body_html}"
        return body_html

    def _make_indent(self) -> str:
        return self.indent_str * self.indent_level

    def _visit(self, node: BaseNode, **kwargs):
        method_name = f'_visit_{node.__class__.__name__}'
        visitor = getattr(self, method_name, self._generic_visit)
        return visitor(node, **kwargs)

    def _generic_visit(self, node: BaseNode, **kwargs):
        raise Exception(f'No _visit_{node.__class__.__name__} method')

    def _visit_RootNode(self, node: RootNode, **kwargs) -> str:
        return "\n".join(self._visit(child) for child in node.children)

    def _visit_CommentNode(self, node: CommentNode, **kwargs) -> str:
        return f"{self._make_indent()}<!-- {node.content} -->"

    def _visit_TextNode(self, node: TextNode, **kwargs) -> str:
        return f"{self._make_indent()}{html.escape(node.content)}"

    def _format_evaluated_value(self, value: EvaluatedValue) -> str:
        if isinstance(value, EvaluatedDimension):
            if value.value == int(value.value): val_str = str(int(value.value))
            else: val_str = str(round(value.value, 2))
            return f"{val_str}{value.unit}"
        elif isinstance(value, EvaluatedString): return value.value
        else: raise Exception(f"Cannot format evaluated value of type {type(value)} for CSS.")

    def _visit_ElementNode(self, node: ElementNode, **kwargs) -> str:
        parts = []
        style_nodes = [n for n in node.children if isinstance(n, StyleNode)]
        other_children = [n for n in node.children if not isinstance(n, StyleNode)]

        all_rules = []
        for sn in style_nodes:
            all_rules.extend(sn.rules)

        inline_properties = [r for r in all_rules if isinstance(r, StylePropertyNode)]
        selector_blocks = [r for r in all_rules if isinstance(r, SelectorBlockNode)]

        evaluator = None
        if inline_properties:
            evaluator = Evaluator(inline_properties, self.element_registry, element_context=node)

        style_attr_str = ""
        if evaluator:
            final_styles_dict = {}
            for prop in inline_properties:
                final_value = evaluator.evaluate(prop.property_name)
                final_str = self._format_evaluated_value(final_value)
                final_styles_dict[prop.property_name] = final_str
            final_styles = [f"{name}: {value};" for name, value in final_styles_dict.items()]
            style_attr_str = " ".join(final_styles)

        for sb in selector_blocks:
            self._visit(sb, context_element=node, evaluator=evaluator)

        attr_str = "".join(f' {attr.name}="{html.escape(attr.value)}"' for attr in node.attributes)
        if style_attr_str: attr_str += f' style="{style_attr_str}"'
        opening_tag = f"{self._make_indent()}<{node.tag_name}{attr_str}>"
        if node.tag_name in self.self_closing_tags: return opening_tag
        parts.append(opening_tag)
        self.indent_level += 1
        for child in other_children: parts.append(self._visit(child))
        self.indent_level -= 1
        parts.append(f"{self._make_indent()}</{node.tag_name}>")
        return "\n".join(parts)

    def _visit_SelectorBlockNode(self, node: SelectorBlockNode, context_element=None, evaluator=None):
        if not evaluator:
            # Create a new evaluator if one wasn't passed down.
            # This happens if a selector block is in a style block with no inline properties.
            evaluator = Evaluator(node.properties, self.element_registry, element_context=context_element)

        prop_strings = []
        for prop in node.properties:
            # Need to temporarily add selector block's own properties to evaluator context
            # This is getting complex. A better design would be for the evaluator to handle scopes.
            # For now, I'll create a new evaluator for the block, but pass the parent's memo cache.
            # This is still not quite right.

            # Let's try to add the selector's properties to the existing evaluator.
            # This can pollute the context for other selector blocks.

            # The simplest fix is to initialize a new evaluator with a combined list of properties.
            parent_props = evaluator.properties
            combined_props = list(parent_props.values()) + node.properties
            block_evaluator = Evaluator(combined_props, self.element_registry, element_context=context_element)

            final_value = block_evaluator.evaluate(prop.property_name)
            final_str = self._format_evaluated_value(final_value)
            prop_strings.append(f"    {prop.property_name}: {final_str};")
        prop_block = "\n".join(prop_strings)
        rule = f"{node.selector} {{\n{prop_block}\n}}"
        self.global_styles.append(rule)
        return ""
