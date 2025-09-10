from CHTL.CHTLNode import nodes
from CHTL.CHTLLexer.token_type import TokenType
from CHTL.CHTLGenerator.evaluator import ExpressionEvaluator
import html

class Generator:
    def __init__(self):
        self.self_closing_tags = {'area', 'base', 'br', 'col', 'embed', 'hr', 'img', 'input', 'link', 'meta', 'param', 'source', 'track', 'wbr'}
        self.global_styles = []
        self.templates = {}
        self.evaluator = None
        self.html_output = ""

    def generate(self, ast: nodes.ProgramNode) -> str:
        self.global_styles = []
        self.templates = {}
        self.evaluator = ExpressionEvaluator(ast)

        # Pre-passes
        self._collect_templates(ast)

        # Main pass
        self.html_output = self._visit(ast)

        # Post-pass injection
        if self.global_styles:
            style_block = "<style>\n" + "\n".join(self.global_styles) + "\n</style>"
            if "<head>" in self.html_output.lower():
                 self.html_output = self.html_output.replace("</head>", f"{style_block}\n</head>", 1)
            else:
                self.html_output = style_block + self.html_output

        return self.html_output

    def _collect_templates(self, node):
        """Pre-pass to find all template definitions and store them."""
        if isinstance(node, nodes.TemplateDefinitionNode):
            # Store the node in the repo, keyed by type and name
            if node.template_type not in self.templates:
                self.templates[node.template_type] = {}
            self.templates[node.template_type][node.name] = node

        if hasattr(node, 'children'):
            for child in node.children:
                if child: self._collect_templates(child)

    def _expand_style_template(self, usage_node: nodes.TemplateUsageNode, parent_element: nodes.ElementNode):
        """Finds a style template and returns a list of its property nodes."""
        template_def = self.templates.get("Style", {}).get(usage_node.name)
        if not template_def: return []
        # The body of a style template contains StylePropertyNodes
        return [p for p in template_def.body if isinstance(p, nodes.StylePropertyNode)]

    def _visit(self, node, parent_element=None):
        method_name = f'_visit_{type(node).__name__.lower()}'
        visitor = getattr(self, method_name, self._generic_visit)
        return visitor(node, parent_element) if method_name in ['_visit_elementnode', '_visit_stylenode'] else visitor(node)

    def _generic_visit(self, node): raise Exception(f"No visit method for node type: {type(node).__name__}")
    def _visit_programnode(self, node: nodes.ProgramNode) -> str:
        # Filter out template definitions from the main render pass
        children_to_render = [c for c in node.children if c and not isinstance(c, nodes.TemplateDefinitionNode)]
        return "".join(self._visit(child) for child in children_to_render)

    def _visit_elementnode(self, node: nodes.ElementNode, parent_element=None) -> str:
        html_str = f"<{node.tag_name}"
        for attr in node.attributes:
            html_str += f' {attr.name}="{html.escape(attr.value)}"'

        style_node = next((child for child in node.children if isinstance(child, nodes.StyleNode)), None)
        if style_node:
            self._visit_stylenode(style_node, parent_element=node)
            inline_props = [p for p in style_node.children if isinstance(p, nodes.StylePropertyNode)]
            # Check for template usages inside the style block
            for usage in [u for u in style_node.children if isinstance(u, nodes.TemplateUsageNode)]:
                if usage.template_type == "Style":
                    inline_props.extend(self._expand_style_template(usage, node))

            if inline_props:
                style_strings = [f"{p.name}: {self.evaluator.evaluate(p.value_expression, node)}" for p in inline_props]
                html_str += f' style="{html.escape("; ".join(style_strings))}"'

        if node.tag_name in self.self_closing_tags and not any(not isinstance(c, nodes.StyleNode) for c in node.children):
            html_str += " />"
            return html_str

        html_str += ">"
        # Render children, expanding element templates
        for child in node.children:
            if isinstance(child, nodes.TemplateUsageNode) and child.template_type == "Element":
                template_def = self.templates.get("Element", {}).get(child.name)
                if template_def:
                    for template_child in template_def.body:
                        html_str += self._visit(template_child, parent_element=node)
            elif child and not isinstance(child, nodes.StyleNode):
                html_str += self._visit(child, parent_element=node)
        html_str += f"</{node.tag_name}>"
        return html_str

    def _visit_stylenode(self, node: nodes.StyleNode, parent_element=None):
        base_selector = ""
        id_attr = next((attr for attr in parent_element.attributes if attr.name == 'id'), None)
        class_attr = next((attr for attr in parent_element.attributes if attr.name == 'class'), None)
        if id_attr: base_selector = f"#{id_attr.value}"
        elif class_attr: base_selector = f".{class_attr.value.split()[0]}"
        last_full_selector = base_selector

        # Expand any style templates first
        all_children = list(node.children)
        for child in node.children:
            if isinstance(child, nodes.TemplateUsageNode) and child.template_type == "Style":
                all_children.extend(self._expand_style_template(child, parent_element))

        for child in all_children:
            if isinstance(child, nodes.StyleSelectorRuleNode):
                raw_selector = "".join(t.lexeme for t in child.selector_tokens)
                if raw_selector.startswith('&'):
                    final_selector = last_full_selector + raw_selector[1:]
                else:
                    final_selector = f"{base_selector} {raw_selector}".strip()
                last_full_selector = final_selector
                prop_strings = [f"  {p.name}: {self.evaluator.evaluate(p.value_expression, parent_element)};" for p in child.properties]
                self.global_styles.append(f"{final_selector} {{\n" + "\n".join(prop_strings) + "\n}}")
        return ""

    def _visit_textnode(self, node: nodes.TextNode) -> str: return html.escape(node.value)
    def _visit_templateusagenode(self, node: nodes.TemplateUsageNode): return ""
    def _visit_stylepropertynode(self, n): return ""
    def _visit_styleselectorrulenode(self, n): return ""
    def _visit_attributenode(self, n): return ""
    def _visit_expressionnode(self, n): return ""
    def _visit_conditionalnode(self, n): return ""
    def _visit_binaryopnode(self, n): return ""
    def _visit_referencenode(self, n): return ""
    def _visit_literalnode(self, n): return ""
    def _visit_valuewithunitnode(self, n): return ""
    def _visit_templatedefinitionnode(self, n): return ""
