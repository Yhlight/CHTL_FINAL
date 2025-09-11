import html
import re
from CHTL.CHTLNode import nodes
from CHTL.CHTLLexer.token_type import TokenType

class Generator:
    def __init__(self):
        self.self_closing_tags = {'area', 'base', 'br', 'col', 'embed', 'hr', 'img', 'input', 'link', 'meta', 'param', 'source', 'track', 'wbr'}
        self.global_styles = []
        self.ast_root = None

    def generate(self, ast: nodes.ProgramNode) -> str:
        self.ast_root = ast
        self.global_styles = []

        html_output = self._visit(ast)

        if self.global_styles:
            style_content = "\n".join(sorted(list(set(self.global_styles))))
            style_block = f"<style>\n{style_content}\n</style>"
            if '</head>' in html_output.lower():
                html_output = re.sub(r'(</head>)', f'{style_block}\\n\\1', html_output, flags=re.IGNORECASE, count=1)
            else:
                html_output = style_block + "\n" + html_output

        return html_output

    def _visit(self, node, parent_element=None, selector_context=""):
        method_name = f'_visit_{type(node).__name__.lower()}'
        visitor = getattr(self, method_name, lambda n, pe, sc: "")
        return visitor(node, parent_element, selector_context)

    def _visit_programnode(self, node, parent_element, selector_context):
        return "".join(self._visit(child) for child in node.children)

    def _visit_elementnode(self, node, parent_element, selector_context):
        html_str = f"<{node.tag_name}"
        for attr in sorted(node.attributes, key=lambda x: x.name):
            html_str += f' {attr.name}="{html.escape(attr.value.strip("""\'"""))}"'

        style_node = next((c for c in node.children if isinstance(c, nodes.StyleNode)), None)
        if style_node:
            inline_styles = [p for p in style_node.children if isinstance(p, nodes.StylePropertyNode)]
            if inline_styles:
                style_strings = [f"{p.name}: {self._evaluate_expression(p.value_expression, node)}" for p in sorted(inline_styles, key=lambda x: x.name)]
                html_str += f' style="{html.escape("; ".join(style_strings))};"'

        if node.tag_name in self.self_closing_tags and not any(not isinstance(c, nodes.StyleNode) for c in node.children):
            return html_str + " />"
        html_str += ">"

        id_attr = next((a for a in node.attributes if a.name == 'id'), None)
        class_attr = next((a for a in node.attributes if a.name == 'class'), None)
        my_selector = f"#{id_attr.value.strip('\"')}" if id_attr else f".{class_attr.value.strip('\"').split()[0]}" if class_attr else ""

        for child in node.children:
            if isinstance(child, nodes.StyleNode):
                self._visit_stylenode(child, node, my_selector)
            else:
                html_str += self._visit(child, node, my_selector)

        html_str += f"</{node.tag_name}>"
        return html_str

    def _visit_stylenode(self, node, parent_element, selector_context):
        for child in node.children:
            if isinstance(child, nodes.StyleSelectorRuleNode):
                raw_selector = "".join(t.lexeme for t in child.selector_tokens).strip()
                final_selector = raw_selector.replace('&', selector_context) if selector_context else raw_selector
                prop_strings = [f"  {p.name}: {self._evaluate_expression(p.value_expression, parent_element)};" for p in sorted(child.properties, key=lambda x: x.name)]
                self.global_styles.append(f"{final_selector} {{\n" + "\n".join(prop_strings) + "\n}")

    def _visit_textnode(self, node, parent_element, selector_context):
        return html.escape(node.value)

    def _evaluate_expression(self, node, current_element):
        value, unit = self._eval_recursive(node, current_element)
        if isinstance(value, float) and value == int(value): value = int(value)
        return f"{value}{unit or ''}"

    def _eval_recursive(self, node, current_element):
        if isinstance(node, nodes.LiteralNode):
            val = node.value
            if isinstance(val, str):
                # Try to parse number and unit from the literal string
                match = re.match(r'^(-?\d+\.?\d*)\s*(.*)$', val.strip())
                if match:
                    number_part, unit_part = match.groups()
                    return (float(number_part), unit_part.strip() or None)
            return (val, None)

        if isinstance(node, nodes.ValueWithUnitNode):
            return (node.value, node.unit)

        if isinstance(node, nodes.ReferenceNode):
            target_element = self._find_node(node.selector_tokens) if node.selector_tokens else current_element
            if target_element:
                style_node = next((c for c in target_element.children if isinstance(c, nodes.StyleNode)), None)
                if style_node:
                    prop_node = next((p for p in style_node.children if isinstance(p, nodes.StylePropertyNode) and p.name == node.property_name), None)
                    if prop_node:
                        # Found a valid property reference, evaluate it
                        return self._eval_recursive(prop_node.value_expression, target_element)

            # If we're here, no property was found. Assume it's a literal string value.
            return (node.property_name, None)

        if isinstance(node, nodes.ConditionalNode):
            cond_val, _ = self._eval_recursive(node.condition, current_element)
            if isinstance(cond_val, (int,float)): cond_val = bool(cond_val)
            return self._eval_recursive(node.true_branch if cond_val else node.false_branch, current_element)

        if isinstance(node, nodes.BinaryOpNode):
            left_val, left_unit = self._eval_recursive(node.left, current_element)
            right_val, right_unit = self._eval_recursive(node.right, current_element)

            op = node.op.type
            if op in [TokenType.AMPERSAND_AMPERSAND, TokenType.PIPE_PIPE, TokenType.GREATER, TokenType.GREATER_EQUAL, TokenType.LESS, TokenType.LESS_EQUAL, TokenType.EQUAL_EQUAL, TokenType.BANG_EQUAL]:
                if isinstance(left_val, str): left_val = float(left_val) if left_val.isnumeric() else 0
                if isinstance(right_val, str): right_val = float(right_val) if right_val.isnumeric() else 0

            if op == TokenType.AMPERSAND_AMPERSAND: return (bool(left_val) and bool(right_val), None)
            if op == TokenType.PIPE_PIPE: return (bool(left_val) or bool(right_val), None)
            if op == TokenType.GREATER: return (left_val > right_val, None)
            if op == TokenType.GREATER_EQUAL: return (left_val >= right_val, None)
            if op == TokenType.LESS: return (left_val < right_val, None)
            if op == TokenType.LESS_EQUAL: return (left_val <= right_val, None)
            if op == TokenType.EQUAL_EQUAL: return (left_val == right_val, None)
            if op == TokenType.BANG_EQUAL: return (left_val != right_val, None)

            if left_unit and right_unit and left_unit != right_unit: return ("(error: incompatible units)", None)
            unit = left_unit or right_unit

            if op == TokenType.PLUS: res = left_val + right_val
            elif op == TokenType.MINUS: res = left_val - right_val
            elif op == TokenType.STAR: res = left_val * right_val
            elif op == TokenType.SLASH: res = left_val / right_val if right_val != 0 else 0
            elif op == TokenType.STAR_STAR: res = left_val ** right_val
            elif op == TokenType.PERCENT: res = left_val % right_val if right_val != 0 else 0
            else: res = 0
            return (res, unit)
        return (None, None)

    def _find_node(self, selector_tokens: list) -> nodes.ElementNode:
        if not selector_tokens: return None
        selector_type = selector_tokens[0].type
        selector_name = selector_tokens[1].lexeme

        def traverse(root):
            if isinstance(root, nodes.ElementNode):
                if selector_type == TokenType.HASH:
                    id_attr = next((a for a in root.attributes if a.name == 'id'), None)
                    if id_attr and id_attr.value.strip("'\"") == selector_name: return root
                elif selector_type == TokenType.DOT:
                    class_attr = next((a for a in root.attributes if a.name == 'class'), None)
                    if class_attr and selector_name in class_attr.value.strip("'\"").split(): return root
            if hasattr(root, 'children'):
                for child in root.children:
                    if child and (found := traverse(child)): return found
            return None
        return traverse(self.ast_root)
