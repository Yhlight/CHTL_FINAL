import html
from .CHTLNode.nodes import (
    RootNode, ElementNode, TextNode, CommentNode, BaseNode, StyleNode,
    SelectorBlockNode, StylePropertyNode
)
from .evaluator import Evaluator, EvaluatedDimension, EvaluatedString, EvaluatedValue
from .element_registry import ElementRegistry
from .template_registry import TemplateRegistry

class Generator:
    def __init__(self, indent: int = 4):
        self.indent_str = " " * indent
        self.indent_level = 0
        self.element_registry = None
        self.template_registry = None
        self.global_styles = []
        self.self_closing_tags = {
            "area", "base", "br", "col", "embed", "hr", "img", "input",
            "link", "meta", "param", "source", "track", "wbr"
        }

    def generate(self, root: RootNode) -> str:
        self.element_registry = ElementRegistry(root)
        self.template_registry = TemplateRegistry(root)
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

    def _make_indent(self) -> str: return self.indent_str * self.indent_level
    def _visit(self, node: BaseNode, **kwargs):
        visitor = getattr(self, f'_visit_{node.__class__.__name__}', self._generic_visit)
        return visitor(node, **kwargs)
    def _generic_visit(self, node: BaseNode, **kwargs): raise Exception(f'No visit method for {node.__class__.__name__}')
    def _visit_RootNode(self, node: RootNode, **kwargs) -> str: return "\n".join(self._visit(child) for child in node.children)
    def _visit_CommentNode(self, node: CommentNode, **kwargs) -> str: return f"{self._make_indent()}<!-- {node.content} -->"
    def _visit_TextNode(self, node: TextNode, **kwargs) -> str: return f"{self._make_indent()}{html.escape(node.content)}"
    def _visit_TemplateDefinitionNode(self, node, **kwargs): return ""
    def _format_evaluated_value(self, value: EvaluatedValue) -> str:
        if isinstance(value, EvaluatedDimension):
            val_str = str(int(value.value)) if value.value == int(value.value) else str(round(value.value, 2))
            return f"{val_str}{value.unit}"
        elif isinstance(value, EvaluatedString): return value.value
        else: raise Exception(f"Cannot format value of type {type(value)}")

    def _visit_ElementNode(self, node: ElementNode, **kwargs) -> str:
        parts = []
        styles = [n for n in node.children if isinstance(n, StyleNode)]
        other_children = [n for n in node.children if not isinstance(n, StyleNode)]
        rules = [r for s in styles for r in s.rules]
        inline_props = [r for r in rules if isinstance(r, StylePropertyNode)]
        selector_blocks = [r for r in rules if isinstance(r, SelectorBlockNode)]

        style_attr_str = ""
        evaluator = Evaluator(inline_props, self.element_registry, self.template_registry, node)
        if inline_props:
            final_styles = {p.property_name: self._format_evaluated_value(evaluator.evaluate(p.property_name)) for p in inline_props}
            style_attr_str = " ".join([f"{n}: {v};" for n, v in final_styles.items()])

        for sb in selector_blocks: self._visit(sb, context_element=node, evaluator=evaluator)

        attr_str = "".join(f' {a.name}="{html.escape(a.value)}"' for a in node.attributes)
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
        if not evaluator: evaluator = Evaluator(node.properties, self.element_registry, self.template_registry, context_element)

        prop_strings = []
        for prop in node.properties:
            parent_props = evaluator.properties
            combined_props = list(parent_props.values()) + node.properties
            block_evaluator = Evaluator(combined_props, self.element_registry, self.template_registry, context_element)
            final_value = block_evaluator.evaluate(prop.property_name)
            final_str = self._format_evaluated_value(final_value)
            prop_strings.append(f"    {prop.property_name}: {final_str};")

        prop_block = "\n".join(prop_strings)
        rule = f"{node.selector} {{\n{prop_block}\n}}"
        self.global_styles.append(rule)
        return ""
