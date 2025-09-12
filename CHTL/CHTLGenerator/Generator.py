from typing import Union, List, Set
from CHTL.CHTLNode.BaseNode import BaseNode
from CHTL.CHTLNode.ElementNode import ElementNode
from CHTL.CHTLNode.TextNode import TextNode
from CHTL.CHTLNode.StyleNode import StyleNode, CssRule
from CHTL.CHTLNode.ExpressionNode import ExpressionNode
from CHTL.CHTLNode.InfixExpressionNode import InfixExpressionNode
from CHTL.CHTLNode.NumericLiteralNode import NumericLiteralNode
from CHTL.CHTLNode.PropertyReferenceNode import PropertyReferenceNode
import html
import re

class Generator:
    def __init__(self):
        self.self_closing_tags = {"br", "hr", "img", "input", "link", "meta"}
        self.global_css_rules: List[CssRule] = []
        self.element_map: dict[str, ElementNode] = {}
        self.dependency_graph: dict[str, Set[str]] = {}
        self.errors: List[str] = []
        self.evaluation_order: List[str] = []
        self.evaluated_styles: dict[str, dict[str, str]] = {}

    def generate(self, ast_root: ElementNode) -> str:
        if not isinstance(ast_root, ElementNode) or ast_root.tag_name != "root":
            raise ValueError("AST root must be a root ElementNode.")

        self.global_css_rules, self.element_map, self.dependency_graph, self.errors = [], {}, {}, []
        self.evaluation_order, self.evaluated_styles = [], {}

        self._populate_element_map(ast_root)
        self._build_dependency_graph(ast_root)

        if not self._detect_and_sort_dependencies():
            self.errors.append("Circular dependency detected in style properties.")
            return "<h1>Error: Circular Dependency Detected</h1>"

        self._evaluate_all_styles()
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

    def _populate_element_map(self, node: BaseNode):
        if isinstance(node, ElementNode):
            primary_selector = self._get_element_primary_selector(node)
            if primary_selector:
                if primary_selector in self.element_map:
                    self.errors.append(f"Duplicate element selector found: {primary_selector}")
                self.element_map[primary_selector] = node

        if hasattr(node, 'children'):
            for child in node.children:
                self._populate_element_map(child)

    def _build_dependency_graph(self, node: BaseNode):
        if isinstance(node, ElementNode):
            source_selector = self._get_element_primary_selector(node)
            if source_selector:
                for child in node.children:
                    if isinstance(child, StyleNode):
                        self._find_dependencies_in_style_node(child, source_selector)

        if hasattr(node, 'children'):
            for child in node.children:
                self._build_dependency_graph(child)

    def _find_dependencies_in_style_node(self, style_node: StyleNode, source_selector: str):
        for prop_value in style_node.properties.values():
            self._find_refs_in_value_list(prop_value, source_selector)
        for rule in style_node.rules:
            for prop_value in rule.properties.values():
                self._find_refs_in_value_list(prop_value, source_selector)

    def _find_refs_in_value_list(self, value_list: List[BaseNode], source_selector: str):
        for value_node in value_list:
            if isinstance(value_node, PropertyReferenceNode):
                target_selector_str = "".join(t.literal for t in value_node.selector_tokens)
                target_key = self._find_target_key(target_selector_str)
                if target_key:
                    if source_selector not in self.dependency_graph:
                        self.dependency_graph[source_selector] = set()
                    self.dependency_graph[source_selector].add(target_key)
            elif isinstance(value_node, InfixExpressionNode):
                self._find_refs_in_value_list([value_node.left], source_selector)
                self._find_refs_in_value_list([value_node.right], source_selector)

    def _find_target_key(self, selector: str) -> str:
        if selector in self.element_map: return selector
        if f"#{selector}" in self.element_map: return f"#{selector}"
        if f".{selector}" in self.element_map: return f".{selector}"
        return None

    def _detect_and_sort_dependencies(self) -> bool:
        self.evaluation_order = []
        path = set()
        visited = set()

        all_nodes = list(self.element_map.keys())
        for node_key in all_nodes:
            if node_key not in visited:
                if not self._dfs_sort(node_key, path, visited):
                    return False
        return True

    def _dfs_sort(self, node_key: str, path: set, visited: set) -> bool:
        path.add(node_key)
        visited.add(node_key)

        if node_key in self.dependency_graph:
            for neighbor in sorted(list(self.dependency_graph[node_key])):
                if neighbor in path:
                    return False
                if neighbor not in visited:
                    if not self._dfs_sort(neighbor, path, visited):
                        return False

        path.remove(node_key)
        self.evaluation_order.append(node_key)
        return True

    def _evaluate_all_styles(self):
        for selector in self.evaluation_order:
            element_node = self.element_map.get(selector)
            if not element_node: continue

            self.evaluated_styles[selector] = {}
            for child in element_node.children:
                if isinstance(child, StyleNode):
                    for prop_name, prop_value_list in child.properties.items():
                        evaluated_value = self._process_property_value(prop_value_list)
                        self.evaluated_styles[selector][prop_name] = evaluated_value

    def _get_element_primary_selector(self, node: ElementNode) -> str:
        if "id" in node.attributes: return f"#{node.attributes['id']}"
        if "class" in node.attributes: return f".{node.attributes['class'].split()[0]}"
        return None

    def _generate_global_css_string(self) -> str:
        rule_strings = []
        for rule in self.global_css_rules:
            prop_strings = [f"        {key}: {self._process_property_value(value_list)};" for key, value_list in rule.properties.items()]
            prop_string = "\n".join(prop_strings)
            rule_strings.append(f"    {rule.selector} {{\n{prop_string}\n    }}")
        return "\n".join(rule_strings)

    def _process_property_value(self, nodes: list[BaseNode]) -> str:
        parts = []
        for node in nodes:
            evaluated_node = self._evaluate_expression(node)
            if isinstance(evaluated_node, NumericLiteralNode): parts.append(self._format_evaluated_node(evaluated_node))
            elif isinstance(evaluated_node, TextNode): parts.append(evaluated_node.value)
        return " ".join(parts)

    def _evaluate_expression(self, node: BaseNode) -> Union[NumericLiteralNode, TextNode]:
        if isinstance(node, (NumericLiteralNode, TextNode)): return node

        if isinstance(node, PropertyReferenceNode):
            target_selector_str = "".join(t.literal for t in node.selector_tokens)
            target_key = self._find_target_key(target_selector_str)
            target_prop_name = node.property_name

            if not (target_key and target_key in self.evaluated_styles and target_prop_name in self.evaluated_styles[target_key]):
                return TextNode("/* reference-error */")

            final_value_str = self.evaluated_styles[target_key][target_prop_name]

            match = re.match(r"(-?\d+\.?\d*|-?\.\d+)(.*)", final_value_str.strip())
            if not match: return TextNode(final_value_str)
            value_str, unit_str = match.groups()
            return NumericLiteralNode(value=float(value_str), unit=unit_str.strip() or None)

        if not isinstance(node, InfixExpressionNode): return TextNode("/* invalid-expression */")

        left = self._evaluate_expression(node.left)
        right = self._evaluate_expression(node.right)

        if not isinstance(left, NumericLiteralNode) or not isinstance(right, NumericLiteralNode):
             return TextNode("/* non-numeric-calc */")

        unit = left.unit or right.unit
        if left.unit and right.unit and left.unit != right.unit:
            return TextNode("/* unit-mismatch-calc */")

        val = 0.0
        if node.operator == '+': val = left.value + right.value
        elif node.operator == '-': val = left.value - right.value
        elif node.operator == '*': val = left.value * right.value
        elif node.operator == '**': val = left.value ** right.value
        elif node.operator == '/': val = left.value / right.value if right.value != 0 else float('inf')
        elif node.operator == '%': val = left.value % right.value if right.value != 0 else float('inf')

        return NumericLiteralNode(value=val, unit=unit)

    def _format_evaluated_node(self, node: NumericLiteralNode) -> str:
        val = int(node.value) if node.value == int(node.value) else round(node.value, 4)
        return f"{val}{node.unit or ''}"

    def _get_parent_selector(self, parent_node: ElementNode) -> str:
        selector = self._get_element_primary_selector(parent_node)
        if selector: return selector
        for child in parent_node.children:
            if isinstance(child, StyleNode):
                if child.auto_classes_to_add: return f".{child.auto_classes_to_add[0]}"
                if child.auto_ids_to_add: return f"#{child.auto_ids_to_add[0]}"
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
        if isinstance(node, ElementNode): return self._generate_element_node(node)
        elif isinstance(node, TextNode): return self._generate_text_node(node)
        elif isinstance(node, StyleNode): return ""
        return ""

    def _generate_element_node(self, node: ElementNode) -> str:
        tag_name = node.tag_name.lower()

        current_attributes = node.attributes.copy()

        auto_classes, auto_ids = set(), []
        for child in node.children:
            if isinstance(child, StyleNode):
                auto_classes.update(child.auto_classes_to_add)
                auto_ids.extend(child.auto_ids_to_add)
        if auto_classes:
            all_classes = set(current_attributes.get("class", "").split()).union(auto_classes)
            if all_classes: current_attributes["class"] = " ".join(sorted(list(all_classes)))
        if auto_ids and "id" not in current_attributes: current_attributes["id"] = auto_ids[0]

        temp_final_node = ElementNode(tag_name=node.tag_name, attributes=current_attributes)
        primary_selector = self._get_element_primary_selector(temp_final_node)

        inline_styles = self.evaluated_styles.get(primary_selector, {})

        if inline_styles:
            style_string = "; ".join([f"{k}: {v}" for k, v in inline_styles.items() if v])
            if style_string:
                existing_style = current_attributes.get("style", "")
                if existing_style and not existing_style.strip().endswith(';'): existing_style += ";"
                current_attributes["style"] = f"{existing_style} {style_string}".strip()

        attr_parts = [f'{k}="{html.escape(str(v), quote=True)}"' for k, v in current_attributes.items()]
        attr_string = " " + " ".join(attr_parts) if attr_parts else ""

        if tag_name in self.self_closing_tags: return f"<{tag_name}{attr_string}>"
        children_html = "".join(self._generate_node(child) for child in node.children if not isinstance(child, StyleNode))
        return f"<{tag_name}{attr_string}>{children_html}</{tag_name}>"

    def _generate_text_node(self, node: TextNode) -> str:
        return html.escape(node.value)
