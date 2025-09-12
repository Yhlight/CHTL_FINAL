from typing import Union
from CHTL.CHTLNode import (BaseNode, ElementNode, TextNode, StyleNode, CssRule,
                           ExpressionNode, InfixExpressionNode, NumericLiteralNode)
import html

class Generator:
    """
    The HTML Generator for the CHTL language. It takes an AST and produces
    an HTML string.
    """
    def __init__(self):
        self.self_closing_tags = {"br", "hr", "img", "input", "link", "meta"}
        self.global_css_rules = []

    def generate(self, ast_root: ElementNode) -> str:
        self.global_css_rules = []
        self._collect_global_styles(ast_root, parent=None)
        body_html = "".join(self._generate_node(child) for child in ast_root.children)
        css_string = self._generate_global_css_string()

        return f"""<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>CHTL Document</title>
    <style>
{css_string}
    </style>
</head>
<body>
{body_html}
</body>
</html>"""

    def _generate_global_css_string(self) -> str:
        rule_strings = []
        for rule in self.global_css_rules:
            prop_strings = []
            for key, value_list in rule.properties.items():
                # Process the list of values for each property
                final_value = self._process_property_value(value_list)
                prop_strings.append(f"        {key}: {final_value};")

            prop_string = "\n".join(prop_strings)
            rule_strings.append(f"    {rule.selector} {{\n{prop_string}\n    }}")
        return "\n".join(rule_strings)

    def _process_property_value(self, nodes: list[BaseNode]) -> str:
        """
        Processes a list of value nodes (which can be expressions or literals)
        into a single CSS value string.
        """
        parts = []
        for node in nodes:
            if isinstance(node, ExpressionNode):
                evaluated_node = self._evaluate_expression(node)
                if evaluated_node:
                    parts.append(self._format_evaluated_node(evaluated_node))
            elif isinstance(node, TextNode):
                parts.append(node.value)
        return " ".join(parts)

    def _evaluate_expression(self, node: BaseNode) -> NumericLiteralNode:
        if isinstance(node, NumericLiteralNode):
            return node
        if not isinstance(node, InfixExpressionNode):
            return None

        left = self._evaluate_expression(node.left)
        right = self._evaluate_expression(node.right)

        if not left or not right: return None

        unit = left.unit
        if unit is None: unit = right.unit
        elif right.unit is not None and left.unit != right.unit: return None

        val = 0.0
        if node.operator == '+': val = left.value + right.value
        elif node.operator == '-': val = left.value - right.value
        elif node.operator == '*': val = left.value * right.value
        elif node.operator == '**': val = left.value ** right.value
        elif node.operator == '/': val = left.value / right.value if right.value != 0 else 0
        elif node.operator == '%': val = left.value % right.value if right.value != 0 else 0

        return NumericLiteralNode(value=val, unit=unit)

    def _format_evaluated_node(self, node: NumericLiteralNode) -> str:
        val = int(node.value) if node.value == int(node.value) else round(node.value, 4)
        unit = node.unit or ""
        return f"{val}{unit}"

    def _get_parent_selector(self, parent_node: ElementNode) -> str:
        if "class" in parent_node.attributes:
            return f".{parent_node.attributes['class'].split()[0]}"
        if "id" in parent_node.attributes:
            return f"#{parent_node.attributes['id']}"
        for child in parent_node.children:
            if isinstance(child, StyleNode):
                if child.auto_classes_to_add:
                    return f".{child.auto_classes_to_add[0]}"
                if child.auto_ids_to_add:
                    return f"#{child.auto_ids_to_add[0]}"
        return parent_node.tag_name.lower()

    def _collect_global_styles(self, current_node: BaseNode, parent: Union[ElementNode, None]):
        if isinstance(current_node, StyleNode) and parent:
            parent_selector = self._get_parent_selector(parent)
            for rule in current_node.rules:
                new_selector = rule.selector.replace('&', parent_selector)
                self.global_css_rules.append(CssRule(selector=new_selector, properties=rule.properties))
        if hasattr(current_node, 'children'):
            new_parent = current_node if isinstance(current_node, ElementNode) else parent
            for child in current_node.children:
                self._collect_global_styles(child, parent=new_parent)

    def _generate_node(self, node: BaseNode) -> str:
        if isinstance(node, ElementNode):
            return self._generate_element_node(node)
        elif isinstance(node, TextNode):
            return self._generate_text_node(node)
        elif isinstance(node, StyleNode):
            return ""
        return ""

    def _generate_attributes(self, node: ElementNode) -> str:
        if not node.attributes:
            return ""
        parts = []
        for key, value in node.attributes.items():
            escaped_value = html.escape(str(value), quote=True)
            parts.append(f'{key}="{escaped_value}"')
        return " " + " ".join(parts)

    def _generate_element_node(self, node: ElementNode) -> str:
        tag_name = node.tag_name.lower()
        inline_styles = {}
        auto_classes = set()
        auto_ids = []
        for child in node.children:
            if isinstance(child, StyleNode):
                inline_styles.update(child.properties)
                for c in child.auto_classes_to_add:
                    auto_classes.add(c)
                auto_ids.extend(child.auto_ids_to_add)

        if auto_classes:
            existing_classes = set(node.attributes.get("class", "").split())
            all_classes = existing_classes.union(auto_classes)
            if all_classes:
                node.attributes["class"] = " ".join(sorted(list(all_classes)))
        if auto_ids and "id" not in node.attributes:
            node.attributes["id"] = auto_ids[0]

        if inline_styles:
            style_parts = []
            for key, value_list in inline_styles.items():
                final_value = self._process_property_value(value_list)
                style_parts.append(f"{key}: {final_value}")

            if style_parts:
                style_string = "; ".join(style_parts)
                existing_style = node.attributes.get("style", "")
                if existing_style and not existing_style.strip().endswith(';'):
                    existing_style += ";"
                final_style = f"{existing_style} {style_string}".strip()
                if final_style:
                    node.attributes["style"] = final_style

        attr_string = self._generate_attributes(node)

        if tag_name in self.self_closing_tags:
            return f"<{tag_name}{attr_string}>"

        children_html = "".join(
            self._generate_node(child) for child in node.children if not isinstance(child, StyleNode)
        )

        return f"<{tag_name}{attr_string}>{children_html}</{tag_name}>"

    def _generate_text_node(self, node: TextNode) -> str:
        return html.escape(node.value)
