from typing import Union, List, Set
from CHTL.CHTLNode.BaseNode import BaseNode
from CHTL.CHTLNode.ElementNode import ElementNode
from CHTL.CHTLNode.TextNode import TextNode
from CHTL.CHTLNode.StyleNode import StyleNode, CssRule
from CHTL.CHTLNode.ExpressionNode import ExpressionNode
from CHTL.CHTLNode.InfixExpressionNode import InfixExpressionNode
from CHTL.CHTLNode.NumericLiteralNode import NumericLiteralNode
from CHTL.CHTLNode.PropertyReferenceNode import PropertyReferenceNode
from CHTL.CHTLNode.ScriptNode import ScriptNode
from CHTL.CHTLNode.TemplateNode import TemplateNode
from CHTL.CHTLNode.TemplateUsageNode import TemplateUsageNode
from CHTL.CHTLNode.FunctionCallNode import FunctionCallNode
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
        self.style_templates: dict[str, dict] = {}
        self.element_templates: dict[str, list] = {}
        self.var_groups: dict[str, dict] = {}

    def generate(self, ast_root: ElementNode) -> str:
        if not isinstance(ast_root, ElementNode) or ast_root.tag_name != "root":
            raise ValueError("AST root must be a root ElementNode.")

        self.global_css_rules, self.element_map, self.dependency_graph, self.errors = [], {}, {}, []
        self.evaluation_order, self.evaluated_styles, self.style_templates = [], {}, {}
        self.element_templates, self.var_groups = {}, {}

        self._collect_templates(ast_root)
        self._expand_element_templates(ast_root)

        # Filter out TemplateNode definitions from the root so they are not processed further
        ast_root.children = [child for child in ast_root.children if not isinstance(child, TemplateNode)]

        self._populate_element_map(ast_root)
        self._build_dependency_graph(ast_root)

        if not self._detect_and_sort_dependencies():
            self.errors.append("Circular dependency detected in style properties.")
            return "<h1>Error: Circular Dependency Detected</h1>"

        self._evaluate_all_styles()
        self._collect_global_styles(ast_root, parent=None)

        print("--- Children before final generation ---")
        for child in ast_root.children:
            print(child)

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

            # Start with an empty dict for this element's combined styles
            final_properties = {}

            for child in element_node.children:
                if isinstance(child, StyleNode):
                    # 1. Apply templates first (in order of appearance)
                    for usage in child.template_usages:
                        if usage.template_type == 'Style' and usage.template_name in self.style_templates:
                            template_props = self.style_templates[usage.template_name]
                            final_properties.update(template_props)

                    # 2. Apply the element's own properties, overriding any from templates
                    final_properties.update(child.properties)

            # Now evaluate the combined properties
            self.evaluated_styles[selector] = {}
            for prop_name, prop_value_list in final_properties.items():
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

        if isinstance(node, FunctionCallNode):
            # Assuming function is an identifier (TextNode) for now
            if not isinstance(node.function, TextNode):
                return TextNode("/* invalid-var-group-name */")
            group_name = node.function.value

            if group_name not in self.var_groups:
                return TextNode(f"/* undefined-var-group:{group_name} */")

            if len(node.arguments) != 1 or not isinstance(node.arguments[0], TextNode):
                return TextNode("/* invalid-var-argument */")
            var_name = node.arguments[0].value

            var_group = self.var_groups[group_name]
            if var_name not in var_group:
                return TextNode(f"/* undefined-var:{var_name} */")

            value_str = var_group[var_name]

            match = re.match(r"(-?\d+\.?\d*|-?\.\d+)(.*)", value_str.strip())
            if not match:
                return TextNode(value=value_str)

            num_str, unit = match.groups()
            return NumericLiteralNode(value=float(num_str), unit=unit.strip() or None)

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

    def _collect_templates(self, node: BaseNode):
        if isinstance(node, TemplateNode):
            if node.template_type == 'Style':
                if node.template_name in self.style_templates:
                    self.errors.append(f"Duplicate style template name found: {node.template_name}")
                else:
                    self.style_templates[node.template_name] = node.content
            elif node.template_type == 'Element':
                if node.template_name in self.element_templates:
                    self.errors.append(f"Duplicate element template name found: {node.template_name}")
                else:
                    self.element_templates[node.template_name] = node.content
            elif node.template_type == 'Var':
                if node.template_name in self.var_groups:
                    self.errors.append(f"Duplicate var group name found: {node.template_name}")
                else:
                    self.var_groups[node.template_name] = node.content

        # Recurse, but not into the content of a template definition
        if hasattr(node, 'children') and not isinstance(node, TemplateNode):
            for child in node.children:
                self._collect_templates(child)

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
        elif isinstance(node, ScriptNode): return self._generate_script_node(node)
        elif isinstance(node, TemplateNode): return "" # Don't render template definitions
        return ""

    def _generate_script_node(self, node: ScriptNode) -> str:
        """Generates a <script> tag from a ScriptNode."""
        # Note: The 'html' library is not used for attributes here because script attributes
        # are often boolean and don't need escaping in the same way. This is a simplification.
        attr_parts = []
        for key, value in node.attributes.items():
            if value is True: # For boolean attributes like 'async'
                attr_parts.append(key)
            else:
                attr_parts.append(f'{key}="{html.escape(str(value), quote=True)}"')

        attr_string = " " + " ".join(attr_parts) if attr_parts else ""

        # Inline script vs. external script
        if node.content:
            # The content is pre-formatted, raw JS. No escaping needed.
            return f"<script{attr_string}>\n{node.content}\n</script>"
        else:
            # External script, e.g., <script src="..."></script>
            return f"<script{attr_string}></script>"

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

        # The 'text' attribute is a special case and should be rendered as a child, not an attribute
        text_content = ""
        if 'text' in current_attributes:
            text_content = html.escape(str(current_attributes['text']))
            del current_attributes['text']

        attr_parts = [f'{k}="{html.escape(str(v), quote=True)}"' for k, v in current_attributes.items()]
        attr_string = " " + " ".join(attr_parts) if attr_parts else ""

        if tag_name in self.self_closing_tags: return f"<{tag_name}{attr_string}>"

        children_html = "".join(self._generate_node(child) for child in node.children if not isinstance(child, StyleNode))

        return f"<{tag_name}{attr_string}>{text_content}{children_html}</{tag_name}>"

    def _generate_text_node(self, node: TextNode) -> str:
        return html.escape(node.value)

    def _expand_element_templates(self, node: BaseNode):
        if not hasattr(node, 'children') or not node.children:
            return

        i = 0
        while i < len(node.children):
            child = node.children[i]

            if isinstance(child, TemplateUsageNode) and child.template_type == 'Element':
                if child.template_name in self.element_templates:
                    import copy
                    template_content = self.element_templates[child.template_name]

                    # Replace the usage node with the content of the template
                    node.children[i:i+1] = copy.deepcopy(template_content)

                    # Adjust index to re-scan the newly inserted nodes
                    i += len(template_content)
                    continue # Continue loop without incrementing i again
                else:
                    self.errors.append(f"Undefined element template used: {child.template_name}")
            else:
                # Recurse into the child
                self._expand_element_templates(child)

            i += 1
